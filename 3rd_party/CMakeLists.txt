# Don't generate tests/examples and documentation for Marian
set(COMPILE_TESTS OFF)
set(COMPILE_EXAMPLES OFF)
set(USE_DOXYGEN OFF)
set(USE_MPI OFF)

add_subdirectory(marian-dev EXCLUDE_FROM_ALL)

# Marian include directories
get_property(INCDIRS DIRECTORY marian-dev/src PROPERTY INCLUDE_DIRECTORIES)
target_include_directories(marian PUBLIC ${INCDIRS})

# Adopt Marian C & CXX flags
get_directory_property(CMAKE_C_FLAGS DIRECTORY marian-dev DEFINITION CMAKE_C_FLAGS)
get_directory_property(CMAKE_CXX_FLAGS DIRECTORY marian-dev DEFINITION CMAKE_CXX_FLAGS)
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} PARENT_SCOPE)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} PARENT_SCOPE)
