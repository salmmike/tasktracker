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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 *
 * Author: Mike Salmela
 */

#ifndef TASK_H
#define TASK_H

#include "database_driver.h"

namespace tasktracker {

struct ScheduledTime
{
    std::chrono::hours hours;
    std::chrono::minutes minutes;
};

/// @brief Class for interracting with TaskInstanceData
class TaskInstance
{
  public:
    explicit TaskInstance(std::unique_ptr<TaskInstanceData>&& data,
                          TaskInstanceDatabase* db);
    ~TaskInstance();

    std::string get_name() const;

    std::string get_uid() const;

    /// @brief Set the task state to started, update start_time to current
    /// time.
    void start_task();

    /// @brief Set the task state to skipped
    void skip_task();

    /// @brief Set the task state to finished, log finish time.
    void finish_task();

    /// @brief Set the task back to not started state.
    void set_undone();

    /// @brief get the date and time when the task is scheduled to start
    /// @return the date and time of scheduled start
    time_t get_scheduled_datetime() const;

    /// @brief get hours and minutes of the start time
    /// @return hours and minutes of the start time in a time_t struct
    time_t get_scheduled_time() const;

    /// @brief Get time spent on the task
    /// @return time in seconds
    std::chrono::seconds get_time_spent() const;

    /// @brief Set a comment for the task
    void set_comment(const std::string& str);

    /// @brief Get related comment
    /// @return the comment as a string
    std::string get_comment() const;

    /// @brief get the TaskInstanceData
    /// @return pointer to TaskInstanceData
    const TaskInstanceData* get_data() const;

    /// @brief Check if task is finished
    /// @return true if finished
    bool is_finished() const;

    /// @brief Check if task is skipped
    /// @return true if skipped
    bool is_skipped() const;

    /// @brief Check if task is started
    /// @return true if started
    bool is_started() const;

  private:
    std::unique_ptr<TaskInstanceData> m_data;
    TaskInstanceDatabase* m_db;
};

class Task
{
  public:
    explicit Task(TaskData* data, TaskDatabase* db);
    ~Task();

    /// @brief Get the time the task starts
    /// @return ScheduledTime struct
    ScheduledTime get_scheduled_start_time();

    /// @brief Get the time the task starts
    /// @return time_t
    time_t get_scheduled_start_time_t() const;

    /// @brief get the name of the task
    /// @return name as a string
    std::string get_name() const;

    /// @brief get the unique id
    /// @return unique id
    int get_id() const;

    /// @brief get Task comment
    /// @return saved comment as string
    std::string get_comment();

    /// @brief set the task comment
    /// @param comment the new comment
    void set_comment(const std::string& comment);

    /// @brief Check if Task occurs on given day
    /// @param day the day to check
    /// @return true if the Task occurs on the day
    bool occurs(tm day);
    bool occurs(std::chrono::year_month_day day);

    TaskData* get_data() { return m_data; };

  private:
    TaskData* m_data;
    TaskDatabase* m_db;
};

} // namespace tasktracker

#endif /* TASK_H */
