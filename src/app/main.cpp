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

#include <iostream>
#include <tasktracklib.h>

#include <QDir>
#include <QGuiApplication>
#include <QNetworkInterface>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <simpleini.h>

#include "include/AddTaskServer.h"
#include "include/TaskListModel.h"
#include "include/TopOptions.h"

const std::string confpath{ "/etc/tasktracker/tasktracker.ini" };

static void
add_test_tasks(tasktracker::TaskTracker* tracker)
{
    tracker->clear();
    auto now = std::chrono::system_clock::now();
    auto today = std::chrono::system_clock::to_time_t(now);

    for (int i = 1; i < 11; ++i) {
        std::stringstream ss;

        if (i % 2) {
            today += 60;
            ss << "a";
        } else {
            ss << "b";
        }
        ss << "test task " << i;
        tracker->add_task(
          ss.str(), tasktracker::RepeatType::NoRepeat, 0, today);
    }
}

unsigned
get_server_port(const simpleini::SimpleINI& config)
{
    unsigned port = 0;

    try {
        port = config["tasktrackerapi"].get_as<unsigned>("port");
    } catch (...) {
        qDebug()
          << "Value for port not found in config. Using default value 8181";
        port = 8181;
    }
    return port;
}

unsigned
get_webui_port(const simpleini::SimpleINI& config)
{
    unsigned port = 0;

    try {
        port = config["webui"].get_as<unsigned>("port");
    } catch (...) {
        qDebug()
          << "Value for port not found in config. Using default value 8181";
        port = 5000;
    }
    return port;
}

simpleini::SimpleINI
get_config(const std::string& conf_path)
{
    simpleini::SimpleINI config;
    try {
        config.set_config_file(conf_path);

    } catch (...) {
        qDebug() << "Configuration not found from" << conf_path;
    }
    return config;
}

int
main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    std::filesystem::path db_path =
      QDir().homePath().toStdString() + "/.tasktracker/" + "tasks.db";
    QDir().mkdir(QDir().homePath() + "/.tasktracker/");
    tasktracker::TaskTracker tracker(db_path);
    TaskServer* server = new TaskServer(&tracker, &app);

    auto config = get_config(confpath);
    server->start(get_server_port(config));

    add_test_tasks(&tracker);

    TaskListModel* taskListModel = new TaskListModel(&tracker, &app);
    qmlRegisterSingletonInstance(
      "com.tasktracker.TaskListModel", 1, 0, "TaskListModel", taskListModel);

    TopOptions* topOptions = new TopOptions(&app, get_webui_port(config));
    qmlRegisterSingletonInstance(
      "com.tasktracker.TopOptions", 1, 0, "TopOptions", topOptions);

    QObject::connect(server,
                     &TaskServer::dataModified,
                     taskListModel,
                     &TaskListModel::refresh);

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/tasktrackerqml/qml/Main.qml"_qs);

    engine.rootContext()->setContextProperty("TopOptions", topOptions);

    QObject::connect(
      &engine,
      &QQmlApplicationEngine::objectCreated,
      &app,
      [url](const QObject* obj, const QUrl& objUrl) {
          if (!obj && url == objUrl)
              QCoreApplication::exit(-1);
      },
      Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
