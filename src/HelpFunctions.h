#pragma once

#include "Connection.h"
#include "TypeDefinitions.h"

#include <QString>
#include <QSerialPortInfo>

namespace Ssmr
{

/**
 * @brief GetTooltipForSerialPortName
 * @param serialPortName
 * @return The tooltip for the requested serialport
 *
 * This method enumerates all available serial ports and generates the tooltip through GetTooltipForSerialPortInfo
 */
extern QString GetTooltipForSerialPortName(const QString &serialPortName);

/**
 * @brief GetTooltipForSerialPortInfo
 * @param serialPortInfo
 * @return
 */
extern QString GetTooltipForSerialPortInfo(const QSerialPortInfo &serialPortInfo);

/**
 * @brief CheckConnectionsForSerialPortName
 * @param connections
 * @param serialPortName
 * @param existingConnectionName Optional pointer to store the connection which uses the serial port
 * @return rue if the provided list of connections contains the provided serial port
 */
extern bool CheckConnectionsForSerialPortName(const QList<ConnectionPtr> &connections,
																							const QString &serialPortName,
																							QString* existingConnectionName = nullptr);

/**
 * @brief GetSerialPortInfoByPortName
 * @param portName
 * @return
 */
extern QSerialPortInfo GetSerialPortInfoByPortName(const QString &portName);

/**
 * @brief ParseCommunicationProtocolFromString
 * @param protocol
 * @return
 */
extern CommunicationProtocol
ParseCommunicationProtocolFromString(const QString &protocol,
																		 const Qt::CaseSensitivity &sensitivity = Qt::CaseInsensitive);

/**
 * @brief GetCommunicationProtocolDescriptionsMapping
 * @return A mapping from command protocols to translatable descriptions
 */
extern QMap<CommunicationProtocol, std::function<QString()>>
GetCommunicationProtocolDescriptionsMapping();

/**
 * @brief ParseStringFromCommunicationProtocol
 * @param protocol
 * @return
 */
extern QString ParseStringFromCommunicationProtocol(const CommunicationProtocol &protocol);

}
