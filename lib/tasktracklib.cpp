#include "tasktracklib.h"

#include <algorithm>
#include <iostream>

namespace tasktracker {

std::string
format_date(tm* date, const std::string& str = "")
{
    char buffer[26];
    strftime(buffer, 26, "%a %Y-%m-%d %H:%M:%S", date);
    return str + buffer;
}

std::string
format_date(time_t* date, const std::string& str = "")
{
    tm* time_tm = localtime(date);
    return format_date(time_tm, str);
}

TaskTracker::TaskTracker(std::filesystem::path path)
  : m_task_instance_db(std::make_unique<TaskInstanceDatabase>(path))
  , m_task_db(std::make_unique<TaskDatabase>(path))
{
    m_task_instance_db->init();
    m_task_db->init();
    m_load_tasks();
}

TaskTracker::~TaskTracker() {}

std::vector<TaskInstance*>
TaskTracker::get_task_instances(std::chrono::year_month_day date)
{
    tm tm_date{};

    tm_date.tm_year = static_cast<int>(date.year()) - 1900;
    tm_date.tm_mon = static_cast<unsigned int>(date.month()) - 1;
    tm_date.tm_mday = static_cast<unsigned int>(date.day());
    return get_task_instances(tm_date);
}

std::vector<TaskInstance*>
TaskTracker::get_task_instances(tm date)
{
    std::vector<TaskInstance*> task_instances;

    for (auto& task : m_tasks) {
        if (task->occurs(date)) {
            auto instance_id = m_create_identifier(date, task.get());
            if (!m_task_instances.contains(instance_id)) {
                m_create_task_instance(task, date, instance_id);
            }
            task_instances.push_back(m_task_instances.at(instance_id).get());
        }
    }

    std::sort(task_instances.begin(),
              task_instances.end(),
              [](const auto& a, const auto& b) {
                  if (a->get_scheduled_time() == b->get_scheduled_time())
                    [[unlikely]] {
                      return a->get_name() < b->get_name();
                  }
                  return a->get_scheduled_time() < b->get_scheduled_time();
              });

    return task_instances;
}

void
TaskTracker::add_task(const std::string& name,
                      RepeatType repeat_type,
                      int repeat_info,
                      std::chrono::year_month_day start_date,
                      std::chrono::hours hour,
                      std::chrono::minutes mins)
{
    tm start_time{};

    start_time.tm_year = static_cast<int>(start_date.year()) - 1900;
    start_time.tm_mon = static_cast<unsigned int>(start_date.month()) - 1;
    start_time.tm_mday = static_cast<unsigned int>(start_date.day());

    start_time.tm_hour = hour.count();
    start_time.tm_min = mins.count();
    add_task(name, repeat_type, repeat_info, start_time);
}

void
TaskTracker::delete_task(int id)
{
    auto task = m_task_db->get_task(id);
    if (task == nullptr) {
        return;
    }

    m_task_db->delete_task(task.get());

    const auto it =
      std::find_if(m_tasks.begin(), m_tasks.end(), [id](const auto& n_task) {
          return n_task->get_id() == id;
      });
    if (it != m_tasks.end()) {
        m_tasks.erase(it);
    }
}

void
TaskTracker::add_task(const std::string& name,
                      RepeatType repeat_type,
                      int repeat_info,
                      tm start_time)
{
    auto id = m_task_db->create_task(name);
    auto task = m_task_db->get_task(id);
    task->repeat_type = repeat_type;
    task->repeat_info = repeat_info;
    task->scheduled_start = mktime(&start_time);
    m_task_db->update_task(task.get());

    m_task_data.push_back(std::move(task));

    auto task_ =
      std::make_unique<Task>(m_task_data.back().get(), m_task_db.get());
    m_tasks.push_back(std::move(task_));
}

void
TaskTracker::add_task(const std::string& name,
                      RepeatType repeat_type,
                      int repeat_info,
                      time_t start_time)
{
    tm* start_time_tm = localtime(&start_time);
    add_task(name, repeat_type, repeat_info, *start_time_tm);
}

void
TaskTracker::clear()
{
    m_task_db->clear();
    m_task_instance_db->clear();
    m_load_tasks();
}

std::vector<Task*>
TaskTracker::get_tasks()
{
    std::vector<Task*> tasks;

    for (const auto& task : m_tasks) {
        tasks.push_back(task.get());
    }

    return tasks;
}

Task*
TaskTracker::get_task(int id)
{
    const auto it =
      std::find_if(m_tasks.begin(), m_tasks.end(), [id](const auto& task) {
          return (task->get_id() == id);
      });
    if (it == m_tasks.end()) {
        return nullptr;
    }
    return it->get();
}

void
TaskTracker::modify_task(const TaskData* task)
{
    m_task_db->update_task(task);
}

std::string
TaskTracker::m_create_identifier(std::chrono::year_month_day day, Task* task)
{
    tm tm_day{};

    tm_day.tm_year = static_cast<int>(day.year()) - 1900;
    tm_day.tm_mon = static_cast<unsigned int>(day.month()) - 1;
    tm_day.tm_mday = static_cast<unsigned int>(day.day());

    return m_create_identifier(tm_day, task);
}

std::string
TaskTracker::m_create_identifier(tm day, Task* task)
{
    std::stringstream ss;
    ss << task->get_id() << "-" << day.tm_year << "-" << day.tm_mon << "-"
       << day.tm_mday;
    auto str = task->get_name() + "-" + ss.str();
    return str;
}

void
TaskTracker::m_create_task_instance(const std::unique_ptr<Task>& task,
                                    tm date,
                                    const std::string& instance_id)
{
    auto task_instance_data = m_task_instance_db->get_task(instance_id);

    if (task_instance_data == nullptr) {
        m_task_instance_db->create_task(
          task->get_id(), instance_id, task->get_name());
        task_instance_data = m_task_instance_db->get_task(instance_id);

        tm start_time = date;
        start_time.tm_year = date.tm_year;
        start_time.tm_mday = date.tm_mday;
        start_time.tm_mon = date.tm_mon;
        start_time.tm_hour = task->get_scheduled_start_time().hours.count();
        start_time.tm_min = task->get_scheduled_start_time().minutes.count();

        task_instance_data->scheduled_start = mktime(&start_time);
        m_task_instance_db->update_task(task_instance_data.get());
    }

    task_instance_data = m_task_instance_db->get_task(instance_id);
    auto task_instance = std::make_unique<TaskInstance>(
      std::move(task_instance_data), m_task_instance_db.get());
    m_task_instances.insert({ instance_id, std::move(task_instance) });
}

void
TaskTracker::m_load_tasks()
{
    m_task_data.clear();
    m_tasks.clear();

    m_task_data = m_task_db->get_tasks();

    for (const auto& task_data : m_task_data) {
        m_tasks.push_back(
          std::make_unique<Task>(task_data.get(), m_task_db.get()));
    }
}

} // namespace tasktracker