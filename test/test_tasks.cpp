#include <gtest/gtest.h>
#include <iostream>
#include <cassert>
#include <task.h>

#define NAME test_tasks
#define TESTDBFILE "test2.db"
#define TESTTASKNAME "test_task"

using namespace tasktracker;

TEST(NAME, test_create_task)
{
    TaskDatabase db(TESTDBFILE);
    auto uid = db.create_task(TESTTASKNAME);
    auto task_data = db.get_task(uid);
    Task task(task_data.get(), &db);
    db.delete_task(task_data.get());
}

TEST(NAME, test_occurs_mon_tue_wed)
{
    TaskDatabase db(TESTDBFILE);
    auto uid = db.create_task(TESTTASKNAME);
    auto task_data = db.get_task(uid);
    task_data->repeat_type = RepeatType::SpecifiedDays;
    task_data->repeat_info = 123;

    Task task(task_data.get(), &db);

    ASSERT_FALSE(task.occurs(std::chrono::year_month_day(
        std::chrono::year(2023),
        std::chrono::month(8),
        std::chrono::day(26)))
    ) << "Saturday is not in repeat info 123";


    ASSERT_FALSE(task.occurs(std::chrono::year_month_day(
        std::chrono::year(2023),
        std::chrono::month(8),
        std::chrono::day(27)))
    ) << "Sunday is not in repat_info 123";

    ASSERT_TRUE(task.occurs(std::chrono::year_month_day(
        std::chrono::year(2023),
        std::chrono::month(8),
        std::chrono::day(28)))
    ) << "Monday is in repat_info 123";

    ASSERT_TRUE(task.occurs(std::chrono::year_month_day(
        std::chrono::year(2023),
        std::chrono::month(8),
        std::chrono::day(29)))
    ) << "Tuesday is in repat_info 123";

    ASSERT_TRUE(task.occurs(std::chrono::year_month_day(
        std::chrono::year(2023),
        std::chrono::month(8),
        std::chrono::day(30)))
    ) << "Wednesday is in repat_info 123";

    ASSERT_FALSE(task.occurs(std::chrono::year_month_day(
        std::chrono::year(2023),
        std::chrono::month(9),
        std::chrono::day(1)))
    ) << "Thursday is not in repat_info 123";

    db.delete_task(task_data.get());
}


TEST(NAME, test_occurs_weekend)
{
    TaskDatabase db(TESTDBFILE);
    auto uid = db.create_task(TESTTASKNAME);
    auto task_data = db.get_task(uid);
    task_data->repeat_type = RepeatType::SpecifiedDays;
    task_data->repeat_info = 67;

    Task task(task_data.get(), &db);

    ASSERT_TRUE(task.occurs(std::chrono::year_month_day(
        std::chrono::year(2023),
        std::chrono::month(8),
        std::chrono::day(26)))
    ) << "Saturday is in repeat info 67";

    ASSERT_TRUE(task.occurs(std::chrono::year_month_day(
        std::chrono::year(2023),
        std::chrono::month(8),
        std::chrono::day(27)))
    ) << "Sunday is in repeat_info 67";

    ASSERT_FALSE(task.occurs(std::chrono::year_month_day(
        std::chrono::year(2023),
        std::chrono::month(8),
        std::chrono::day(28)))
    ) << "Monday is not in repeat_info 67";

    ASSERT_FALSE(task.occurs(std::chrono::year_month_day(
        std::chrono::year(2023),
        std::chrono::month(8),
        std::chrono::day(29)))
    ) << "Tuesday is not in repat_info 67";

    db.delete_task(task_data.get());
}

TEST(NAME, test_occurs_monthly)
{
    TaskDatabase db(TESTDBFILE);
    auto uid = db.create_task(TESTTASKNAME);
    auto task_data = db.get_task(uid);
    task_data->repeat_type = RepeatType::Monthly;
    task_data->repeat_info = 20;

    Task task(task_data.get(), &db);

    ASSERT_TRUE(task.occurs(std::chrono::year_month_day(
        std::chrono::year(2023),
        std::chrono::month(8),
        std::chrono::day(20)))
    ) << "20.8.2023 is the 20th day";

    ASSERT_TRUE(task.occurs(std::chrono::year_month_day(
        std::chrono::year(2012),
        std::chrono::month(2),
        std::chrono::day(20)))
    ) << "20.8.2023 is the 20th day";

    ASSERT_FALSE(task.occurs(std::chrono::year_month_day(
        std::chrono::year(2023),
        std::chrono::month(8),
        std::chrono::day(27)))
    ) << "27.8.2023 is not the 20th day";

    db.delete_task(task_data.get());
}

