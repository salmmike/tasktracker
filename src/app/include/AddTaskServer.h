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
    QHttpServerResponse getTasks();
};

#endif /* ADDTASKSERVER_H */
