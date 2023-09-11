#include "include/TaskListModel.h"
#include <QDebug>

TaskListModel::TaskListModel(tasktracker::TaskTracker* tracker, QObject *parent):
    QAbstractListModel{parent}, m_tracker{tracker}
{
    auto now = std::chrono::system_clock::now();
    m_date = std::chrono::system_clock::to_time_t(now);
    populate();
}

int TaskListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_active_task_instance_list.size();
}

QVariant TaskListModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_active_task_instance_list.size()) {
        return QVariant();
    }

    auto current_task = m_active_task_instance_list.at(index.row());
    std::stringstream time_ss;
    tm* start_tm {};
    time_t start_time_t {};

    switch ((TaskListRole) role)
    {
    case TaskNameRole:
        return QString(current_task->get_name().c_str());
    case TaskStartTimeRole:
        start_time_t = current_task->get_scheduled_time();
        start_tm = localtime(&start_time_t);
        if (start_tm->tm_hour < 10) {
            time_ss << "0";
        }
        time_ss << start_tm->tm_hour << ":";

        if (start_tm->tm_min < 10) {
            time_ss << "0";
        }

        time_ss << start_tm->tm_min;
        return QString(time_ss.str().c_str());
    default:
        return QVariant();
    }
}

void TaskListModel::populate()
{
    const auto tasks = m_tracker->get_task_instances(*localtime(&m_date));
    m_active_task_instance_list.reserve(tasks.size());
    std::copy(tasks.begin(), tasks.end(), std::back_inserter(m_active_task_instance_list));
}

QHash<int, QByteArray> TaskListModel::roleNames() const
{
    QHash<int, QByteArray> result;

    result[TaskNameRole] = "taskName";
    result[TaskStartTimeRole] = "startTime";

    return result;
}