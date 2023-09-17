#include "database_driver.h"

#include <algorithm>
#include <iostream>
#include <sstream>

#define TASK_ID "ID"
#define TASK_NAME "TASKNAME"
#define TIME_SPENT "TIMESPENT"
#define START_TIME "START_TIME"
#define REPEAT_TYPE "REPEATTYPE"
#define REPEAT_INFO "REPEATINFO"
#define TASK_STATE "TASKSTATE"
#define TASK_COMMENT "COMMENT"
#define TASK_BEGINNING "BEGINNING"
#define PARENT_ID "PARENT_ID"
#define FINISH_TIME "FINISHTIME"
#define STATE "STATE"

namespace tasktracker {

std::string
num_to_string(auto num)
{
    std::stringstream ss;
    ss << num;
    return ss.str();
}

static int
get_id_cb(void* data, int argc, char** argv, char** column)
{
    (void)column;
    int* id = static_cast<int*>(data);
    int id_value = 0;

    for (int i = 0; i < argc; ++i) {
        std::stringstream(argv[i]) >> id_value;
    }
    (*id) = id_value;

    return 0;
}

static int
s_get_task_cb(void* data, int argc, char** argv, char** column)
{
    (void)column;
    auto res = static_cast<std::vector<std::unique_ptr<TaskData>>*>(data);
    auto task = std::make_unique<TaskData>();

    for (int i = 0; i < argc; ++i) {
        switch (i) {
            case 0:
                std::stringstream(argv[i]) >> task->id;
                break;
            case 1:
                task->name = argv[i];
                break;
            case 2:
                if (argv[i] != 0) {
                    std::stringstream(argv[i]) >> task->scheduled_start;
                }
                break;
            case 3:
                if (argv[i] != 0)
                    task->state = argv[i];
                break;
            case 4:
                if (argv[i] != 0)
                    task->comment = argv[i];
                break;
            case 5:
                if (argv[i] != 0) {
                    int value = 0;
                    std::stringstream(argv[i]) >> value;
                    task->repeat_type = static_cast<RepeatType>(value);
                }
                break;
            case 6:
                if (argv[i] != 0)
                    std::stringstream(argv[i]) >> task->repeat_info;
                break;
            default:
                break;
        }
    }
    res->push_back(std::move(task));
    return 0;
}

static int
s_get_task_instance_cb(void* data, int argc, char** argv, char** column)
{
    (void)column;
    auto* res =
      static_cast<std::vector<std::unique_ptr<TaskInstanceData>>*>(data);
    auto task = std::make_unique<TaskInstanceData>();
    for (int i = 0; i < argc; ++i) {
        switch (i) {
            case 0:
                task->id = argv[i];
                break;
            case 1:
                std::stringstream(argv[i]) >> task->parent_id;
                break;
            case 2:
                if (argv[i] != 0) {
                    task->name = argv[i];
                }
                break;
            case 3:
                if (argv[i] != 0)
                    std::stringstream(argv[i]) >> task->scheduled_start;
                break;
            case 4:
                if (argv[i] != 0)
                    std::stringstream(argv[i]) >> task->start_time;
                break;
            case 5:
                if (argv[i] != 0)
                    std::stringstream(argv[i]) >> task->finish_time;
                break;
            case 6:
                if (argv[i] != 0) {
                    long long value;
                    std::stringstream(argv[i]) >> value;
                    task->time_spent = static_cast<std::chrono::seconds>(value);
                }
                break;
            case 7:
                if (argv[i] != 0)
                    task->comment = argv[i];
                break;
            case 8:
                if (argv[i] != 0) {
                    int value = 0;
                    std::stringstream(argv[i]) >> value;
                    task->state = static_cast<TaskState>(value);
                }
            default:
                break;
        }
    }
    res->push_back(std::move(task));
    return 0;
}

std::string
escape_quote(const std::string& str)
{
    auto cpy = str;
    std::replace(begin(cpy), end(cpy), '\'', '`');
    return cpy;
}

DatabaseDriver::DatabaseDriver(std::filesystem::path path,
                               std::string table_name)
  : m_path(path)
  , m_table(table_name)
{
}

void
DatabaseDriver::init() noexcept(false)
{
    m_make_table();
}

void
DatabaseDriver::clear()
{
    m_open_db();
    auto str = "DROP TABLE " + m_table + ";";
    m_execute(str);
    m_close_db();
    m_make_table();
}

void
DatabaseDriver::m_execute(const std::string& statement,
                          void* return_value,
                          int (*callback)(void*, int, char**, char**))
{
    char* err = nullptr;
    int ern =
      sqlite3_exec(m_db, statement.c_str(), callback, return_value, &err);

    if (ern != SQLITE_OK) {
        auto exept =
          DatabaseErr("Executing statement\n" + statement + "\nfailed: " + err);
        m_close_db();
        sqlite3_free(err);
        throw exept;
    }
}

void
DatabaseDriver::m_open_db()
{
    if (sqlite3_open(m_path.c_str(), &m_db) != SQLITE_OK) {
        throw DatabaseErr("Database " + m_path.string() + " didn't open.");
    }
}

void
DatabaseDriver::m_close_db()
{
    if (m_db) {
        sqlite3_close(m_db);
    }
}

TaskDatabase::TaskDatabase(std::filesystem::path path)
  : DatabaseDriver(path, TASKS_TABLE_NAME)
{
}

void
TaskDatabase::m_make_table()
{
    m_open_db();
    const std::string str =
      "CREATE TABLE IF NOT EXISTS " + m_table +
      "(" TASK_ID "        INTEGER PRIMARY KEY, " TASK_NAME
      "      TEXT NOT NULL, " TASK_BEGINNING " INTEGER, " TASK_STATE
      "     INTEGER, " TASK_COMMENT "   TEXT, " REPEAT_TYPE
      "    INTEGER, " REPEAT_INFO "    INTEGER"
      ");";

    m_execute(str);
    m_close_db();
}

int
TaskDatabase::create_task(const std::string& task)
{
    m_open_db();
    auto str = "INSERT INTO " + m_table + " VALUES(NULL, '" +
               escape_quote(task) + "', NULL, NULL, NULL, NULL, NULL);";
    m_execute(str);

    str = "SELECT last_insert_rowid();";
    int id = 0;
    m_execute(str, &id, get_id_cb);
    m_close_db();
    return id;
}

void
TaskDatabase::update_task(const TaskData* task)
{
    m_open_db();
    auto str = "UPDATE " + m_table + " SET " + TASK_NAME "='" +
               escape_quote(task->name) + "', " + TASK_BEGINNING "='" +
               num_to_string(task->scheduled_start) + "', " + TASK_STATE "='" +
               escape_quote(task->state) + "', " + TASK_COMMENT "='" +
               escape_quote(task->comment) + "', " + REPEAT_TYPE "='" +
               num_to_string(static_cast<int>(task->repeat_type)) + "', " +
               REPEAT_INFO "='" + num_to_string(task->repeat_info) + "' " +
               "WHERE " TASK_ID "=" + num_to_string(task->id) + ";";

    m_execute(str);
    m_close_db();
}

void
TaskDatabase::delete_task(const TaskData* task)
{
    m_open_db();
    std::string str = "DELETE FROM " + m_table + " WHERE " TASK_ID "=" +
                      num_to_string(task->id) + ";";

    m_execute(str);
    m_close_db();
}

std::vector<std::unique_ptr<TaskData>>
TaskDatabase::get_tasks(const std::string& task)
{
    std::string str = "SELECT * FROM " + m_table;
    std::vector<std::unique_ptr<TaskData>> res;

    if (!task.empty()) {
        str += " WHERE ";
        str += (std::string(TASK_NAME) + "='" + task + "'");
    }
    str += ";";

    m_open_db();
    m_execute(str, &res, s_get_task_cb);
    m_close_db();

    return res;
}

std::unique_ptr<TaskData>
TaskDatabase::get_task(int id)
{
    std::string query = "SELECT * FROM " + m_table + " WHERE " TASK_ID "='" +
                        num_to_string(id) + "';";
    std::vector<std::unique_ptr<TaskData>> result;

    m_open_db();
    m_execute(query, &result, s_get_task_cb);
    m_close_db();

    if (result.size() == 1) {
        return std::move(result.at(0));
    }

    return nullptr;
}

TaskInstanceDatabase::TaskInstanceDatabase(std::filesystem::path path)
  : DatabaseDriver(path, TASK_INSTANCES_TABLE_NAME)
{
}

void
TaskInstanceDatabase::create_task(const int& parent_task,
                                  const std::string& uid,
                                  const std::string& name) noexcept(false)
{
    m_open_db();
    auto str = "INSERT INTO " + m_table + " VALUES('" + escape_quote(uid) +
               "', '" + num_to_string(parent_task) + "', '" + name + "', " +
               "NULL, NULL, NULL, NULL, NULL, NULL);";
    m_execute(str);
    m_close_db();
}

void
TaskInstanceDatabase::update_task(const TaskInstanceData* task) noexcept(false)
{
    m_open_db();
    auto str = "UPDATE " + m_table + " SET " + TASK_NAME "='" +
               escape_quote(task->name) + "', " + TASK_BEGINNING "='" +
               num_to_string(task->scheduled_start) + "', " + START_TIME "='" +
               num_to_string(task->start_time) + "', " + FINISH_TIME "='" +
               num_to_string(task->finish_time) + "', " + TIME_SPENT "='" +
               num_to_string(task->time_spent.count()) + "', " +
               TASK_COMMENT "='" + escape_quote(task->comment) + "', " +
               TASK_STATE "='" + num_to_string(static_cast<int>(task->state)) +
               "' " + "WHERE " TASK_ID "='" + task->id + "';";

    m_execute(str);
    m_close_db();
}

void
TaskInstanceDatabase::delete_task(const TaskInstanceData* task) noexcept(false)
{
    m_open_db();
    std::string str = "DELETE FROM " + m_table + " WHERE " TASK_ID "=" +
                      num_to_string(task->id) + ";";

    m_execute(str);
    m_close_db();
}

std::vector<std::unique_ptr<TaskInstanceData>>
TaskInstanceDatabase::get_tasks(const size_t parent_id,
                                bool not_done) noexcept(false)
{
    std::string str = "SELECT * FROM " + m_table;
    std::vector<std::unique_ptr<TaskInstanceData>> res;

    if (parent_id > 0 || not_done) {
        str += " WHERE ";
    }

    if (parent_id > 0) {
        str += (std::string(PARENT_ID) + "='" + num_to_string(parent_id) + "'");
        if (not_done) {
            str += " AND ";
        }
    }
    if (not_done) {
        str += (std::string(TASK_STATE) + " NOT 'DONE'");
    }
    str += ";";

    m_open_db();
    m_execute(str, &res, s_get_task_cb);
    m_close_db();

    return res;
}

std::unique_ptr<TaskInstanceData>
TaskInstanceDatabase::get_task(const std::string& id) noexcept(false)
{
    std::string query = "SELECT * FROM " + m_table + " WHERE " TASK_ID "='" +
                        escape_quote(id) + "';";
    std::vector<std::unique_ptr<TaskInstanceData>> result;

    m_open_db();
    m_execute(query, &result, s_get_task_instance_cb);
    m_close_db();

    if (result.size() == 1) {
        return std::move(result.at(0));
    }

    return nullptr;
}

void
TaskInstanceDatabase::m_make_table()
{
    m_open_db();
    const std::string str =
      "CREATE TABLE IF NOT EXISTS " + m_table +
      "(" TASK_ID "            STRING PRIMARY KEY, " PARENT_ID
      "          INTEGER, " TASK_NAME "          TEXT NOT NULL, " TASK_BEGINNING
      "     INTEGER, " START_TIME "         INTEGER, " FINISH_TIME
      "        INTEGER, " TIME_SPENT "         INTEGER, " TASK_COMMENT
      "       TEXT, " TASK_STATE "         INTEGER"
      ");";

    m_execute(str);
    m_close_db();
}

} // namespace tasktracker
