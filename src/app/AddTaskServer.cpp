#include <QDebug>
#include <QHttpServerResponse>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>

#include "include/AddTaskServer.h"

TaskServer::TaskServer(tasktracker::TaskTracker* tracker, QObject* parent)
  : QObject(parent)
  , m_tracker(tracker)
  , m_server(std::make_unique<QHttpServer>())
{
    m_server->route(
      TASK_UPDATE_PATH,
      QHttpServerRequest::Method::Post,
      [this](const QHttpServerRequest& request) {
          QJsonParseError err;
          const auto json = QJsonDocument::fromJson(request.body(), &err);
          if (err.error != QJsonParseError::NoError || !json.isObject()) {
              QJsonObject obj{ { "error", "bad request" } };
              return QHttpServerResponse(
                obj, QHttpServerResponse::StatusCode::BadRequest);
          }
          return this->parseRequest(json.object(), TaskUpdateOperation::Create);
      });

    m_server->route(
      TASK_UPDATE_PATH,
      QHttpServerRequest::Method::Patch,
      [this](const QHttpServerRequest& request) {
          QJsonParseError err;
          const auto json = QJsonDocument::fromJson(request.body(), &err);
          if (err.error != QJsonParseError::NoError || !json.isObject()) {
              QJsonObject obj{ { "error", "bad request" } };
              return QHttpServerResponse(
                obj, QHttpServerResponse::StatusCode::BadRequest);
          }
          return this->parseRequest(json.object(), TaskUpdateOperation::Update);
      });

    m_server->route(
      TASK_UPDATE_PATH,
      QHttpServerRequest::Method::Delete,
      [this](const QHttpServerRequest& request) {
          QJsonParseError err;
          const auto json = QJsonDocument::fromJson(request.body(), &err);
          if (err.error != QJsonParseError::NoError || !json.isObject()) {
              QJsonObject obj{ { "error", "Bad request body." } };
              return QHttpServerResponse(
                obj, QHttpServerResponse::StatusCode::BadRequest);
          }
          return this->parseRequest(json.object(), TaskUpdateOperation::Delete);
      });

    m_server->route(TASK_UPDATE_PATH,
                    QHttpServerRequest::Method::Get,
                    [this]() { return this->getTasks(); });
}

TaskServer::~TaskServer() {}

void
TaskServer::start()
{
    m_server->listen(QHostAddress::Any, TASKSERVER_PORT);
}

QHttpServerResponse
TaskServer::parseRequest(const QJsonObject& obj, TaskUpdateOperation op)
{
    qDebug() << "Request in!";
    qDebug() << obj;
    auto res = checkJsonFields(obj, op);

    if (!res.first) {
        return QHttpServerResponse(
          QJsonObject{ { "error", "Request fields incorrect." } },
          QHttpServerResponse::StatusCode::BadRequest);
    }
    switch (op) {
        case TaskUpdateOperation::Create:
            addTask(res.second);
            break;
        case TaskUpdateOperation::Update:
            modifyTask(res.second);
            break;
        case TaskUpdateOperation::Delete:
            deleteTask(res.second);
            break;
    }
    emit dataModified();

    return QHttpServerResponse(QJsonObject{ { "error", 0 } },
                               QHttpServerResponse::StatusCode::Ok);
}

std::pair<bool, TaskJSONRequest>
TaskServer::checkJsonFields(const QJsonObject& obj, TaskUpdateOperation op)
{
    TaskJSONRequest requestData;

    if (op == TaskUpdateOperation::Create) {
        const auto valit = obj.constFind("taskName");

        if (valit == obj.constEnd() || !valit.value().isString()) {
            return { false, {} };
        }
        qDebug() << valit.value().toString();
        requestData.name = valit.value().toString();
    }
    if (op == TaskUpdateOperation::Delete ||
        op == TaskUpdateOperation::Update) {
        const auto valit = obj.constFind("taskID");

        if (valit == obj.constEnd() || !valit.value().isDouble()) {
            qDebug() << "Fault in taskID";
            return { false, {} };
        }

        qDebug() << valit.value().toInteger();
        requestData.id = valit.value().toInteger();
    }

    if (op != TaskUpdateOperation::Delete) {
        for (const auto str :
             { "taskStart", "taskRepeatInfo", "taskRepeatType" }) {
            const auto valit = obj.constFind(str);

            if (valit == obj.constEnd() || !valit.value().isDouble()) {
                qDebug() << "Fault in" << str;
                return { false, {} };
            }
            qDebug() << valit.value().toInteger();
            if (!strcmp(str, "taskStart")) {
                requestData.start_time = valit.value().toInteger();
            } else if (!strcmp(str, "taskRepeatType")) {
                requestData.repeat_type = valit.value().toInteger();
            } else {
                requestData.repeat_info = valit.value().toInteger();
            }
        }
    }

    return { true, requestData };
}

void
TaskServer::addTask(const TaskJSONRequest& request)
{
    std::string name = request.name.toStdString();
    m_tracker->add_task(name,
                        (tasktracker::RepeatType)request.repeat_type,
                        request.repeat_info,
                        (time_t)request.start_time);
}

void
TaskServer::modifyTask(const TaskJSONRequest& request)
{
    tasktracker::Task* task = m_tracker->get_task(request.id);
    auto data = task->get_data();
    data->name =
      !request.name.isEmpty() ? request.name.toStdString() : data->name;
    data->repeat_info =
      request.repeat_info > 0 ? request.repeat_info : data->repeat_info;
    data->repeat_type =
      request.repeat_type >= 0
        ? static_cast<tasktracker::RepeatType>(request.repeat_type)
        : data->repeat_type;
    data->scheduled_start =
      request.start_time >= 0 ? request.start_time : data->scheduled_start;

    m_tracker->modify_task(data);
}

void
TaskServer::deleteTask(const TaskJSONRequest& request)
{
    m_tracker->delete_task(request.id);
}

QHttpServerResponse
TaskServer::getTasks()
{
    QVariantList resp;

    for (const tasktracker::Task* task : m_tracker->get_tasks()) {
        QVariantMap map = {
            { "taskName", QString::fromStdString(task->get_name()) },
            { "taskID", task->get_id() },
            { "taskStart",
              static_cast<qint64>(task->get_scheduled_start_time_t()) },
        };
        resp.append(map);
    }

    return { QJsonArray::fromVariantList(resp) };
}
