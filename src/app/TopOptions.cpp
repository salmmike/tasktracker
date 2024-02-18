#include "TopOptions.h"
#include <QNetworkInterface>

TopOptions::TopOptions(QObject* parent, unsigned port)
  : QObject(parent)
  , m_port(port)
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