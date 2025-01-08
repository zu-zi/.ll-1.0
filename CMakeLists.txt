# cmake -DEXECUTE_FILE=test.c .. && cmake --build . --target run

cmake_minimum_required(VERSION 3.13)
project(compiler)

# settings
# set to OFF to enable C mode
set(CPP_MODE ON)
if(CPP_MODE)
  set(FB_EXT ".cpp")
else()
  set(FB_EXT ".c")
endif()
message(STATUS "Flex/Bison generated source file extension: ${FB_EXT}")
add_compile_options(-g)
# enable all warnings
if(MSVC)
  add_compile_options(/W3)
else()
  # disable warnings caused by old version of Flex
  add_compile_options(-Wall -Wno-register)
endif()

# options about libraries and includes
set(LIB_DIR "$ENV{CDE_LIBRARY_PATH}/native" CACHE STRING "directory of libraries")
set(INC_DIR "$ENV{CDE_INCLUDE_PATH}" CACHE STRING "directory of includes")
set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/thirdparty)
set(THIRD_PARTY_BINIRY_DIR ${PROJECT_BINARY_DIR}/thirdparty)
message(STATUS "Library directory: ${LIB_DIR}")
message(STATUS "Include directory: ${INC_DIR}")

# find Flex/Bison
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

# generate lexer/parser
file(GLOB_RECURSE L_SOURCES "src/parser/*.l")
file(GLOB_RECURSE Y_SOURCES "src/parser/*.y")
if(NOT (L_SOURCES STREQUAL "" AND Y_SOURCES STREQUAL ""))
  string(REGEX REPLACE ".*/(.*)\\.l" "${CMAKE_CURRENT_BINARY_DIR}/\\1.lex${FB_EXT}" L_OUTPUTS "${L_SOURCES}")
  string(REGEX REPLACE ".*/(.*)\\.y" "${CMAKE_CURRENT_BINARY_DIR}/\\1.tab${FB_EXT}" Y_OUTPUTS "${Y_SOURCES}")
  flex_target(Lexer ${L_SOURCES} ${L_OUTPUTS})
  bison_target(Parser ${Y_SOURCES} ${Y_OUTPUTS})
  add_flex_bison_dependency(Lexer Parser)
endif()
message(STATUS "Lex_out in ${L_OUTPUTS}")
message(STATUS "Yacc_out in ${Y_OUTPUTS}")
# project link directories
link_directories(${LIB_DIR})

# project include directories
include_directories(src)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${INC_DIR})
set(COMPILER_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/include)
include_directories(${COMPILER_SRC_INCLUDE_DIR})
# all of C/C++ source files
file(GLOB_RECURSE C_SOURCES "src/*.c" "src/*/*.c" "src/*/*/*.c")
file(GLOB_RECURSE CXX_SOURCES "src/*.cpp" "src/*/*.cpp" "src/*/*/*.cpp")
file(GLOB_RECURSE CC_SOURCES "src/*.cc" "src/*/*.cc" "src/*/*/*.cc")
set(SOURCES ${C_SOURCES} ${CXX_SOURCES} ${CC_SOURCES}
            ${FLEX_Lexer_OUTPUTS} ${BISON_Parser_OUTPUT_SOURCE})

#add parse lib for test
add_library(parse SHARED ${L_OUTPUTS} ${Y_OUTPUTS} ${SOURCES})

# executable
add_executable(compiler ${SOURCES})

# add command
add_custom_target(
  run 
  COMMAND ${CMAKE_BINARY_DIR}/${PROJECT_NAME} test ${CMAKE_SOURCE_DIR}/test/${EXECUTE_FILE} -o ${CMAKE_SOURCE_DIR}/test/${EXECUTE_FILE}.ll
)

add_custom_target(
  llvm 
  clang -S -emit-llvm ../debug/hello.c
)

add_custom_target(
  bc
  llvm-as ${CMAKE_SOURCE_DIR}/debug/hello.ll -o hello.bc
)

add_custom_target(
  quicksort
  COMMAND ${CMAKE_BINARY_DIR}/${PROJECT_NAME} test ${CMAKE_SOURCE_DIR}/test/quicksort.c -o ${CMAKE_SOURCE_DIR}/debug/quicksort.ll
)

add_custom_target(
  build_quicksort
  COMMAND llc -march=x86-64 ${CMAKE_SOURCE_DIR}/debug/quicksort.ll -o ${CMAKE_SOURCE_DIR}/debug/quicksort.s
  COMMAND as ${CMAKE_SOURCE_DIR}/debug/quicksort.s -o ${CMAKE_SOURCE_DIR}/debug/quicksort.o
  COMMAND clang -Wall -Werror ${CMAKE_SOURCE_DIR}/debug/quicksort.o -o ${CMAKE_SOURCE_DIR}/debug/quicksort -I${CMAKE_SOURCE_DIR}/sysy-runtime-lib-master/src -L${CMAKE_SOURCE_DIR}/sysy-runtime-lib-master/build -lsysy -O1
  COMMENT "quicksort Linked"
)

add_custom_target(
  test_quicksort
  COMMAND make quicksort
  COMMAND make build_quicksort
  COMMENT "quicksort Linked"
  COMMAND chmod a+x ${CMAKE_SOURCE_DIR}/tester/quicksort/quicksort-linux-amd64
  COMMAND ${CMAKE_SOURCE_DIR}/tester/quicksort/quicksort-linux-amd64 ${CMAKE_SOURCE_DIR}/debug/quick_sort
  COMMENT "quicksort Tested"
)

set_target_properties(compiler PROPERTIES C_STANDARD 11 CXX_STANDARD 17)
target_link_libraries(compiler pthread dl)