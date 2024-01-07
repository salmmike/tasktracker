
set(CMAKE_AUTOMOC ON)

include_directories(${PROJECT_SOURCE_DIR}/lib/include/)
find_package(Qt6 REQUIRED COMPONENTS Core Quick HttpServer)

set(CMAKE_CXX_CLANG_TIDY
    clang-tidy;
    -header-filter=.;
    -extra-arg=-std=c++20)

add_executable(tasktracker
    main.cpp
    include/TaskListModel.h
    include/AddTaskServer.h
    include/TopOptions.h
    TaskListModel.cpp
    AddTaskServer.cpp
    TopOptions.cpp
)

qt_add_qml_module(tasktracker
    URI tasktrackerqml
    VERSION 1.0
    QML_FILES qml/Main.qml qml/TaskItem.qml qml/TaskList.qml
)

target_link_libraries(tasktracker
                      PRIVATE
                      Qt6::Core Qt6::Quick Qt6::HttpServer
                      ${PROJECT_NAME}lib
                      simpleini
)

include(GNUInstallDirs)
install(TARGETS tasktracker
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
