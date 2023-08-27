set(SOURCEFILES
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/lib/include/)
add_executable(${PROJECT_NAME} ${SOURCEFILES})
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}lib)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
