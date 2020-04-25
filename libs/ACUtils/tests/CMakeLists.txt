cmake_minimum_required(VERSION 2.8.12)

add_subdirectory(libs/check)

add_executable(ACUtilsTest src/test_dynarray.c ../src/dynarray.c)
set_target_properties(ACUtilsTest PROPERTIES C_STANDARD 90 C_EXTENSIONS off)
target_compile_definitions(ACUtilsTest PUBLIC A_C_UTILS_TEST=1 A_C_UTILS_ONE_SOURCE=0)
target_include_directories(ACUtilsTest PUBLIC ../include)
target_link_libraries(ACUtilsTest check)
