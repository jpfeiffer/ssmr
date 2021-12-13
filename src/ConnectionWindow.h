#pragma once

#include <QSettings>
#include <QWidget>
#include <QUrl>
#include <QDir>

namespace Ssmr
{

namespace Ui
{
class ConnectionWindow;
}

class ObisValueLogWidget;

class Connection;
typedef std::shared_ptr<Connection> ConnectionPtr;
typedef std::weak_ptr<Connection> ConnectionWeakPtr;

/**
 * @brief The ConnectionWindow class Shows all information about a specific connection
 */
class ConnectionWindow : public QWidget
{
	Q_OBJECT

public:

	/**
	 * @brief ConnectionWindow Constructor
	 * @param connection
	 * @param parent
	 */
	explicit ConnectionWindow(ConnectionPtr connection, QWidget *parent = nullptr);

	/**
	 * @brief ~ConnectionWindow Destructor
	 */
	virtual ~ConnectionWindow() override;

private slots:

	/**
	 * @brief on_btnConnect_clicked Perform connect if not already connected
	 */
	void on_btnConnect_clicked();

	/**
	 * @brief on_btnDisconnect_clicked Perform disconnect if connected
	 */
	void on_btnDisconnect_clicked();

	/**
	 * @brief onConnectionChanged Handle connection changes
	 */
	void onConnectionChanged(bool connected) const;

	/**
	 * @brief onConnectionUpdate Periodic connection updates
	 */
	void onConnectionTimeChanged(qint64 elapsed) const;

	/**
	 * @brief onDataValueReceived Called when a new value for a given obis number is received
	 * @param obisValue
	 * @param timestamp
	 * @param dataValue
	 */
	void onDataValueReceived(const QString &obisValue, const qint64 &timestamp, const QVariant &dataValue);

	/**
	 * @brief on_btnSettings_clicked Open dialog to change connection settings
	 */
	void on_btnSettings_clicked();

private:

	Ui::ConnectionWindow *ui;

	/**
	 * @brief m_Connection The connection instance
	 */
	ConnectionPtr m_Connection;

	/**
	 * @brief m_Settings The persistent connection settings
	 */
	QSettings m_Settings;

	/**
	 * @brief m_CsvDir Where to save the csv files
	 */
	QDir m_CsvDir;

	/**
	 * @brief m_LogWidget Where to put log messages
	 */
	ObisValueLogWidget* m_LogWidget;
};

}
