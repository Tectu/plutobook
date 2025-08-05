cmake_minimum_required(VERSION 3.31)

# Project
project(
    plutobook
    VERSION 0.0.1
    LANGUAGES
        CXX
)

# Options
option(PLUTOBOOK_BUILD_EXAMPLES "Build examples."       OFF)
option(PLUTOBOOK_BUILD_STATIC   "Build static library." ON)
option(PLUTOBOOK_BUILD_SHARED   "Build shared library." ON)
option(PLUTOBOOK_BUILD_EXAMPLES "Build the examples."   OFF)
option(PLUTOBOOK_FEATURE_CURL   "Ability to fetch external resources (via curl)." ON)
option(PLUTOBOOK_FEATURE_JPEG   "JPEG support."         ON)

# User settings
set(CMAKE_DEBUG_POSTFIX d)

# Handle external dependencies
include(cmake/external.cmake)

# Include examples (if supposed to)
if (PLUTOBOOK_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Define target names
set(NAME plutobook)
set(TARGET_STATIC ${NAME}-static)
set(TARGET_SHARED ${NAME}-shared)

# Function to setup stuff common to all targets
function(setup_target_common TARGET)
    target_compile_features(
        ${TARGET}
        PUBLIC
            cxx_std_20
    )

    set_target_properties(
        ${TARGET}
        PROPERTIES
            OUTPUT_NAME "plutobook"
            ARCHIVE_OUTPUT_NAME "plutobook"
            VERSION ${PROJECT_VERSION}
            POSITION_INDEPENDENT_CODE ON
    )

    target_include_directories(
        ${TARGET}
        PRIVATE
            include
            source
            source/graphics
            source/layout
            source/resource
    )

    target_compile_definitions(
        ${TARGET}
        PUBLIC
            $<$<BOOL:${PLUTOBOOK_FEATURE_CURL}>:PLUTOBOOK_HAS_CURL>
            $<$<BOOL:${PLUTOBOOK_FEATURE_JPEG}>:PLUTOBOOK_HAS_TURBOJPEG>
    )

    target_link_libraries(
        ${TARGET}
        PUBLIC
            EXPAT::EXPAT
            harfbuzz::harfbuzz
            ICU::uc
            Fontconfig::Fontconfig
            Freetype::Freetype
            PkgConfig::cairo
            $<$<BOOL:${PLUTOBOOK_FEATURE_CURL}>:CURL::libcurl>
            $<$<BOOL:${PLUTOBOOK_FEATURE_JPEG}>:PkgConfig::jpeg-turbo>
    )

    target_sources(
        ${TARGET}
        PUBLIC
            FILE_SET
                HEADERS
                BASE_DIRS ${CMAKE_CURRENT_LIST_DIR}/include
                FILES
                    include/plutobook.h
                    include/plutobook.hpp
    )

    target_sources(
        ${TARGET}
        PRIVATE
            source/graphics/color.cpp
            source/graphics/geometry.cpp
            source/graphics/graphicscontext.cpp
            source/graphics/textshape.cpp

            source/layout/blockbox.cpp
            source/layout/borderpainter.cpp
            source/layout/box.cpp
            source/layout/boxlayer.cpp
            source/layout/boxstyle.cpp
            source/layout/boxview.cpp
            source/layout/contentbox.cpp
            source/layout/flexiblebox.cpp
            source/layout/formcontrolbox.cpp
            source/layout/fragmentbuilder.cpp
            source/layout/inlinebox.cpp
            source/layout/linebox.cpp
            source/layout/linelayout.cpp
            source/layout/listitembox.cpp
            source/layout/multicolumnbox.cpp
            source/layout/pagebox.cpp
            source/layout/replacedbox.cpp
            source/layout/svgboxmodel.cpp
            source/layout/svgcontainerbox.cpp
            source/layout/svggeometrybox.cpp
            source/layout/svglinelayout.cpp
            source/layout/svgreplacedbox.cpp
            source/layout/svgresourcebox.cpp
            source/layout/svgtextbox.cpp
            source/layout/tablebox.cpp
            source/layout/textbox.cpp

            source/resource/fontresource.cpp
            source/resource/imageresource.cpp
            source/resource/resource.cpp
            source/resource/textresource.cpp
            source/resource/url.cpp

            source/counters.cpp
            source/cssparser.cpp
            source/cssrule.cpp
            source/cssstylesheet.cpp
            source/csstokenizer.cpp
            source/document.cpp
            source/globalstring.cpp
            source/htmldocument.cpp
            source/htmlentityparser.cpp
            source/htmlparser.cpp
            source/htmltokenizer.cpp
            source/plutobook.cc
            source/plutobook.cpp
            source/svgdocument.cpp
            source/svgproperty.cpp
            source/textbreakiterator.cpp
            source/xmldocument.cpp
            source/xmlparser.cpp
    )

endfunction()


################################################################################
# Shared library                                                               #
################################################################################

if (PLUTOBOOK_BUILD_SHARED)
    add_library(${TARGET_SHARED} SHARED)
    setup_target_common(${TARGET_SHARED})
endif()


################################################################################
# Static library                                                               #
################################################################################

if (PLUTOBOOK_BUILD_STATIC)
    add_library(${TARGET_STATIC} STATIC)
    setup_target_common(${TARGET_STATIC})
endif()


################################################################################
# Print configuration                                                          #
################################################################################
message(STATUS "")
message(STATUS "------------------------")
message(STATUS "PlutoBook configuration:")
message(STATUS "  version   : " ${PROJECT_VERSION})
message(STATUS "")
message(STATUS "  build:")
message(STATUS "    static   : " ${PLUTOBOOK_BUILD_STATIC})
message(STATUS "    shared   : " ${PLUTOBOOK_BUILD_SHARED})
message(STATUS "    examples : " ${PLUTOBOOK_BUILD_EXAMPLES})
message(STATUS "")
message(STATUS "  features:")
message(STATUS "    curl     : " ${PLUTOBOOK_FEATURE_CURL})
message(STATUS "    jpeg     : " ${PLUTOBOOK_FEATURE_JPEG})
message(STATUS "")
message(STATUS "------------------------")
message(STATUS "")


################################################################################
# Install                                                                      #
################################################################################

# Some basic stuff we'll need in this section
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
set(ConfigPackageLocation ${CMAKE_INSTALL_LIBDIR}/cmake/plutobook)


# Targets
if (PLUTOBOOK_BUILD_STATIC)
    list(APPEND INSTALL_TARGETS_LIST ${TARGET_STATIC})
endif()
if (PLUTOBOOK_BUILD_SHARED)
    list(APPEND INSTALL_TARGETS_LIST ${TARGET_SHARED})
endif()
install(
    TARGETS
        ${INSTALL_TARGETS_LIST}
        EXPORT plutobook-targets
    FILE_SET
        HEADERS
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/plutobook
    LIBRARY
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT lib
    ARCHIVE
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT lib
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT bin
)

# Package version
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/plutobook/plutobook-config-version.cmake"
    VERSION ${plutobook_VERSION}
    COMPATIBILITY AnyNewerVersion
)
install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/plutobook/plutobook-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/plutobook/plutobook-config-version.cmake"
    DESTINATION ${ConfigPackageLocation}
)

# Package configuration
configure_file(plutobook-config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/plutobook/plutobook-config.cmake"
    @ONLY
)

# Package export targets
export(
    EXPORT plutobook-targets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/plutobook/plutobook-targets.cmake"
    NAMESPACE plutobook::
)
install(
    EXPORT plutobook-targets
    FILE plutobook-targets.cmake
    NAMESPACE plutobook::
    DESTINATION ${ConfigPackageLocation}
)


include(CPack)
