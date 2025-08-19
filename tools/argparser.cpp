#include "argparser.h"
#include "plutobook.h"

#include <cassert>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <cstdio>

namespace plutobook {

static void printArg(const ArgDesc* arg, int width)
{
    fprintf(stderr, "  %s", arg->name);

    const char* meta = "";
    if(!arg->positional) {
        switch(arg->type) {
        case ArgType::Flag:
            meta = " <flag>";
            break;
        case ArgType::String:
            meta = " <string>";
            break;
        case ArgType::Int:
            meta = " <int>";
            break;
        case ArgType::Float:
            meta = " <float>";
            break;
        case ArgType::Length:
            meta = " <length>";
            break;
        case ArgType::Choice:
            meta = " <choice>";
            break;
        }
    }

    constexpr int kMaxMetaWidth = 9;

    int columns = kMaxMetaWidth + width - strlen(arg->name);
    fprintf(stderr, "%-*s", columns, meta);
    if(arg->help)
        fprintf(stderr, " %s", arg->help);
    fprintf(stderr, "\n");
}

static void printUsage(const char* program, const char* description, const ArgDesc* args, int status)
{
    int width = 0;
    const ArgDesc* arg = args;
    for(; arg->name; ++arg) {
        int columns = strlen(arg->name);
        if(columns > width) {
            width = columns;
        }
    }

    fprintf(stderr, "%s\n\nUsage: %s ", description, program);
    for(arg = args; arg->name; ++arg) {
        if(arg->positional) {
            fprintf(stderr, "%s ", arg->name);
        }
    }

    fprintf(stderr, "[options]\n\n");
    for(arg = args; arg->name; ++arg) {
        if(arg->positional) {
            printArg(arg, width);
        }
    }

    fprintf(stderr, "\noptions:\n");
    for(arg = args; arg->name; ++arg) {
        if(!arg->positional) {
            printArg(arg, width);
        }
    }

    if(status == EXIT_FAILURE)
        fprintf(stderr, "%s: error: %s\n", program, plutobook_get_error_message());
    exit(status);
}

static bool flag_func(void* closure, const char* value)
{
    *(bool*)(closure) = true;
    return true;
}

static bool string_func(void* closure, const char* value)
{
    *(const char**)(closure) = value;
    return true;
}

static bool int_func(void* closure, const char* value)
{
    char* end;
    long num = strtol(value, &end, 10);
    if(*end) {
        plutobook_set_error_message("invalid int value: '%s'", value);
        return false;
    }

    *(int*)(closure) = num;
    return true;
}

static bool float_func(void* closure, const char* value)
{
    char* end;
    float num = strtof(value, &end);
    if(*end) {
        plutobook_set_error_message("invalid float value: '%s'", value);
        return false;
    }

    *(float*)(closure) = num;
    return true;
}

static bool striequals(const char* a, const char* b)
{
    while(*a && *b) {
        if(tolower(*a) != tolower(*b))
            return false;
        ++a;
        ++b;
    }

    return (*a == '\0' && *b == '\0');
}

static bool length_func(void* closure, const char* value)
{
    char* end;
    float length = strtof(value, &end);
    static const struct {
        const char* unit;
        const float factor;
    } table[] = {
        {"pt", PLUTOBOOK_UNITS_PT},
        {"pc", PLUTOBOOK_UNITS_PC},
        {"in", PLUTOBOOK_UNITS_IN},
        {"cm", PLUTOBOOK_UNITS_CM},
        {"mm", PLUTOBOOK_UNITS_MM},
        {"px", PLUTOBOOK_UNITS_PX}
    };

    for(auto item : table) {
        if(striequals(item.unit, end)) {
            *(float*)(closure) = length * item.factor;
            return true;
        }
    }

    plutobook_set_error_message("invalid length value: '%s'", value);
    return false;
}

static bool is_option(const char* name) { return (name[0] == '-' && name[1]); }

static ArgDesc* findArg(ArgDesc* args, const char* name)
{
    for(int i = 0; args[i].name; ++i) {
        if((args[i].positional && args[i].required && !is_option(name))
            || strcmp(args[i].name, name) == 0) {
            args[i].required = false;
            return args + i;
        }
    }

    return nullptr;
}

void parseArgs(const char* program, const char* description, ArgDesc* args, int argc, char *argv[])
{
    for(int i = 0; args[i].name; ++i) {
        args[i].positional = args[i].required = !is_option(args[i].name);
        if(args[i].func == nullptr) {
            switch(args[i].type) {
            case ArgType::Flag:
                args[i].func = flag_func;
                break;
            case ArgType::String:
                args[i].func = string_func;
                break;
            case ArgType::Int:
                args[i].func = int_func;
                break;
            case ArgType::Float:
                args[i].func = float_func;
                break;
            case ArgType::Length:
                args[i].func = length_func;
                break;
            default:
                assert(false);
            }
        }
    }

    for(int i = 1; i < argc; ++i) {
        auto arg = findArg(args, argv[i]);
        if(arg == nullptr) {
            const char* name = argv[i];
            if(strcmp(name, "-h") == 0 || strcmp(name, "--help") == 0)
                printUsage(program, description, args, EXIT_SUCCESS);
            if(strcmp(name, "-v") == 0 || strcmp(name, "--version") == 0) {
                fprintf(stderr, "%s version %s\n", program, PLUTOBOOK_VERSION_STRING);
                exit(EXIT_SUCCESS);
            }

            plutobook_set_error_message("unrecognized argument: %s", name);
            printUsage(program, description, args, EXIT_FAILURE);
        }

        const char* value = argv[i];
        if(!arg->positional && arg->type != ArgType::Flag) {
            if(i == argc - 1 || is_option(argv[i + 1])) {
                plutobook_set_error_message("argument %s: expected one argument", arg->name);
                printUsage(program, description, args, EXIT_FAILURE);
            }

            value = argv[++i];
        }

        if(!arg->func(arg->value, value)) {
            plutobook_set_error_message("argument %s: %s", arg->name, plutobook_get_error_message());
            printUsage(program, description, args, EXIT_FAILURE);
        }
    }

    int required = 0;
    for(int i = 0; args[i].name; ++i) {
        if(args[i].required) {
            required++;
        }
    }

    if(required > 0) {
        plutobook_set_error_message("the following arguments are required: ");
        for(int i = 0; args[i].name; ++i) {
            if(args[i].required) {
                const char* last_message = plutobook_get_error_message();
                plutobook_set_error_message("%s%s", last_message, args[i].name);
                if(--required) {
                    plutobook_set_error_message("%s%s", last_message, ", ");
                }
            }
        }

        printUsage(program, description, args, EXIT_FAILURE);
    }
}

bool parseArgChoices(void* closure, const char* name, const ArgChoice* choices, int nchoices)
{
    for(int i = 0; i < nchoices; ++i) {
        if(striequals(choices[i].name, name)) {
            *(int*)(closure) = choices[i].value;
            return true;
        }
    }

    plutobook_set_error_message("invalid choice: '%s' (choose from ", name);
    for(int i = 0; i < nchoices; ++i) {
        const char* last_message = plutobook_get_error_message();
        plutobook_set_error_message("%s'%s'%s", last_message, choices[i].name, i != nchoices - 1 ? ", " : ")");
    }

    return false;
}

} // namespace plutobook
