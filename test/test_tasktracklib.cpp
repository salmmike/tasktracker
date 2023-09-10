#include <gtest/gtest.h>
#include <iostream>
#include <cassert>
#include <database_driver.h>
#include <tasktracklib.h>

#define NAME test_tasktracklib
#define TESTDBFILE "test2.db"
#define TESTTASKNAME "test_task"
#define TESTTASKNAME2 "test_task2"

using namespace tasktracker;

std::string format_date(tm* date, const std::string& str="")
{
    char buffer[26];
    strftime(buffer, 26, "%a %Y-%m-%d %H:%M:%S", date);
    return str + buffer;
}

tm add_days(int days, tm date)
{
    time_t time = mktime(&date);
    time += 24 * 60 * 60 * days;
    return *localtime(&time);
}

TEST(NAME, test_create_tasktracker)
{
    TaskTracker tracker(TESTDBFILE);
}

TEST(NAME, test_create_task)
{
    TaskTracker tracker(TESTDBFILE);
    tracker.clear();

    tm start_time {};
    start_time.tm_year = 2023 - 1900;
    start_time.tm_mday = 1;
    start_time.tm_hour = 9;

    tracker.add_task(TESTTASKNAME, RepeatType::WithInterval, 5, start_time);
    auto tasks = tracker.get_task_instances(start_time);

    ASSERT_EQ(tasks.size(), 1);

    tracker.add_task(TESTTASKNAME2, RepeatType::WithInterval, 5, start_time);
    tasks = tracker.get_task_instances(start_time);
    ASSERT_EQ(tasks.size(), 2);

    start_time.tm_mday += 1;
    tasks = tracker.get_task_instances(start_time);
    ASSERT_EQ(tasks.size(), 0);

    tracker.clear();
}

TEST(NAME, test_task_repeat_interval)
{
    TaskTracker tracker(TESTDBFILE);
    tracker.clear();

    tm start_time {};
    start_time.tm_year = 2023 - 1900;
    start_time.tm_mday = 1;
    start_time.tm_hour = 9;

    tracker.add_task(TESTTASKNAME, RepeatType::WithInterval, 5, start_time);
    auto tasks = tracker.get_task_instances(start_time);
    ASSERT_EQ(tasks.size(), 1);
    auto task = tasks[0];

    auto start_time_t = task->get_scheduled_datetime();

    tm task_start_time = *localtime(&start_time_t);

    ASSERT_EQ(task_start_time.tm_mday, start_time.tm_mday);

    start_time = add_days(5, start_time);

    tasks = tracker.get_task_instances(start_time);

    ASSERT_EQ(tasks.size(), 1);
    task = tasks[0];

    start_time_t = task->get_scheduled_datetime();
    task_start_time = *localtime(&start_time_t);

    ASSERT_EQ(task_start_time.tm_mday, start_time.tm_mday);
    tracker.clear();
}

TEST(NAME, test_task_repeat_weekdays)
{
    TaskTracker tracker(TESTDBFILE);
    tracker.clear();

    tm start_time {};
    start_time.tm_year = 2023 - 1900;
    start_time.tm_mday = 1; // start time is Sunday january 1st 2023
    start_time.tm_hour = 9;

    tracker.add_task(TESTTASKNAME, RepeatType::SpecifiedDays, 12345, start_time);
    auto tasks = tracker.get_task_instances(start_time);
    ASSERT_EQ(tasks.size(), 0) << "This task should only run on weekdays";

    for (int i = 0; i < 7; ++i) {
        start_time = add_days(1, start_time); // set to monday
        tasks = tracker.get_task_instances(start_time);
        if (i < 5) {
            ASSERT_EQ(tasks.size(), 1) << "this task should run on " << format_date(&start_time);
        } else {
            ASSERT_EQ(tasks.size(), 0) << "this task should not run on day number " << format_date(&start_time);
        }
    }
}

TEST(NAME, test_task_repeat_monthly)
{
    TaskTracker tracker(TESTDBFILE);
    tracker.clear();

    tm start_time {};
    start_time.tm_year = 2023 - 1900;
    start_time.tm_mday = 1; // start time is Sunday january 1st 2023
    start_time.tm_hour = 9;

    tracker.add_task(TESTTASKNAME, RepeatType::Monthly, 12, start_time);
    auto tasks = tracker.get_task_instances(start_time);
    ASSERT_EQ(tasks.size(), 0) << "This task should only run on the 12th day of the month";

    start_time = add_days(11, start_time);
    tasks = tracker.get_task_instances(start_time);
    ASSERT_EQ(tasks.size(), 1) << "This task should run on " << format_date(&start_time);

    start_time = add_days(31, start_time);
    tasks = tracker.get_task_instances(start_time);
    ASSERT_EQ(tasks.size(), 1) << "This task should run on " << format_date(&start_time);

    start_time = add_days(12, start_time);
    tasks = tracker.get_task_instances(start_time);
    ASSERT_EQ(tasks.size(), 0) << "This task should not run on " << format_date(&start_time);
    tracker.clear();
}


TEST(NAME, test_task_repeat_monthly_day)
{
    TaskTracker tracker(TESTDBFILE);
    tracker.clear();

    tm start_time {};
    start_time.tm_year = 2023 - 1900;
    start_time.tm_mday = 1; // start time is Sunday january 1st 2023
    start_time.tm_hour = 9;

    tracker.add_task(TESTTASKNAME, RepeatType::MonthlyDay, 23, start_time); // Second wednesday
    auto tasks = tracker.get_task_instances(start_time);
    ASSERT_EQ(tasks.size(), 0) << "This task should only run on the 2nd wednesday of the month";

    start_time = add_days(10, start_time);
    tasks = tracker.get_task_instances(start_time);
    ASSERT_EQ(tasks.size(), 1) << "This task should run on " << format_date(&start_time);

    start_time = add_days(28, start_time);
    tasks = tracker.get_task_instances(start_time);
    ASSERT_EQ(tasks.size(), 1) << "This task should run on " << format_date(&start_time);

    for (int i = 0; i < 20; ++i) {
        start_time = add_days(1, start_time);
        tasks = tracker.get_task_instances(start_time);
        ASSERT_EQ(tasks.size(), 0) << "This task should not run on " << format_date(&start_time);
    }

    tracker.clear();
}

TEST(NAME, test_task_lookup_performance)
{

    TaskTracker tracker(TESTDBFILE);
    tracker.clear();

    tm start_time {};
    start_time.tm_year = 2023 - 1900;
    start_time.tm_mday = 1; // start time is Sunday january 1st 2023
    start_time.tm_hour = 9;

    tracker.add_task(TESTTASKNAME, RepeatType::MonthlyDay, 23, start_time); // Second wednesday
    for (int i = 0; i < 10000; ++i) {
        auto tasks = tracker.get_task_instances(start_time);
        ASSERT_EQ(tasks.size(), 0) << "This task should only run on the 2nd wednesday of the month";
    }

    start_time = add_days(10, start_time);
    for (int i = 0; i < 10000; ++i) {
        auto tasks = tracker.get_task_instances(start_time);
        ASSERT_EQ(tasks.size(), 1) << "This task should only run on the 2nd wednesday of the month";
    }


    for (int i = 0; i < 100; ++i) {
        start_time = add_days(1, start_time);
        auto tasks = tracker.get_task_instances(start_time);
    }
}


int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
