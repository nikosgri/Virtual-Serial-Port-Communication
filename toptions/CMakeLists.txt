add_library(
    toptions
    src/toptions/term_options.h
    src/toptions/term_options.c
)
target_include_directories(toptions PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")