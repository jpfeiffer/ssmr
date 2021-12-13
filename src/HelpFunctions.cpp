#include "HelpFunctions.h"

#include <QTextStream>
#include <QMap>

namespace Ssmr
{

namespace
{
  const QMap<CommunicationProtocol, QString> cCommunicationProtocolMapping =
    {{CommunicationProtocol::eUnknown, {""}},
     {CommunicationProtocol::eDSSInformation, {"dss"}},
     {CommunicationProtocol::eD0Dialog, {"d0"}}};

  const QMap<CommunicationProtocol, std::function<QString()>> cCommunicationProtocolDescriptionsMapping =
    {{CommunicationProtocol::eUnknown, []() { return QObject::tr("unknown"); }},
     {CommunicationProtocol::eDSSInformation, []() { return QObject::tr("DSS-Information (SML)"); }},
     {CommunicationProtocol::eD0Dialog, []() { return QObject::tr("D0-Dialog (IEC 62056-21)"); }}};
}

QString GetTooltipForSerialPortName(const QString &serialPortName)
{
  for(const auto &serialPortInfo : QSerialPortInfo::availablePorts())
  {
    if(serialPortName == serialPortInfo.portName())
    {
      return GetTooltipForSerialPortInfo(serialPortInfo);
    }
  }

  return {};
}
//----------------------------------------------------------------------------------------------------------------------

QString GetTooltipForSerialPortInfo(const QSerialPortInfo &serialPortInfo)
{
  const QString blankString = "N/A";
  QString description;
  QString manufacturer;
  QString serialNumber;

  QString tooltip;
  QTextStream out(&tooltip);

  description = serialPortInfo.description();
  manufacturer = serialPortInfo.manufacturer();
  serialNumber = serialPortInfo.serialNumber();

  out << QObject::tr("Port: ") << serialPortInfo.portName() << Qt::endl
      << QObject::tr("Location: ") << serialPortInfo.systemLocation() << Qt::endl
      << QObject::tr("Description: ") << (!description.isEmpty() ? description : blankString) << Qt::endl
      << QObject::tr("Manufacturer: ") << (!manufacturer.isEmpty() ? manufacturer : blankString) << Qt::endl
      << QObject::tr("Serial number: ") << (!serialNumber.isEmpty() ? serialNumber : blankString) << Qt::endl
      << QObject::tr("Vendor Identifier: ") << (serialPortInfo.hasVendorIdentifier()
                                                ? QByteArray::number(serialPortInfo.vendorIdentifier(), 16)
                                                : blankString) << Qt::endl
      << QObject::tr("Product Identifier: ") << (serialPortInfo.hasProductIdentifier()
                                                 ? QByteArray::number(serialPortInfo.productIdentifier(), 16)
                                                 : blankString);

  return tooltip;
}
//----------------------------------------------------------------------------------------------------------------------

bool CheckConnectionsForSerialPortName(const QList<ConnectionPtr> &connections,
                                       const QString &serialPortName,
                                       QString* existingConnectionName)
{
  for(const auto &connection : connections)
  {
    if(connection->getSerialPortInfo().portName() == serialPortName)
    {
      if(nullptr != existingConnectionName) *existingConnectionName = connection->getName();
      return true;
    }
  }

  return false;
}
//----------------------------------------------------------------------------------------------------------------------

QSerialPortInfo GetSerialPortInfoByPortName(const QString &portName)
{
  for(const auto &serialPortInfo : QSerialPortInfo::availablePorts())
  {
    if(portName == serialPortInfo.portName()) return serialPortInfo;
  }

  return {};
}
//----------------------------------------------------------------------------------------------------------------------

CommunicationProtocol ParseCommunicationProtocolFromString(const QString &protocol,
                                                           const Qt::CaseSensitivity &sensitivity)
{
  return cCommunicationProtocolMapping.key((Qt::CaseInsensitive == sensitivity) ? protocol.toLower() : protocol,
                                           CommunicationProtocol::eUnknown);
}
//----------------------------------------------------------------------------------------------------------------------

QString ParseStringFromCommunicationProtocol(const CommunicationProtocol &protocol)
{
  return cCommunicationProtocolMapping.value(protocol, QString());
}
//----------------------------------------------------------------------------------------------------------------------

QMap<CommunicationProtocol, std::function<QString ()>> GetCommunicationProtocolDescriptionsMapping()
{
  return cCommunicationProtocolDescriptionsMapping;
}
//----------------------------------------------------------------------------------------------------------------------

}
