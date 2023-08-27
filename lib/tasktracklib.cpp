#include "tasktracklib.h"
#include <iostream>

namespace tasktracker {

TaskTracker::TaskTracker(std::filesystem::path path):
    _task_instance_db(std::make_unique<TaskInstanceDatabase>(path)),
    _task_db(std::make_unique<TaskDatabase>(path))
{
    _load_tasks();
}

TaskTracker::~TaskTracker()
{
}

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

    for (auto &task : _tasks) {
        if (task->occurs(date)) {
            auto instance_id = _create_identifier(date, task.get());
            if (!_task_instances.contains(instance_id)) {
                _create_task_intance(task, date, instance_id);
            }
            task_instances.push_back(_task_instances.at(instance_id).get());
        }
    }
    return task_instances;
}

void
TaskTracker::add_task(const std::string &name, RepeatType repeat_type, int repeat_info, std::chrono::year_month_day start_date, std::chrono::hours hour, std::chrono::minutes mins)
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
TaskTracker::add_task(const std::string &name, RepeatType repeat_type, int repeat_info, tm start_time)
{
    auto id = _task_db->create_task(name);
    auto task = _task_db->get_task(id);
    task->repeat_type = repeat_type;
    task->repeat_info = repeat_info;
    task->scheduled_start = mktime(&start_time);
    _task_db->update_task(task.get());

    _task_data.push_back(std::move(task));

    auto task_ = std::make_unique<Task>(_task_data.back().get(), _task_db.get());
    _tasks.push_back(std::move(task_));
}

void TaskTracker::clear()
{
    _task_db->clear();
    _task_instance_db->clear();
    _load_tasks();
}

std::string
TaskTracker::_create_identifier(std::chrono::year_month_day day, Task *task)
{

    tm tm_day{};

    tm_day.tm_year = static_cast<int>(day.year()) - 1900;
    tm_day.tm_mon = static_cast<unsigned int>(day.month()) - 1;
    tm_day.tm_mday = static_cast<unsigned int>(day.day());

    return _create_identifier(tm_day, task);
}

std::string
TaskTracker::_create_identifier(tm day, Task *task)
{
    std::stringstream ss;
    ss << task->get_id() << "-" << day.tm_year << "-" << day.tm_mon << "-" << day.tm_mday;
    return task->get_name() + "-" + ss.str();
}

void TaskTracker::_create_task_intance(const std::unique_ptr<Task> &task, tm date, const std::string& instance_id)
{
    auto task_instance_data = _task_instance_db->get_task(instance_id);

    if (task_instance_data == nullptr) {
        _task_instance_db->create_task(task->get_id(), instance_id, task->get_name());
        task_instance_data = _task_instance_db->get_task(instance_id);

        tm start_time = date;
        start_time.tm_year = date.tm_year;
        start_time.tm_mday = date.tm_mday;
        start_time.tm_mon = date.tm_mon;
        start_time.tm_hour = task->get_scheduled_start_time().hours.count();
        start_time.tm_min = task->get_scheduled_start_time().minutes.count();

        task_instance_data->scheduled_start = mktime(&start_time);
        _task_instance_db->update_task(task_instance_data.get());
    }
    task_instance_data = _task_instance_db->get_task(instance_id);
    auto task_instance = std::make_unique<TaskInstance>(std::move(task_instance_data), _task_instance_db.get());
    _task_instances.insert({instance_id, std::move(task_instance)});
}

void TaskTracker::_load_tasks()
{
    _task_data.clear();
    _tasks.clear();

    _task_data = std::move(_task_db->get_tasks());

    for (const auto& task_data : _task_data) {
        _tasks.push_back(std::make_unique<Task>(task_data.get(), _task_db.get()));
    }
}

}