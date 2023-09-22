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
    void refresh();

  signals:
    void finishedChanged(int index);

  private:
    tasktracker::TaskTracker* m_tracker;

    QList<tasktracker::TaskInstance*> m_active_task_instance_list;

    time_t m_date;
};

#endif /* TASKLIST_H */
