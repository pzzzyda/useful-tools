cmake_minimum_required(VERSION 3.20)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(useful-tools)

include_directories(include)

file(GLOB SOURCES src/collections/*.c src/*.c)

file(GLOB HEADERS include/*.h)

add_library(ut STATIC ${SOURCES})

install(
	TARGETS ut
	ARCHIVE DESTINATION lib
)

install(
	FILES ${HEADERS}
	DESTINATION include
)

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
	@ONLY
)

add_custom_target(
	uninstall
	COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
	COMMENT "Uninstalling..."
)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
	enable_testing()

	add_executable(ut_array_test test/ut_array_test.c)
	add_executable(ut_deque_test test/ut_deque_test.c)
	add_executable(ut_hash_map_test test/ut_hash_map_test.c)
	add_executable(ut_hash_set_test test/ut_hash_set_test.c)
	add_executable(ut_heap_test test/ut_heap_test.c)
	add_executable(ut_list_test test/ut_list_test.c)
	add_executable(ut_string_test test/ut_string_test.c)
	add_executable(ut_tree_map_test test/ut_tree_map_test.c)
	add_executable(ut_tree_set_test test/ut_tree_set_test.c)

	target_link_libraries(ut_array_test ut)
	target_link_libraries(ut_deque_test ut)
	target_link_libraries(ut_hash_map_test ut)
	target_link_libraries(ut_hash_set_test ut)
	target_link_libraries(ut_heap_test ut)
	target_link_libraries(ut_list_test ut)
	target_link_libraries(ut_string_test ut)
	target_link_libraries(ut_tree_map_test ut)
	target_link_libraries(ut_tree_set_test ut)

	add_test(UTArrayTest ut_array_test)
	add_test(UTDequeTest ut_deque_test)
	add_test(UTHashMapTest ut_hash_map_test)
	add_test(UTHashSetTest ut_hash_set_test)
	add_test(UTHeap ut_heap_test)
	add_test(UTListTest ut_list_test)
	add_test(UTStringTest ut_string_test)
	add_test(UTTreeMapTest ut_tree_map_test)
	add_test(UTTreeSetTest ut_tree_set_test)
endif()
