set(SOURCEFILES
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/lib/include/)
find_package(Qt6 REQUIRED COMPONENTS Core)

add_executable(${PROJECT_NAME} ${SOURCEFILES})
target_link_libraries(${PROJECT_NAME}  PRIVATE ${PROJECT_NAME}lib Qt6::Core)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
