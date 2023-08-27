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

#ifndef TASKTRACKLIB_H
#define TASKTRACKLIB_H

#include <map>
#include <task_data.h>
#include <database_driver.h>
#include <task.h>

namespace tasktracker {

/// @brief Class for keeping track of tasks.
class TaskTracker
{
public:
    /// @brief Create a TaskTracker
    /// @param path path to the file used as database
    explicit TaskTracker(std::filesystem::path path);
    ~TaskTracker();

    /// @brief get tasks scheduled for date. This object must not leave scope while the results are used.
    /// @param date the date when the tasks are scheduled
    /// @return list of TaskInstance objects.
    std::vector<TaskInstance*> get_task_instances(std::chrono::year_month_day date);
    std::vector<TaskInstance*> get_task_instances(tm date);

    /// @brief Add a new task
    /// @param name name of the task
    /// @param repeat_type RepeatType enum
    /// @param repeat_info repeat info, handled based on repeat_type
    /// @param start_time First date of the task and the time when the task should be scheduled.
    void add_task(const std::string& name, RepeatType repeat_type, int repeat_info, tm start_time);
    void add_task(const std::string& name, RepeatType repeat_type, int repeat_info, std::chrono::year_month_day start_date, std::chrono::hours hour, std::chrono::minutes mins);

    /// @brief Clear the database used by this TaskTracker
    void clear();

private:
    const std::unique_ptr<TaskInstanceDatabase> _task_instance_db;
    const std::unique_ptr<TaskDatabase> _task_db;

    std::vector<std::unique_ptr<TaskData>> _task_data;
    std::map<std::string, std::unique_ptr<TaskInstance>> _task_instances;
    std::vector<std::unique_ptr<Task>> _tasks;

    /// @brief Create a unique identifier for a TaskInstance based on the Task and it's date
    /// @param day the scheduled date
    /// @return 
    std::string _create_identifier(std::chrono::year_month_day day, Task* task);
    std::string _create_identifier(tm day, Task* task);

    void _create_task_intance(const std::unique_ptr<Task>& task, tm date, const std::string& instance_id);

    void _load_tasks();
};
}

#endif /* TASKTRACKLIB_H */
