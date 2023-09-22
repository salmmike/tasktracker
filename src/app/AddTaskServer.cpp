#include <QDebug>
#include <QHttpServerResponse>
#include <QJsonObject>
#include <QJsonParseError>

#include "include/AddTaskServer.h"

TaskServer::~TaskServer() {}

void
TaskServer::start()
{
    m_server->listen(QHostAddress::Any, TASKSERVER_PORT);
}

QHttpServerResponse
TaskServer::parseRequest(const QJsonObject& obj, TaskUpdateOperation op)
{
    qDebug() << obj;
    auto res = checkJsonFields(obj, op);

    if (!res.first) {
        return QHttpServerResponse(
          QJsonObject{ { "error", "Request fields incorrect." } },
          QHttpServerResponse::StatusCode::BadRequest);
    }

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

TaskServer::TaskServer(const tasktracker::TaskTracker* tracker, QObject* parent)
  : QObject(parent)
  , m_tracker(tracker)
  , m_server(std::make_unique<QHttpServer>())
{
    m_server->route(
      "/test",
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
}
