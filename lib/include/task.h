/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Author: Mike Salmela
 */

#ifndef TASK_H
#define TASK_H

#include <database_driver.h>

namespace tasktracker {

struct ScheduledTime {
    std::chrono::hours hours;
    std::chrono::minutes minutes;
};

/// @brief Class for interracting with TaskInstanceData
class TaskInstance
{
public:
    explicit TaskInstance(std::unique_ptr<TaskInstanceData> &&data, TaskInstanceDatabase* db);
    ~TaskInstance();

    std::string get_name();

    std::string get_uid();

    /// @brief Set the task state to started, update start_time to current time.
    void start_task();

    /// @brief Set the task state to skipped
    void skip_task();

    /// @brief Set the task state to finished, log finish time.
    void finish_task();

    /// @brief get the time when the task is scheduled to start
    /// @return the of scheduled start
    time_t get_scheduled_time();

    /// @brief Get time spent on the task
    /// @return time in seconds
    std::chrono::seconds get_time_spent();

    /// @brief Set a comment for the task
    void set_comment(const std::string& str);

    /// @brief Get related comment
    /// @return the comment as a string
    std::string get_comment();

    /// @brief get the TaskInstanceData
    /// @return pointer to TaskInstanceData
    const TaskInstanceData* get_data();

private:
    std::unique_ptr<TaskInstanceData> _data;
    TaskInstanceDatabase* _db;
};

class Task
{
public:
    explicit Task(TaskData* data, TaskDatabase* db);
    ~Task();

    /// @brief Get the time the task starts
    /// @return ScheduledTime struct
    ScheduledTime get_scheduled_start_time();

    std::string get_name();

    int get_id();

    std::string get_comment();

    /// @brief Check if Task occurs on given day
    /// @param day the day to check
    /// @return true if the Task occurs on the day
    bool occurs(tm day);
    bool occurs(std::chrono::year_month_day day);

private:
    TaskData* _data;
    TaskDatabase* _db;

};

}

#endif /* TASK_H */
