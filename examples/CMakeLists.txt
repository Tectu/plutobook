################################################################################
# Example "html2pdf"
################################################################################

set(TARGET1 plutobook-example-html2pdf)

add_executable(${TARGET1})

target_sources(
    ${TARGET1}
    PRIVATE
        html2pdf.cpp
)

target_link_libraries(
    ${TARGET1}
    plutobook-static
)


################################################################################
# Example "invoice"
################################################################################

set(TARGET2 plutobook-example-invoice)

add_executable(${TARGET2})

target_sources(
    ${TARGET2}
    PRIVATE
        invoice.cpp
)

target_link_libraries(
    ${TARGET2}
    plutobook-static
)
