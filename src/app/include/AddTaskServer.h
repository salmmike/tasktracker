#ifndef ADDTASKSERVER_H
#define ADDTASKSERVER_H

#include <memory>

#include <QHttpServer>
#include <QHttpServerResponse>
#include <QJsonObject>

#include <tasktracklib.h>

#ifndef TASKSERVER_PORT
#define TASKSERVER_PORT 8181
#endif

#define TASK_UPDATE_PATH "/task"

struct TaskJSONRequest
{
    int id;
    QString name;
    time_t start_time;
    int repeat_info;
    int repeat_type;
};

enum TaskUpdateOperation
{
    Create,
    Update,
    Delete
};

class TaskServer : public QObject
{
    Q_OBJECT

  public:
    explicit TaskServer(tasktracker::TaskTracker* tracker,
                        QObject* parent = nullptr);
    ~TaskServer();
    void start();

  signals:
    void dataModified();

  private:
    tasktracker::TaskTracker* m_tracker;
    std::unique_ptr<QHttpServer> m_server;
    QHttpServerResponse parseRequest(const QJsonObject& obj,
                                     TaskUpdateOperation op);
    std::pair<bool, TaskJSONRequest> checkJsonFields(const QJsonObject& obj,
                                                     TaskUpdateOperation op);
    void addTask(const TaskJSONRequest& request);
    void modifyTask(const TaskJSONRequest& request);
    void deleteTask(const TaskJSONRequest& request);
};

#endif /* ADDTASKSERVER_H */
