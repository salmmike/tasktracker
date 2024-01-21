#include "task.h"

#include <iostream>

static tm
s_get_nth_weekday_of_month(int day, int week, tm schedule_tm)
{
    schedule_tm.tm_mday = 1;
    schedule_tm.tm_hour = 0;

    time_t one_day = 24 * 60 * 60;

    time_t t = mktime(&schedule_tm);

    while (localtime(&t)->tm_wday != day) {
        t += one_day;
    } // t now at first occurance of day.
    t += (week - 1) * 7 * one_day;
    return *localtime(&t);
}

namespace tasktracker {

TaskInstance::TaskInstance(std::unique_ptr<TaskInstanceData>&& data,
                           TaskInstanceDatabase* db)
  : m_data(std::move(data))
  , m_db(db)
{
}

TaskInstance::~TaskInstance() {}

std::string
TaskInstance::get_name() const
{
    return m_data->name;
}

std::string
TaskInstance::get_uid() const
{
    return m_data->id;
}

void
TaskInstance::start_task()
{
    m_data->state = TaskState::Started;
    m_db->update_task(m_data.get());
}
void
TaskInstance::skip_task()
{
    m_data->state = TaskState::Skipped;
    m_db->update_task(m_data.get());
}
void
TaskInstance::finish_task()
{
    m_data->state = TaskState::Finished;
    m_db->update_task(m_data.get());
}

void
TaskInstance::set_undone()
{
    m_data->state = TaskState::NotStarted;
    m_db->update_task(m_data.get());
}

time_t
TaskInstance::get_scheduled_datetime() const
{
    return m_data->scheduled_start;
}

time_t
TaskInstance::get_scheduled_time() const
{
    tm start_time{};
    const time_t start_datetime = get_scheduled_datetime();
    tm start_datetime_tm = *localtime(&start_datetime);

    start_time.tm_year = start_datetime_tm.tm_year;
    start_time.tm_hour = start_datetime_tm.tm_hour;
    start_time.tm_min = start_datetime_tm.tm_min;

    return mktime(&start_time);
}

std::chrono::seconds
TaskInstance::get_time_spent() const
{
    return m_data->time_spent;
}

void
TaskInstance::set_comment(const std::string& str)
{
    m_data->comment = str;
    m_db->update_task(m_data.get());
}

std::string
TaskInstance::get_comment() const
{
    return m_data->comment;
}

const TaskInstanceData*
TaskInstance::get_data() const
{
    return m_data.get();
}

bool
TaskInstance::is_finished() const
{
    return m_data->state == TaskState::Finished;
}

bool
TaskInstance::is_skipped() const
{
    return m_data->state == TaskState::Skipped;
}

bool
TaskInstance::is_started() const
{
    return m_data->state == TaskState::Started;
}

Task::Task(TaskData* data, TaskDatabase* db)
  : m_data(data)
  , m_db(db)
{
}

Task::~Task() {}

ScheduledTime
Task::get_scheduled_start_time()
{
    tm* time_struct = localtime(&m_data->scheduled_start);

    ScheduledTime time;
    time.hours = std::chrono::hours(time_struct->tm_hour);
    time.minutes = std::chrono::minutes(time_struct->tm_min);
    return time;
}

time_t
Task::get_scheduled_start_time_t() const
{
    return m_data->scheduled_start;
}

std::string
Task::get_name() const
{
    return m_data->name;
}

int
Task::get_id() const
{
    return m_data->id;
}

std::string
Task::get_comment()
{
    return m_data->comment;
}

void
Task::set_comment(const std::string& comment)
{
    m_data->comment = comment;
    m_db->update_task(m_data);
}

bool
Task::occurs(std::chrono::year_month_day day)
{
    tm time{};
    time.tm_year = static_cast<int>(day.year()) - 1900;
    time.tm_mon = static_cast<unsigned int>(day.month()) - 1;
    time.tm_mday = static_cast<unsigned int>(day.day());

    time_t t = mktime(&time);
    tm tm = *localtime(&t);

    return occurs(tm);
}

bool
Task::occurs(tm day)
{
    tm schedule_tm = *localtime(&m_data->scheduled_start);
    int repeat_info = m_data->repeat_info;

    switch (m_data->repeat_type) {
        case RepeatType::Monthly:
            return day.tm_mday == repeat_info;

        case RepeatType::MonthlyDay: {
            int day_val = repeat_info % 10 % 7;
            int week_val = (repeat_info / 10) % 10;
            if (day_val != day.tm_wday)
                return false;

            tm correct_day = s_get_nth_weekday_of_month(day_val, week_val, day);
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
            time_t start_day = m_data->scheduled_start;
            tm start_day_tm = *localtime(&start_day);
            start_day_tm.tm_hour = 0;
            start_day_tm.tm_min = 0;
            start_day_tm.tm_sec = 0;
            start_day = mktime(&start_day_tm);

            time_t test_day = mktime(&day);
            long day_length = 24 * 60 * 60;

            long val = (static_cast<long>(std::difftime(test_day, start_day)) /
                        day_length);
            return (val % repeat_info == 0);
    }
    return false;
}

} // namespace tasktracker
