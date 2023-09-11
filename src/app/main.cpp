#include <iostream>
#include <tasktracklib.h>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDir>

#include "include/TaskListModel.h"

static void add_test_tasks(tasktracker::TaskTracker* tracker)
{
    tracker->clear();
    auto now = std::chrono::system_clock::now();
    auto today = std::chrono::system_clock::to_time_t(now);
    std::cout << "Start creating tasks\n";

    for (int i = 0; i < 10; ++i) {
        std::stringstream ss;

        if (i % 2) {
            today += 60;
            ss << "a";
        } else {
            ss << "b";
        }
        ss << "test task " << i;
        tracker->add_task(ss.str(), tasktracker::RepeatType::NoRepeat, 0, today);
    }

    std::cout << "Tasks created.\n";
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    std::filesystem::path db_path = QDir().homePath().toStdString() + "/.tasktracker/" + "tasks.db";
    QDir().mkdir(QDir().homePath() + "/.tasktracker/");
    tasktracker::TaskTracker tracker(db_path);

    //add_test_tasks(&tracker);

    TaskListModel *taskListModel = new TaskListModel(&tracker, &app);
    qmlRegisterSingletonInstance("com.tasktracker.TaskListModel", 1, 0, "TaskListModel", taskListModel);

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/tasktrackerqml/qml/Main.qml"_qs);

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](const QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
