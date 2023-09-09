
set(CMAKE_AUTOMOC ON)

include_directories(${PROJECT_SOURCE_DIR}/lib/include/)
find_package(Qt6 REQUIRED COMPONENTS Core Quick)

add_executable(tasktracker
    main.cpp
)

qt_add_qml_module(tasktracker
    URI tasktrackerqml
    VERSION 1.0
    QML_FILES qml/Main.qml
)

target_link_libraries(tasktracker
                      PRIVATE Qt6::Core Qt6::Quick ${PROJECT_NAME}lib
)

include(GNUInstallDirs)
install(TARGETS tasktracker
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
