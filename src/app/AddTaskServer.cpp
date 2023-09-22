#include <QDebug>
#include <QHttpServerResponse>
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
              QJsonObject obj{ { "error", "bad request" } };
              return QHttpServerResponse(
                obj, QHttpServerResponse::StatusCode::BadRequest);
          }
          return this->parseRequest(json.object(), TaskUpdateOperation::Delete);
      });
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
    switch (op)
    {
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
    Q_UNUSED(request)
}

void
TaskServer::deleteTask(const TaskJSONRequest& request)
{
    Q_UNUSED(request)
}
