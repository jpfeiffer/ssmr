#include "ConnectionSerializer.h"

#include "Connection.h"
#include "HelpFunctions.h"

namespace Ssmr {

QList<ConnectionPtr> ConnectionSerializer::DeserializeConnections(QSettings &m_Settings)
{
  QList<ConnectionPtr> connections;

  m_Settings.beginGroup("connections");
  const auto connectionNames = m_Settings.childGroups();

  for(const auto &connectionName : connectionNames)
  {
    QList<ObisValueMapping> mappings;

    m_Settings.beginGroup(connectionName);

    qDebug() << "ConnectionSerializer::DeserializeConnections() loading connection=" << connectionName
             << "name:" << m_Settings.value("name").toString()
             << "port:" << m_Settings.value("port").toString()
             << "protocol:" << m_Settings.value("protocol").toString();

    const auto name = m_Settings.value("name").toString();
    const auto portName = m_Settings.value("port").toString();
    const auto protocol = ParseCommunicationProtocolFromString(m_Settings.value("protocol").toString());

    int size = m_Settings.beginReadArray("mappings");
    for (int i = 0; i < size; ++i)
    {
        m_Settings.setArrayIndex(i);

        QString obisNumber = m_Settings.value("obis").toString();
        QString description = m_Settings.value("description").toString();
        QString unit = m_Settings.value("unit").toString();
        Duration interval = Duration::FromString(QString("%1s").arg(m_Settings.value("interval").toULongLong()));

        mappings.append(ObisValueMapping{obisNumber, description, unit, interval});
    }
    m_Settings.endArray();

    const auto connectionData = ConnectionData(name,
                                               portName,
                                               GetSerialPortInfoByPortName(portName),
                                               protocol,
                                               mappings);

    if(false == connectionData.isValid())
    {
      qDebug() << "ConnectionSerializer::DeserializeConnections() failed to load connection=" << connectionName;
      continue;
    }

    connections.append(std::make_shared<Connection>(connectionData));

    m_Settings.endGroup();
  }
  m_Settings.endGroup();

  return connections;
}
//----------------------------------------------------------------------------------------------------------------------

ConnectionSerializer::ConnectionSerializer(const ConnectionPtr &connection)
  : m_Connection(connection)
{

}
//----------------------------------------------------------------------------------------------------------------------

ConnectionSerializer::~ConnectionSerializer()
{

}
//----------------------------------------------------------------------------------------------------------------------

bool ConnectionSerializer::save(QSettings &settings)
{
  if(nullptr == m_Connection) return false;

  const auto data = m_Connection->getConnectionData();

  settings.beginGroup("connections");

  settings.beginGroup(data.name);

  settings.setValue(QString("name"), QVariant::fromValue(data.name));
  settings.setValue(QString("port"), QVariant::fromValue(data.info.portName()));
  settings.setValue(QString("protocol"), QVariant::fromValue(ParseStringFromCommunicationProtocol(data.protocol)));

  settings.beginWriteArray("mappings");
  for (int i = 0; i < data.mappings.size(); ++i)
  {
    settings.setArrayIndex(i);
    settings.setValue("obis", data.mappings.at(i).obisNumber);
    settings.setValue("description", data.mappings.at(i).description);
    settings.setValue("unit", data.mappings.at(i).unit);
    settings.setValue("interval", data.mappings.at(i).interval.toSeconds());
  }
  settings.endArray();

  settings.endGroup();
  settings.endGroup();
  return true;
}
//----------------------------------------------------------------------------------------------------------------------


} // namespace Ssmr
