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

#ifndef TASK_DATA_H
#define TASK_DATA_H

#include <string>
#include <chrono>

namespace tasktracker {

enum RepeatType {
    /// @brief event only happens once.
    NoRepeat,
    /// @brief if repeat_info is e.g. 5, repeats on every 5th day of the month.
    Monthly,
    /// @brief if repeat_info is e.g. 25, repeats on the 5th day of the second week.
    /// if the month begins in the middle of the week and repeat_info is e.g. 11,
    /// the event happens on the first available monday.
    MonthlyDay,
    /// @brief if repeat_info is 1, event happens every monday. If repeat_info is 123,
    /// the event happens every monday, tuesday and wednesday.
    SpecifiedDays,
    /// @brief if repeat_info is e.g. 10, the event happens every 10 days.
    WithInterval
};

/// @brief Data or periodic tasks that should be ran.
struct TaskData {
    /// @brief unique task ID
    size_t id;
    /// @brief task name
    std::string name;
    /// @brief scheduled time for the task to start. Date represents the first occurance of the task,
    /// the repeated tasks are calculated from this time. Task instances related to this task always start
    /// at the same time as the original one.
    time_t scheduled_start;
    /// @brief state of the task (active, inactive)
    std::string state;
    /// @brief comment for task
    std::string comment;
    /// @brief type of repeat (defines how the repeat_info
    /// is treated or if no repeat happens)
    RepeatType repeat_type;
    /// @brief info of repeating, handled according to repeat_type
    int repeat_info;
};

enum TaskState {
    NotStarted,
    Started,
    Finished,
    Skipped
};

/// @brief Data about a single task that is to be done or done.
struct TaskInstanceData
{
    /// @brief unique ID for the finished task
    std::string id;
    /// @brief reference to the TaskData
    size_t parent_id;
    /// @brief name of the task
    std::string name;
    /// @brief when the task was scheduled to start
    time_t scheduled_start;
    /// @brief when the task was started
    time_t start_time;
    /// @brief when the task was finished
    time_t finish_time;
    /// @brief time spent in minutes
    std::chrono::seconds time_spent;
    /// @brief comment for task
    std::string comment;
    /// @brief state of the task.
    TaskState state;
};

}

#endif /* TASK_DATA_H */
