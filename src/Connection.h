#pragma once

#include <memory>

#include <QList>
#include <QDebug>
#include <QTimer>
#include <QObject>
#include <QVariant>
#include <QElapsedTimer>
#include <QSerialPortInfo>

#include "TypeDefinitions.h"

namespace Ssmr
{

class Connection;
typedef std::shared_ptr<Connection> ConnectionPtr;
typedef std::weak_ptr<Connection> ConnectionWeakPtr;

/**
 * @brief The Connection class Represents a single connection to a device
 */
class Connection : public QObject
{

	Q_OBJECT

public:

	/**
	 * @brief Connection Connection data constructor
	 * @param data
	 */
	Connection(const ConnectionData &data, QObject* parent = nullptr);

	/**
	 * @brief ~Connection Default destructor
	 */
	virtual ~Connection() override;

	/**
	 * @brief setConnectionData Set a new connection data, this will disconnect the connection if connected
	 * @param data
	 */
	void setConnectionData(const ConnectionData &data);

	/**
	 * @brief getConnectionData
	 * @return The connection information
	 */
	ConnectionData getConnectionData() const;

	/**
	 * @brief isValid
	 * @return True if this connection is valid and can be used to establish a connection
	 */
	bool isValid() const;

	/**
	 * @brief getName
	 * @return The connection name
	 */
	QString getName() const;

	/**
	 * @brief getSerialPortName
	 * @return The configured serial port name
	 */
	QString getSerialPortName() const;

	/**
	 * @brief getSerialPortInfo
	 * @return The contained serial port information structur
	 */
	QSerialPortInfo getSerialPortInfo() const;

	/**
	 * @brief getProtocol
	 * @return Which protocol is used
	 */
	CommunicationProtocol getProtocol() const;

	/**
	 * @brief getSerialPort
	 * @return Access to the serial port instance for this connection
	 */
	QSerialPort* getSerialPort();

	/**
	 * @brief getSerialPort
	 * @return Access to the serial port instance for this connection
	 */
	const QSerialPort* getSerialPort() const;

	/**
	 * @brief isConnected
	 * @return True if already connected
	 */
	bool isConnected() const;

	/**
	 * @brief connect
	 * @return True if connection could be established
	 */
	bool connect();

	/**
	 * @brief disconnect Close any connection if established
	 */
	void disconnect();

signals:

	/**
	 * @brief connectionChanged Emitted when the connection state changed
	 * @param connected
	 */
	void connectionChanged(bool connected);

	/**
	 * @brief connectionTimeChanged Emitted with the new connection time in milliseconds
	 * @param elapsedMilliseconds
	 */
	void connectionTimeChanged(qint64 elapsedMilliseconds);

	/**
	 * @brief dataValueReceived Emitted when a new data value for a given obis number is received
	 * @param obisValue The obis number received
	 * @param timestamp Time in milliseconds since epoc
	 * @param dataValue The new data value according to the data type received (supported are bool, hex string, double)
	 */
	void dataValueReceived(const QString &obisValue, const qint64 &timestamp, const QVariant &dataValue);

	/**
	 * @brief mappingAdded A new mapping added for this connection
	 * @param mapping
	 */
	void mappingAdded(const ObisValueMapping &mapping);

	/**
	 * @brief mappingRemoved A mapping has been removed for this connection
	 * @param mapping
	 */
	void mappingRemoved(const ObisValueMapping &mapping);

private slots:

	/**
	 * @brief onConnectionUpdate Handle connection timer updates
	 */
	void onConnectionUpdate();

	/**
	 * @brief onDataReceived Called with new serial data to be processed
	 */
	void onDataReceived();

private:

	struct ObisValueList
	{
		QList<QPair<qint64, QVariant>> values;

		void append(qint64 t, QVariant v)
		{
			values.append(qMakePair(t,v));
		}

		bool toSamples(QList<QPair<qint64, double>> &samples)
		{
			if(false == values.isEmpty()) return false;
			if(false == values.first().second.canConvert<double>()) return false;

			for(const QPair<qint64, QVariant> &value : values)
			{
				samples.append(qMakePair(value.first, value.second.toDouble()));
			}

			return true;
		}
	};

	/**
	 * @brief parseSmlData Method to parse received sml data
	 * @param message
	 * @return
	 */
	int parseSmlData(const QByteArray &message);

	/**
	 * @brief m_ConnectionData The connection information
	 */
	ConnectionData m_ConnectionData;

	/**
	 * @brief m_ConnectionDuration The total connection duration
	 */
	QElapsedTimer m_ConnectionDuration;

	/**
	 * @brief m_ConnectionUpdate Handle period connection updates
	 */
	QTimer* m_ConnectionUpdate;

	/**
	 * @brief m_SerialPort The underlying serial port object used for this connection
	 */
	QSerialPort* m_SerialPort;

	/**
	 * @brief m_ReceiveBuffer This is used to buffer incoming data until enough bytes for a message are received
	 */
	QByteArray m_ReceiveBuffer;

	/**
	 * @brief m_ObisValueMapping Map from the obis strings to the list of received values
	 */
	QMap<QString, ObisValueList> m_ObisValueMapping;
};

}

Q_DECLARE_METATYPE(Ssmr::ConnectionData)