TEST(NAME, test_occurs_monthly_day_friday)
{
    TaskDatabase db(TESTDBFILE);
    auto uid = db.create_task(TESTTASKNAME);
    auto task_data = db.get_task(uid);
    task_data->repeat_type = RepeatType::MonthlyDay;
    task_data->repeat_info = 25; // Second friday

    Task task(task_data.get(), &db);

    ASSERT_TRUE(task.occurs(std::chrono::year_month_day(
        std::chrono::year(2023),
        std::chrono::month(8),
        std::chrono::day(11)))
    ) << "11.8.2023 is the second friday of the month";

    ASSERT_FALSE(task.occurs(std::chrono::year_month_day(
        std::chrono::year(2012),
        std::chrono::month(2),
        std::chrono::day(25)))
    ) << "25.8.2023 is the 3rd friday of the month";

    ASSERT_FALSE(task.occurs(std::chrono::year_month_day(
        std::chrono::year(2023),
        std::chrono::month(8),
        std::chrono::day(27)))
    ) << "27.8.2023 is a monday";

    db.delete_task(task_data.get());
}

TEST(NAME, test_occurs_monthly_day_sunday)
{
    TaskDatabase db(TESTDBFILE);
    auto uid = db.create_task(TESTTASKNAME);
    auto task_data = db.get_task(uid);
    task_data->repeat_type = RepeatType::MonthlyDay;
    task_data->repeat_info = 27; // Second friday

    Task task(task_data.get(), &db);

    ASSERT_TRUE(task.occurs(std::chrono::year_month_day(
        std::chrono::year(2023),
        std::chrono::month(8),
        std::chrono::day(13)))
    ) << "13.8.2023 is the second sunday of the month";

    ASSERT_FALSE(task.occurs(std::chrono::year_month_day(
        std::chrono::year(2012),
        std::chrono::month(2),
        std::chrono::day(25)))
    ) << "25.8.2023 is the 3rd friday of the month";

    ASSERT_FALSE(task.occurs(std::chrono::year_month_day(
        std::chrono::year(2023),
        std::chrono::month(8),
        std::chrono::day(27)))
    ) << "27.8.2023 is a monday";

    db.delete_task(task_data.get());
}


TEST(NAME, test_occurs_interval)
{
    TaskDatabase db(TESTDBFILE);
    auto uid = db.create_task(TESTTASKNAME);
    auto task_data = db.get_task(uid);
    task_data->repeat_type = RepeatType::WithInterval;
    task_data->repeat_info = 10; // Every ten days
    tm start_date{};
    start_date.tm_year = 123;
    start_date.tm_mon = 0;
    start_date.tm_mday = 1;
    start_date.tm_hour = 12;
    task_data->scheduled_start = mktime(&start_date);

    Task task(task_data.get(), &db);

    for (int i = 1; i < 31; ++i) {
        if (i == 1 || i == 11 || i == 21) {
            ASSERT_TRUE(task.occurs(std::chrono::year_month_day(
                std::chrono::year(2023),
                std::chrono::month(1),
                std::chrono::day(i)))
            );
        } else {
            ASSERT_FALSE(task.occurs(std::chrono::year_month_day(
                std::chrono::year(2023),
                std::chrono::month(1),
                std::chrono::day(i)))
            );
        }
    }

    for (int i = 1; i < 28; ++i) {
        if (i == 10 || i == 20) {
            ASSERT_TRUE(task.occurs(std::chrono::year_month_day(
                std::chrono::year(2023),
                std::chrono::month(2),
                std::chrono::day(i)))
            );
        } else {
            ASSERT_FALSE(task.occurs(std::chrono::year_month_day(
                std::chrono::year(2023),
                std::chrono::month(2),
                std::chrono::day(i)))
            );
        }
    }

    db.delete_task(task_data.get());
}


int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
