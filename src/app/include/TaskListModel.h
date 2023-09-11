#ifndef TASKLIST_H
#define TASKLIST_H

#include <QAbstractListModel>
#include <tasktracklib.h>

enum TaskListRole {
    TaskNameRole = Qt::UserRole + 1,
    TaskStartTimeRole
};

class TaskListModel: public QAbstractListModel {
    Q_OBJECT

public:
    explicit TaskListModel(tasktracker::TaskTracker* tracker, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    void populate();

    QHash<int, QByteArray> roleNames() const override;

private:
    tasktracker::TaskTracker* m_tracker;

    QList<tasktracker::TaskInstance*> m_active_task_instance_list;

    time_t m_date;
};

#endif /* TASKLIST_H */
