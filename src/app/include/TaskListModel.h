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

#ifndef TASKLIST_H
#define TASKLIST_H

#include <QAbstractListModel>
#include <tasktracklib.h>

enum TaskListRole
{
    TaskNameRole = Qt::UserRole + 1,
    TaskStartTimeRole,
    TaskFinishedRole,
    TaskSkippedRole,
    TaskStartedRole
};

class TaskListModel : public QAbstractListModel
{
    Q_OBJECT

  public:
    explicit TaskListModel(tasktracker::TaskTracker* tracker,
                           QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role) const override;

    void populate();

    QHash<int, QByteArray> roleNames() const override;

  public slots:
    void setFinished(int index);
    void setUndone(int index);
    void setSkipped(int index);
    void removeTask(int index);
    void refresh();
    void nextDay();
    void previousDay();

  signals:
    void finishedChanged(int index);
    void addressChanged();

  private:
    tasktracker::TaskTracker* m_tracker;

    QList<tasktracker::TaskInstance*> m_active_task_instance_list;

    time_t m_date;
};

#endif /* TASKLIST_H */
