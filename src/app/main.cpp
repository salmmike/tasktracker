#include <iostream>
#include <tasktracklib.h>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDir>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    std::filesystem::path db_path = QDir().homePath().toStdString() + "/.tasktracker/" + "tasks.db";
    QDir().mkdir(QDir().homePath() + "/.tasktracker/");
    tasktracker::TaskTracker tracker(db_path);

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:tasktrackerqml/qml/Main.qml"_qs);

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](const QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
