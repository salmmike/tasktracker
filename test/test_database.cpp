#include <gtest/gtest.h>
#include <iostream>
#include <cassert>
#include <database_driver.h>
#include <task_data.h>

#define NAME database_driver
#define TESTDBFILE "./test.db"
#define TESTTASKNAME "test_task"

TEST(NAME, test_create_database)
{
    try {
        tasktracker::TaskDatabase db(TESTDBFILE);
        db.create_task(TESTTASKNAME);
        db.clear();
    } catch (tasktracker::DatabaseErr &err) {
        FAIL() << "An error was thrown: " << err.what();
    }
}

TEST(NAME, test_get_task)
{
    try {
        tasktracker::TaskDatabase db(TESTDBFILE);
        db.clear();
        db.create_task(TESTTASKNAME);
        auto tasks = db.get_tasks();
        ASSERT_TRUE(tasks[0]->name == TESTTASKNAME) << "created task name isn't what it should be.";
    } catch (tasktracker::DatabaseErr &err) {
        FAIL() << "An error was thrown: " << err.what();
    }
}

TEST(NAME, test_clear_db)
{

    try {
        tasktracker::TaskDatabase db(TESTDBFILE);
        db.clear();
        db.create_task(TESTTASKNAME);
        auto tasks = db.get_tasks();

        ASSERT_EQ(tasks.size(), 1) << "the database should contain one task.";
        db.clear();

        tasks = db.get_tasks();
        ASSERT_EQ(tasks.size(), 0);
    } catch (tasktracker::DatabaseErr &err) {
        FAIL() << "An error was thrown: " << err.what();
    }
}

TEST(NAME, test_delete_task)
{

    try {
        tasktracker::TaskDatabase db(TESTDBFILE);
        db.clear();

        int task_count = 10;
        for (int i = 0; i < task_count; ++i) {
            ASSERT_EQ(db.create_task(TESTTASKNAME), i + 1);
        }

        auto tasks = db.get_tasks();
        ASSERT_EQ(tasks.size(), task_count);

        for (int i = 0; i < task_count; ++i) {
            db.delete_task(tasks[i].get());
            auto after_delete = db.get_tasks();
            ASSERT_EQ(after_delete.size(), task_count - (i+1));
        }
    } catch (tasktracker::DatabaseErr &err) {
        FAIL() << "An error was thrown: " << err.what();
    }
}

TEST(NAME, test_task_creation)
{
    try {
        tasktracker::TaskDatabase db(TESTDBFILE);
        db.clear();
        for (int i = 0; i < 100; ++i) {
            ASSERT_EQ(db.create_task(TESTTASKNAME), i + 1);
        }

        auto tasks = db.get_tasks();

        for (int i = 0; i < 100; ++i) {
            ASSERT_EQ(tasks[i]->id, i + 1);
            ASSERT_EQ(tasks[i]->name, TESTTASKNAME);
        }
        db.clear();
    } catch (tasktracker::DatabaseErr &err) {
        FAIL() << "An error was thrown: " << err.what();
    }
}

TEST(NAME, test_task_update)
{
    try {
        tasktracker::TaskDatabase db(TESTDBFILE);
        const std::string new_name = "new_name";
        const std::string new_state = "new_state";
        const std::string new_comment = "test comment";

        int new_start = 100;
        int new_repeat_info = 150;
        auto new_repeat_type = tasktracker::RepeatType::WithInterval;


        db.clear();
        int id = db.create_task(TESTTASKNAME);

        auto task_unique = db.get_task(id);
        auto task = task_unique.get();
        ASSERT_NE(task, nullptr);

        task->name = new_name;
        task->state = new_state;
        task->scheduled_start = new_start;
        task->repeat_info = new_repeat_info;
        task->comment = new_comment;
        task->repeat_type = new_repeat_type;
        db.update_task(task);

        auto task2_unique = db.get_task(id);
        auto task2 = task2_unique.get();
        ASSERT_NE(task, nullptr);

        ASSERT_EQ(task2->name, new_name);
        ASSERT_EQ(task2->state, new_state);
        ASSERT_EQ(task2->scheduled_start, new_start);
        ASSERT_EQ(task2->repeat_info, new_repeat_info);
        ASSERT_EQ(task2->comment, new_comment);
        ASSERT_EQ(task2->repeat_type, new_repeat_type);
        db.clear();
    } catch (tasktracker::DatabaseErr &err) {
        FAIL() << "An error was thrown: " << err.what();
    }
}

TEST(NAME, test_taskinstance_update)
{
    try {
        std::string test_task_id = "testabc123";
        tasktracker::TaskInstanceDatabase db(TESTDBFILE);
        db.create_task(1, test_task_id, TESTTASKNAME);

        const time_t new_scheduled_start = 1000;
        const time_t new_finish = 1234;
        const std::chrono::seconds new_time_spent = std::chrono::seconds(50);
        const std::string new_comment = "test comment";
        const tasktracker::TaskState new_state = tasktracker::TaskState::Finished;

        auto task_instance = db.get_task(test_task_id);
        task_instance->scheduled_start = new_scheduled_start;
        task_instance->finish_time = new_finish;
        task_instance->time_spent = new_time_spent;
        task_instance->comment = new_comment;
        task_instance->state = new_state;
        db.update_task(task_instance.get());

        auto task_instance2 = db.get_task(test_task_id);
        ASSERT_EQ(task_instance2->scheduled_start, new_scheduled_start);
        ASSERT_EQ(task_instance2->finish_time, new_finish);
        ASSERT_EQ(task_instance2->time_spent, new_time_spent);
        ASSERT_EQ(task_instance2->comment, new_comment);
        ASSERT_EQ(task_instance2->state, new_state);
        db.clear();
    } catch (tasktracker::DatabaseErr &err) {
        FAIL() << "An error was thrown: " << err.what();
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
