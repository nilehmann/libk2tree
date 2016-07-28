enable_testing()

add_executable(test_libk2tree test_main.cc queries.cc)
target_link_libraries(test_libk2tree ${GTEST_LIBRARIES} rt gtest ${LIBK2TREE_NAME} ${Boost_LIBRARIES} pthread boost_system boost_filesystem)