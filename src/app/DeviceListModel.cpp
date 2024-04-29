#include "DeviceListModel.h"
#include <QDebug>

QList<QString>
unconnected_devices(BoredomScheduler* scheduler)
{
    auto devices = scheduler->list_unconnected_devices();
    QList<QString> device_names(devices.size());

    std::transform(std::begin(devices),
                   std::end(devices),
                   std::begin(device_names),
                   [](const auto& device) { return device.name.c_str(); });
    for (const auto& str : device_names) {
        qDebug() << str << "DEVICES";
    }
    return device_names;
}

DeviceListModel::DeviceListModel(BoredomScheduler* scheduler, QObject* parent)
  : QAbstractListModel{ parent }
  , m_scheduler{ scheduler }
  , m_devices(unconnected_devices(scheduler))
{
}

int
DeviceListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_devices.size();
}

QVariant
DeviceListModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= m_devices.size()) {
        return QVariant();
    }

    switch ((DeviceListRole)role) {
        case DeviceNameRole:
            return m_devices.at(index.row());
        case DeviceAlarmRole:
            return QVariant(isAlarm());
        default:
            return QVariant();
    }
}

void
DeviceListModel::populate()
{
    beginResetModel();
    m_devices = unconnected_devices(m_scheduler);
    endResetModel();
}

QHash<int, QByteArray>
DeviceListModel::roleNames() const
{
    QHash<int, QByteArray> result;
    result[DeviceNameRole] = "deviceName";
    result[DeviceAlarmRole] = "isAlarm";
    return result;
}

void
DeviceListModel::refresh()
{
    populate();
}

bool
DeviceListModel::isAlarm() const
{
    return m_scheduler->is_alarm();
}

void
DeviceListModel::setSnooze(int index)
{
    Q_UNUSED(index);
    m_scheduler->snooze(5);
    refresh();
}
