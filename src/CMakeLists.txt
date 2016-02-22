file(GLOB_RECURSE LIBK2TREE_SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cc)

add_library(${LIBK2TREE_NAME} SHARED ${LIBK2TREE_SRC_FILES})
target_link_libraries(${LIBK2TREE_NAME} ${libcds2_LIBRARIES} dacs)
#target_link_libraries(${LIBK2TREE_NAME} dacs)
include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/dacs/include
)

install(TARGETS ${LIBK2TREE_NAME}
  LIBRARY DESTINATION lib
  COMPONENT libraries
)
