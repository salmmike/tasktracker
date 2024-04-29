#ifndef TOPOPTIONS_H
#define TOPOPTIONS_H

#include <QList>
#include <QObject>
#include <QPair>
#include <QString>
#include <scheduler.h>

class TopOptions : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString webAddress READ webAddress NOTIFY webAddressChanged)

  public:
    explicit TopOptions(QObject* parent,
                        unsigned port,
                        BoredomScheduler* scheduler);
    QString webAddress();
    QList<QString> unconnected_devices();

  signals:
    void webAddressChanged();

  private:
    QString m_webaddress;
    unsigned int m_port;
    BoredomScheduler* m_scheduler;
};

#endif /* TOPOPTIONS_H */
