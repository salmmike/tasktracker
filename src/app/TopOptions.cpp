#include "TopOptions.h"
#include <QNetworkInterface>

TopOptions::TopOptions(QObject* parent,
                       unsigned port,
                       BoredomScheduler* scheduler)
  : QObject(parent)
  , m_port(port)
  , m_scheduler(scheduler)
{
    auto addresses = QNetworkInterface::allAddresses();
    auto host_addr = *std::find_if(
      std::begin(addresses), std::end(addresses), [](QHostAddress& a) {
          return a.protocol() == QAbstractSocket::IPv4Protocol &&
                 a != QHostAddress(QHostAddress::LocalHost);
      });

    m_webaddress = host_addr.toString() + ":" + QString::number(port);
}

QString
TopOptions::webAddress()
{
    return m_webaddress;
}

QList<QString>
TopOptions::unconnected_devices()
{
    auto devices = m_scheduler->list_unconnected_devices();
    QList<QString> device_names;
    for (const auto& device : devices) {
        device_names.push_back(QString(device.name.c_str()));
    }

    return device_names;
}
