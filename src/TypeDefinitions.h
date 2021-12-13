#pragma once

#include <QObject>
#include <QSerialPortInfo>
#include <QRegularExpression>

namespace Ssmr
{

Q_NAMESPACE

enum class CommunicationProtocol
{
	eUnknown = 0,
	eDSSInformation = 1,
	eD0Dialog = 2,
};
Q_ENUM_NS(CommunicationProtocol)

struct Duration
{

	/**
	 * @brief Duration Default constructor which will create an invalid instance
	 * @param h
	 * @param m
	 * @param s
	 */
	Duration(quint64 h = {}, quint64 m = {}, quint64 s = {})
		: hours(h)
		, minutes(m)
		, seconds(s)
	{
	}

	/**
	 * @brief isValid
	 * @return False is all time values are 0
	 */
	bool isValid() const
	{
		return 0 != (hours + minutes + seconds);
	}

	/**
	 * @brief toMilliseconds
	 * @return This duration in milliseconds
	 */
	quint64 toMilliseconds() const
	{
		return 1000*seconds + 60*1000*minutes + 60*60*1000*hours;
	}

	/**
	 * @brief toSeconds
	 * @return This duration in seconds
	 */
	quint64 toSeconds() const
	{
		return seconds + 60*minutes + 60*60*hours;
	}

	/**
	 * @brief FromString Parse it from a string representation
	 * @param duration The string to parse. The input can contain hour values with <number>h and minutes with
	 * <minutes>m or seconds with <seconds>s
	 * @return The constructed duration instance parsed from the string. Returns an invalid instance if the conversion
	 * fails.
	 */
	static Duration FromString(const QString &duration)
	{
		Duration d;

		static const QRegularExpression r("^\\s?(?:(\\d*)h)?\\s*(?:(\\d*)m)?\\s*(?:(\\d*)s)?$");

		const auto match = r.match(duration);

		if(true == match.hasMatch())
		{
			d.hours = match.captured(1).toULongLong();
			d.minutes = match.captured(2).toULongLong();
			d.seconds = match.captured(3).toULongLong();
		}

		return d;
	}

	quint64 hours;
	quint64 minutes;
	quint64 seconds;
};

/**
 * @brief The ObisValueMapping struct represents a single maping between an OBIS number and a description and unit
 *
 * Only if a mapping exists are the values for a OBIS number monitored. Each values has its own refresh interval
 */
struct ObisValueMapping
{
	ObisValueMapping(const QString &o = {},
									 const QString &d = {},
									 const QString &u = {},
									 const Duration &i = {})
		: obisNumber(o)
		, description(d)
		, unit(u)
		, interval(i)
	{}

	/**
	 * @brief isValid Returns true if the obis number is not empty
	 * @return
	 */
	bool isValid() const
	{
		return !obisNumber.isEmpty();
	}

	QString obisNumber;
	QString description;
	QString unit;

	//!How often to refresh. Values with an invalid interval are refreshed for every reading
	Duration interval;
};

/**
 * @brief The ConnectionData class contains all information for a single connection instance
 */
class ConnectionData
{
	public:

	/**
	 * @brief ConnectionData Default constructor creates an invalid instance
	 */
	inline ConnectionData(QString n = QString(),
												QString s = QString(),
												QSerialPortInfo i = QSerialPortInfo(),
												const CommunicationProtocol &p = CommunicationProtocol::eUnknown,
												const QList<ObisValueMapping> &m = {})
		: name(n)
		, serialPortName(s)
		, info(i)
		, protocol(p)
		, mappings(m)
	{}

	/**
	 * @brief ConnectionData Default copy constructor
	 * @param d
	 */
	inline ConnectionData(const ConnectionData &d)
		: ConnectionData(d.name, d.serialPortName, d.info, d.protocol, d.mappings)
	{}

	/**
	 * @brief operator = Default assignment operator
	 * @param other
	 * @return
	 */
	ConnectionData &operator=(const ConnectionData &other)
	{
		if(this != &other)
		{
			name = other.name;
			serialPortName = other.serialPortName;
			info = other.info;
			protocol = other.protocol;
			mappings = other.mappings;
		}

		return *this;
	}

	/**
	 * @brief ~ConnectionData Default destructor
	 */
	virtual ~ConnectionData()
	{}

	/**
	 * @brief getMappingObisNumbers Returns a list with all obis number from the contained mappings
	 * @return
	 */
	QStringList getMappingObisNumbers() const
	{
		QStringList names{};

		for(const auto &mapping : mappings)
		{
			names.append(mapping.obisNumber);
		}

		return names;
	}

	ObisValueMapping getMappingByObisNumber(const QString &obisNumber) const
	{
		for(const auto &mapping : mappings)
		{
			if(mapping.obisNumber == obisNumber) return mapping;
		}
		return {};
	}

	/**
	 * @brief isValid
	 * @return True if the name and serial port name are not empty a valid serial port infor is set and the protocol
	 * is not unknown
	 */
	bool isValid() const
	{
		return (false == name.isEmpty()) &&
					 (false == serialPortName.isEmpty()) &&
					 (false == info.isNull()) &&
					 (CommunicationProtocol::eUnknown != protocol);
	}

	QString name;
	QString serialPortName;
	QSerialPortInfo info;
	CommunicationProtocol protocol;
	QList<ObisValueMapping> mappings;
};

}
