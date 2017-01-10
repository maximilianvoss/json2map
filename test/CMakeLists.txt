cmake_minimum_required(VERSION 2.8)

set(SOURCE_FILES
        testcases.c)

add_executable(testcases ${SOURCE_FILES})
target_link_libraries(testcases json2map)

add_test(NAME test_map2json_emptyMap COMMAND testcases test_map2json_emptyMap)
add_test(NAME test_map2json_null COMMAND testcases test_map2json_null)
add_test(NAME test_map2json_true COMMAND testcases test_map2json_true)
add_test(NAME test_map2json_false COMMAND testcases test_map2json_false)
add_test(NAME test_map2json_numberInt COMMAND testcases test_map2json_numberInt)
add_test(NAME test_map2json_numberFloat COMMAND testcases test_map2json_numberFloat)
add_test(NAME test_map2json_array COMMAND testcases test_map2json_array)
add_test(NAME test_map2json_primitive COMMAND testcases test_map2json_primitive)
add_test(NAME test_map2json_deepNesting COMMAND testcases test_map2json_deepNesting)
