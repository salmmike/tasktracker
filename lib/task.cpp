#include "task.h"
#include <iostream>

static tm s_get_nth_weekday_of_month(int day, int week, tm schedule_tm)
{
    schedule_tm.tm_mday = 1;
    schedule_tm.tm_hour = 0;

    time_t one_day = 24 * 60 * 60;

    time_t t = mktime(&schedule_tm);

    while(localtime(&t)->tm_wday != day) {
        t += one_day;
    } // t now at first occurance of day.
    t += (week - 1) * 7 * one_day;
    return *localtime(&t);
}

namespace tasktracker {

TaskInstance::TaskInstance(std::unique_ptr<TaskInstanceData> &&data, TaskInstanceDatabase *db):
    _data(std::move(data)), _db(db)
{
}

TaskInstance::~TaskInstance()
{
}

std::string TaskInstance::get_name()
{
    return _data->name;
}

std::string TaskInstance::get_uid()
{
    return _data->id;
}

void
TaskInstance::start_task()
{
    _data->state = TaskState::Started;
    _db->update_task(_data.get());
}
void
TaskInstance::skip_task()
{
    _data->state = TaskState::Skipped;
    _db->update_task(_data.get());
}
void
TaskInstance::finish_task()
{
    _data->state = TaskState::Finished;
    _db->update_task(_data.get());
}

time_t
TaskInstance::get_scheduled_time()
{
    return _data->scheduled_start;
}

std::chrono::seconds
TaskInstance::get_time_spent()
{
    return _data->time_spent;
}

void
TaskInstance::set_comment(const std::string &str)
{
    _data->comment = str;
    _db->update_task(_data.get());
}

std::string
TaskInstance::get_comment()
{
    return _data->comment;
}

const TaskInstanceData*
TaskInstance::get_data()
{
    return _data.get();
}

Task::Task(TaskData *data, TaskDatabase *db):
    _data(data), _db(db)
{
}

Task::~Task()
{
}

ScheduledTime Task::get_scheduled_start_time()
{
    tm *time_struct = localtime(&_data->scheduled_start);

    ScheduledTime time;
    time.hours = std::chrono::hours(time_struct->tm_hour);
    time.minutes = std::chrono::minutes(time_struct->tm_min);
    return time;
}

std::string Task::get_name()
{
    return _data->name;
}

int Task::get_id()
{
    return _data->id;
}

std::string Task::get_comment()
{
    return _data->comment;
}

bool Task::occurs(std::chrono::year_month_day day)
{
    tm time {};
    time.tm_year = static_cast<int>(day.year()) - 1900;
    time.tm_mon = static_cast<unsigned int>(day.month()) - 1;
    time.tm_mday = static_cast<unsigned int>(day.day());

    time_t t = mktime(&time);
    tm tm = *localtime(&t);

    return occurs(tm);
}

bool Task::occurs(tm day)
{
    tm schedule_tm = *localtime(&_data->scheduled_start);
    int repeat_info = _data->repeat_info;

    switch (_data->repeat_type)
    {
        case RepeatType::Monthly:
            return day.tm_mday == repeat_info;

        case RepeatType::MonthlyDay: {
            int day_val = repeat_info % 10 % 7;
            int week_val = (repeat_info / 10) % 10;
            if (day_val != day.tm_wday) return false;

            tm correct_day  = s_get_nth_weekday_of_month(day_val, week_val, day);
            return day.tm_mday == correct_day.tm_mday;
        }

        case RepeatType::NoRepeat: {
            return schedule_tm.tm_year == day.tm_year &&
                schedule_tm.tm_mon == day.tm_mon &&
                schedule_tm.tm_mday == day.tm_mday;
        }

        case RepeatType::SpecifiedDays: {
            while (repeat_info) {
                int info_day = repeat_info % 10;
                if ((day.tm_wday) == info_day ||
                    (info_day == 7 && day.tm_wday == 0)) {
                    return true;
                }
                repeat_info /= 10;
            }
            return false;
        }
        case RepeatType::WithInterval:
            time_t start_day = _data->scheduled_start;
            tm start_day_tm = *localtime(&start_day);
            start_day_tm.tm_hour = 0;
            start_day_tm.tm_min = 0;
            start_day_tm.tm_sec = 0;
            start_day = mktime(&start_day_tm);

            time_t test_day = mktime(&day);
            long day_length = 24*60*60;

            long val = (static_cast<long>(std::difftime(test_day, start_day)) / day_length);
            return (val % repeat_info == 0);

    }
    return false;
}

}
