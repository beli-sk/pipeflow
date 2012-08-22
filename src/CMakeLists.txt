include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
)

# define the sources
set(PIPEFLOW_SRC pipeflow.c)

#define executable
add_executable(pipeflow ${PIPEFLOW_SRC})
target_link_libraries(pipeflow rt ${CMAKE_REQUIRED_LIBRARIES})

install(TARGETS pipeflow RUNTIME DESTINATION bin)
