include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

add_executable(test_database test_database.cpp)

target_link_libraries(test_database
    PRIVATE
    GTest::GTest
    ${PROJECT_NAME}lib)

add_executable(test_tasktracklib test_tasktracklib.cpp)

target_link_libraries(test_tasktracklib
    PRIVATE
    GTest::GTest
    ${PROJECT_NAME}lib)

add_executable(test_tasks test_tasks.cpp)

target_link_libraries(test_tasks
      PRIVATE
      GTest::GTest
      ${PROJECT_NAME}lib)

add_test(test_database test_database)
add_test(test_tasktracklib test_tasktracklib)
add_test(test_tasks test_tasks)