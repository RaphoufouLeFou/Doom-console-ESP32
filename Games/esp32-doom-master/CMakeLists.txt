# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp32_doom)

set(CMAKE_C_FLAGS  "-Wno-nonnull -Wno-implicit-function-declaration -Wno-misleading-indentation -Wno-format-overflow -Wno-unused-const-variable -Wno-sizeof-pointer-div -Wno-duplicate-decl-specifier -Wno-dangling-else")

set(COMPONENT_REQUIRES mcp23x17  prboom  prboom-esp32-compat)


#set(CFLAGS_LIST
#    "-Wall "
#    "-Wmissing-prototypes -Wmissing-declarations -Wredundant-decls "
#    "-Wunused-function -Wunused-value -Wunused-variable ")
