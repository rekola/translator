cmake_minimum_required(VERSION 3.10)

# set the project name
project(m22 VERSION 1.1.24)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Default to Release
if(NOT CMAKE_BUILD_TYPE)
  message(WARNING "CMAKE_BUILD_TYPE not set; setting to Release")
  set(CMAKE_BUILD_TYPE "Release")
endif()

# Setup Git submodules
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(MSVC)
    add_definitions(-DUSE_SSE2=1)  # error C3861
    add_compile_options("$<$<CONFIG:DEBUG>:/MDd>")  # error LNK2038
endif()

# Disable CUDA for Marian by default
set(COMPILE_CUDA OFF CACHE BOOL "Compile GPU version")

add_subdirectory(3rd_party)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -O3 -Wall -Wsuggest-override -Werror=return-local-addr -Werror=multichar -Werror=enum-compare -Werror=return-type -Werror=conversion-null -Werror=parentheses  -Werror=address -Werror=trigraphs -Werror=pointer-arith -ffast-math -fno-math-errno -Werror=write-strings -Werror=return-local-addr -Werror=pessimizing-move -Wuninitialized -Wno-suggest-override -Wno-unknown-pragmas -Werror=switch -Werror=format -I../../fastText/src")

# -Werror=non-virtual-dtor -Werror=cast-qual -Wconversion -Wsign-conversion -Wsign-promo -Wno-float-conversion

# add the executable
add_executable(translator
  	src/main.cpp
	src/TranslationContext.cpp
	src/MarianTranslator.cpp
	../fastText/src/fasttext.cc
	../fastText/src/matrix.cc
	../fastText/src/dictionary.cc
	../fastText/src/loss.cc
	../fastText/src/productquantizer.cc
	../fastText/src/densematrix.cc
	../fastText/src/quantmatrix.cc
	../fastText/src/vector.cc
	../fastText/src/model.cc
	../fastText/src/utils.cc
	../fastText/src/args.cc
	../fastText/src/meter.cc
)


target_link_libraries(translator m fmt pthread utf8proc marian)

target_include_directories(translator
  PUBLIC src/
  3rd_party/cpp-httplib
  3rd_party/json/single_include
  )

