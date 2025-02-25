# Set the minimum required CMake version and project name
cmake_minimum_required(VERSION 3.10)
project(YourProject)

# Set MicroPython directory (can be overridden by the user)
set(MICROPYTHON_DIR micropython CACHE PATH "Path to MicroPython directory")
set(MICROPYTHON_INCLUDE_DIR ${MICROPYTHON_DIR}/include)
set(MICROPYTHON_LIB_DIR ${MICROPYTHON_DIR}/lib)

# Validate MicroPython header and library paths
if(NOT EXISTS ${MICROPYTHON_INCLUDE_DIR}/micropython.h)
    message(FATAL_ERROR "MicroPython header not found at ${MICROPYTHON_INCLUDE_DIR}/micropython.h. Set MICROPYTHON_DIR correctly.")
endif()
if(NOT EXISTS ${MICROPYTHON_LIB_DIR}/libmicropython.a)
    message(FATAL_ERROR "MicroPython library not found at ${MICROPYTHON_LIB_DIR}/libmicropython.a. Set MICROPYTHON_DIR correctly.")
endif()

# Define source and build directories
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(BUILD_DIR ${CMAKE_BINARY_DIR}/build)
file(MAKE_DIRECTORY ${BUILD_DIR})

# Define Cython source files and their corresponding generated C files
set(CYTHON_FILES ${SRC_DIR}/my_list.pyx)
set(C_FILES_GENERATED ${BUILD_DIR}/my_list.c)

# Generate C files from Cython files using custom commands
foreach(cython_file IN LISTS CYTHON_FILES)
    get_filename_component(base_name ${cython_file} NAME_WE)
    set(generated_c_file ${BUILD_DIR}/${base_name}.c)
    add_custom_command(
        OUTPUT ${generated_c_file}
        COMMAND cython -o ${generated_c_file} ${cython_file}
        DEPENDS ${cython_file}
        COMMENT "Generating ${generated_c_file} from ${cython_file}"
    )
endforeach()

# Define all C source files (original and generated)
set(C_FILES ${SRC_DIR}/main.c)
set(ALL_C_FILES ${C_FILES} ${C_FILES_GENERATED})

# Modify includes in C files using modify_includes.py
set(INCLUDES_MODIFIED_STAMP ${BUILD_DIR}/.includes_modified)
add_custom_command(
    OUTPUT ${INCLUDES_MODIFIED_STAMP}
    COMMAND python modify_includes.py ${ALL_C_FILES}
    COMMAND ${CMAKE_COMMAND} -E touch ${INCLUDES_MODIFIED_STAMP}
    DEPENDS ${ALL_C_FILES}
    COMMENT "Modifying includes in C files"
)
add_custom_target(modify_includes DEPENDS ${INCLUDES_MODIFIED_STAMP})

# Define the executable with all C files
add_executable(your_program ${ALL_C_FILES})

# Set include directories for the executable
target_include_directories(your_program PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${MICROPYTHON_INCLUDE_DIR}
)

# Set link directories and libraries for the executable
link_directories(${MICROPYTHON_LIB_DIR})
target_link_libraries(your_program PRIVATE micropython)

# Ensure includes are modified before building the executable
add_dependencies(your_program modify_includes)

# Set compiler optimization flags
set_target_properties(your_program PROPERTIES COMPILE_FLAGS "-O2")