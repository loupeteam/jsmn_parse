# Tell compiler to use C++20 features. The code doesn't actually use any of them.
# target_compile_features(poly PUBLIC cxx_std_20)

# version 3.11 or later of CMake or needed later for installing GoogleTest
# so let's require it now.
cmake_minimum_required(VERSION 3.11-3.18)

project(jsmn_parse)

file(GLOB poly_SRC
     "jsmn.c"
)
add_library(jsmn_parse ${poly_SRC})
add_definitions(-D_REPLACE_CONST)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/../includes/)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/../includes/loupe/Includes/)
