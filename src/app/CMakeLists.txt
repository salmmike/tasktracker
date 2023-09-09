
set(CMAKE_AUTOMOC ON)

include_directories(${PROJECT_SOURCE_DIR}/lib/include/)
find_package(Qt6 REQUIRED COMPONENTS Core Quick)

add_executable(tasktracker
    main.cpp
)
target_link_libraries(tasktracker PRIVATE Qt6::Core Qt6::Quick ${PROJECT_NAME}lib)
