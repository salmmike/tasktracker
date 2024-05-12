
set(CMAKE_AUTOMOC ON)

include_directories(${PROJECT_SOURCE_DIR}/lib/include/
                    ${CMAKE_CURRENT_SOURCE_DIR}/include/)

find_package(Qt6 REQUIRED COMPONENTS Core Quick HttpServer)

add_executable(tasktracker
    main.cpp
    include/TaskListModel.h
    include/DeviceListModel.h
    include/AddTaskServer.h
    include/TopOptions.h
    include/WeatherListModel.h


    TaskListModel.cpp
    DeviceListModel.cpp
    AddTaskServer.cpp
    TopOptions.cpp
    WeatherListModel.cpp
)

qt_add_qml_module(tasktracker
    URI tasktrackerqml
    VERSION 1.0
    QML_FILES qml/Main.qml qml/TaskItem.qml qml/TaskList.qml qml/DeviceList.qml qml/WeatherList.qml
)

target_link_libraries(tasktracker
                      PRIVATE
                      Qt6::Core Qt6::Quick Qt6::HttpServer
                      ${PROJECT_NAME}lib
                      simpleini
                      boredomlock
)

include(GNUInstallDirs ./include)
install(TARGETS tasktracker
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
