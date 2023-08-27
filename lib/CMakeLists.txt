set(INCDIR ${CMAKE_CURRENT_LIST_DIR}/include/)
set(LIBHEADERS
    ${INCDIR}database_driver.h
    ${INCDIR}task_data.h
    ${INCDIR}tasktracklib.h
    ${INCDIR}task.h
)


set(LIBSOURCES
    ${CMAKE_CURRENT_LIST_DIR}/database_driver.cpp
    ${CMAKE_CURRENT_LIST_DIR}/tasktracklib.cpp
    ${CMAKE_CURRENT_LIST_DIR}/task.cpp
)

set(LIBNAME ${PROJECT_NAME}lib)

include_directories(${INCDIR})
add_library(${LIBNAME} STATIC ${LIBSOURCES})
target_include_directories(${LIBNAME} PUBLIC ${INCDIR})


find_package (SQLite3)

if (SQLITE3_FOUND)
  target_link_libraries(${LIBNAME} LINK_PUBLIC sqlite3)
else()
    message("sqlite3 not found!")
endif (SQLITE3_FOUND)

find_package(fmt)
target_link_libraries(${LIBNAME} LINK_PUBLIC fmt::fmt)

set(PATHNAME ${PROJECT_NAME})
