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
#include <scheduler.h>
#include <tasktracklib.h>
#include <time.h>

#include <QDir>
#include <QGuiApplication>
#include <QNetworkInterface>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>
#include <simpleini.h>

#include "include/AddTaskServer.h"
#include "include/DeviceListModel.h"
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
        qDebug() << "Creating task in time:" << ctime(&today);

        if (i % 2) {
            today += 60;
            ss << "a";
        } else {
            ss << "b";
        }
        ss << "test task " << i;
        tracker->add_task(
          ss.str(), tasktracker::RepeatType::WithInterval, 2, today);
    }
}

unsigned
get_api_port(const simpleini::SimpleINI& config)
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
          << "Value for port not found in config. Using default value 5000";
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

bool
create_test_tasks_set(int argc, char* argv[])
{
    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "add-debug-tasks") == 0) {
            return true;
        }
    }
    return false;
}

std::unique_ptr<BoredomScheduler>
make_boredom_scheduler(const simpleini::SimpleINI& config)
{
    auto scheduler = std::make_unique<BoredomScheduler>();
    try {
        auto section = config["Boredom scheduler"];
        scheduler->set_config_file(section["configpath"]);
    } catch (std::out_of_range& err) {
        qDebug() << "No 'Boredom scheduler' configuration.";
    }
    return scheduler;
}

void
refresh_device_list(void* data)
{
    auto object = static_cast<QuickNotify*>(data);
    object->notify_slot();
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
    QuickNotify* notifyer = new QuickNotify(&app);
    QTimer* notify_timer = new QTimer(&app);

    notify_timer->setSingleShot(true);

    auto config = get_config(confpath);
    auto scheduler = make_boredom_scheduler(config);

    server->start(get_api_port(config));
    if (create_test_tasks_set(argc, argv)) {
        add_test_tasks(&tracker);
    }

    TaskListModel* taskListModel = new TaskListModel(&tracker, &app);
    qmlRegisterSingletonInstance(
      "com.tasktracker.TaskListModel", 1, 0, "TaskListModel", taskListModel);

    TopOptions* topOptions =
      new TopOptions(&app, get_webui_port(config), scheduler.get());
    qmlRegisterSingletonInstance(
      "com.tasktracker.TopOptions", 1, 0, "TopOptions", topOptions);

    DeviceListModel* deviceListModel =
      new DeviceListModel(scheduler.get(), &app);

    qmlRegisterSingletonInstance("com.tasktracker.DeviceListModel",
                                 1,
                                 0,
                                 "DeviceListModel",
                                 deviceListModel);

    QObject::connect(server,
                     &TaskServer::dataModified,
                     taskListModel,
                     &TaskListModel::refresh);

    scheduler->set_device_event_cb(refresh_device_list);
    scheduler->set_event_cb_data(notifyer);

    QObject::connect(notifyer,
                     &QuickNotify::notify,
                     notify_timer,
                     [notify_timer]() { notify_timer->start(200); });

    QObject::connect(notify_timer,
                     &QTimer::timeout,
                     deviceListModel,
                     &DeviceListModel::refresh);

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
