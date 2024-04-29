#ifndef DEVICELISTMODEL_H
#define DEVICELISTMODEL_H
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 *
 * Author: Mike Salmela
 */

#include <QAbstractListModel>
#include <QDate>
#include <QTimer>
#include <scheduler.h>

class QuickNotify : public QObject
{
    Q_OBJECT
  public:
    explicit QuickNotify(QObject* parent = nullptr)
      : QObject(parent){};
    void notify_slot() { emit notify(); };

  signals:
    void notify();
};

enum DeviceListRole
{
    DeviceNameRole = Qt::UserRole + 1,
    DeviceAlarmRole,
};

class DeviceListModel : public QAbstractListModel
{
    Q_OBJECT

  public:
    explicit DeviceListModel(BoredomScheduler* scheduler,
                             QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role) const override;

    void populate();

    QHash<int, QByteArray> roleNames() const override;

  public slots:
    ///@brief Snooze device.
    void setSnooze(int index);
    void refresh();
    bool isAlarm() const;

  signals:
    void deviceChanged(QString newDate);

  private:
    BoredomScheduler* m_scheduler;
    QList<QString> m_devices;
};

#endif /* DEVICELISTMODEL_H */
