file(GLOB HEADERS *.h)

add_executable(c2 ${HEADERS}
    ast_builder.cpp
    ast_syntax.cpp
    ast.cpp
    command_line.cpp
    common.cpp
    cppgen.cpp
    errors.cpp
    main.cpp
    bst.cpp
    cst.cpp
)

target_link_libraries(c2
    absl::base
    absl::str_format
    absl::strings
    forrest::util
    microlib::microlib
)
