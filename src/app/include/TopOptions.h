#ifndef TOPOPTIONS_H
#define TOPOPTIONS_H

#include <QObject>
#include <QString>

class TopOptions : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString webAddress READ webAddress NOTIFY webAddressChanged)
  public:
    explicit TopOptions(QObject* parent, unsigned port);
    QString webAddress();
  signals:
    void webAddressChanged();

  private:
    QString m_webaddress;
    unsigned int m_port;
};

#endif /* TOPOPTIONS_H */
