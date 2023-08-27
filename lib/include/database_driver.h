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


#ifndef DATABASE_DRIVER_H
#define DATABASE_DRIVER_H

#include <string>
#include <sqlite3.h>
#include <vector>
#include <exception>
#include <filesystem>
#include <memory>
#include "task_data.h"

namespace tasktracker{

const std::string TASKS_TABLE_NAME = "TASKS";
const std::string TASK_INSTANCES_TABLE_NAME = "TASKINSTANCES";

class DatabaseErr: public std::exception
{
public:
    explicit DatabaseErr(const std::string& msg): message(msg) {};
    const char* what() {
        return message.c_str();
    }

private:
    const std::string message;
};

class DatabaseDriver
{
public:
    explicit DatabaseDriver(std::filesystem::path path, std::string table_name);
    virtual ~DatabaseDriver() {};

    /// @brief Clear the whole task database.
    /// @throws DatabaseErr on exception.
    void clear() noexcept(false);

protected:
    virtual void _make_table()  noexcept(false) {};
    void _open_db() noexcept(false);
    void _close_db() noexcept(false);
    void _execute(const std::string& statement, void* return_value=NULL,
                    int(*callback)(void*, int, char**, char**)=NULL) noexcept(false);

    const std::filesystem::path _path;
    sqlite3* _db {nullptr};
    const std::string _table;
};

/// @brief Interraction handler with Task SQL database for Task data,
/// e.g. the information of when a task should be scheduled, etc.
class TaskDatabase : protected DatabaseDriver
{
public:
    /// @brief Create a new taskdatabase item with databasefile found in path.
    // if the database file isn't found, it's created.
    /// @param path path to the database.
    /// @throws DatabaseErr on exception.
    explicit TaskDatabase(std::filesystem::path path) noexcept(false);
    ~TaskDatabase() {};

    /// @brief Create a new task
    /// @param task name of the task
    /// @return unique database ID of the task
    /// @throws DatabaseErr on exception.
    int create_task(const std::string &task) noexcept(false);

    using DatabaseDriver::clear;

    /// @brief update the database with task data
    /// @param task pointer to the TaskData to be updated. task->id must exist
    /// @throws DatabaseErr on exception.
    void update_task(const TaskData* task) noexcept(false);

    /// @brief delete a task.
    /// @param task pointer to the TaskData containing the ID
    /// @throws DatabaseErr on exception.
    void delete_task(const TaskData* task) noexcept(false);

    /// @brief get a list of TaskData
    /// @param task if this is set, find tasks with matching name
    /// @return matching tasks
    /// @throws DatabaseErr on exception.
    std::vector<std::unique_ptr<TaskData>> get_tasks(const std::string &task = "") noexcept(false);

    /// @brief get task with id
    /// @param id the unique ID
    /// @return unique pointer to TaskData or nullptr.
    /// @throws DatabaseErr on exception.
    std::unique_ptr<TaskData> get_task(int id) noexcept(false);

private:
    void _make_table() override;
};


/// @brief Interraction handler with SQL database for individual task events,
/// like a single task of mopping the floor, when was it finished after scheduling, etc.
class TaskInstanceDatabase : protected DatabaseDriver
{
public:
    /// @brief Create a new TaskInstanceDatabase item with databasefile found in path.
    // if the database file isn't found, it's created.
    /// @param path path to the database.
    /// @throws DatabaseErr on exception.
    explicit TaskInstanceDatabase(std::filesystem::path path) noexcept(false);
    ~TaskInstanceDatabase() {};

    using DatabaseDriver::clear;

    /// @brief Create a new TaskInstance
    /// @param parent_task
    /// @param uid Unique string for the task instance
    /// @param name
    /// @throws DatabaseErr on exception.
    void create_task(const int &parent_task, const std::string& uid, const std::string &name) noexcept(false);

    /// @brief update the database with TaskInstanceData data
    /// @param task pointer to the TaskData to be updated. task->id must exist
    /// @throws DatabaseErr on exception.
    void update_task(const TaskInstanceData* task) noexcept(false);

    /// @brief delete a task.
    /// @param task pointer to the TaskData containing the ID
    /// @throws DatabaseErr on exception.
    void delete_task(const TaskInstanceData* task) noexcept(false);

    /// @brief get a list of TaskInstanceData
    /// @param parent_id if > 0  find tasks with matching parent_id.
    /// @param not_done if this is true, only return tasks that are not done.
    /// @return matching TaskInstanceData
    /// @throws DatabaseErr on exception.
    std::vector<std::unique_ptr<TaskInstanceData>> get_tasks(const size_t parent_id=0, bool not_done=false) noexcept(false);

    /// @brief get TaskInstanceData with id
    /// @param id the unique ID
    /// @return unique pointer to TaskInstanceData or nullptr.
    /// @throws DatabaseErr on exception.
    std::unique_ptr<TaskInstanceData> get_task(const std::string& id) noexcept(false);

private:
    void _make_table() override;

};

}

#endif
