#pragma once

#include <memory>

#include <QSettings>

namespace Ssmr
{

class Connection;
typedef std::shared_ptr<Connection> ConnectionPtr;

/**
 * @brief The ConnectionSerializer class provides a convenient interface to serialize connection instances
 */
class ConnectionSerializer
{
public:

	/**
	 * @brief DeserializeConnections Parse all connections from the given settings instance
	 * @param settings
	 * @return The parsed connection instance
	 */
	static QList<ConnectionPtr> DeserializeConnections(QSettings &m_Settings);

	/**
	 * @brief ConnectionSerializer
	 * @param connection The connection to serialize
	 */
	ConnectionSerializer(const ConnectionPtr &connection);

	/**
	 * @brief ~ConnectionSerializer Destructor
	 */
	virtual ~ConnectionSerializer();

	/**
	 * @brief save Store this connection within the given settings
	 * @param settings
	 * @return False if a nullptr connection was provided
	 */
	bool save(QSettings &settings);

private:

	ConnectionPtr m_Connection;
};

} // namespace Ssmr
