#pragma once

#include <QWidget>
#include <QUrl>

namespace Ssmr
{

namespace Ui
{
class DeviceWindow;
}

class ObisValueLogWidget;

class Connection;
typedef std::shared_ptr<Connection> ConnectionPtr;
typedef std::weak_ptr<Connection> ConnectionWeakPtr;

class DeviceWindow : public QWidget
{
	Q_OBJECT

public:

	explicit DeviceWindow(ConnectionPtr connection, QWidget *parent = nullptr);

	virtual ~DeviceWindow() override;

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

	//void on_pushButton_clicked();

private:

	Ui::DeviceWindow *ui;

	ConnectionPtr m_Connection;

	/**
	 * @brief m_LogWidget Where to put log messages
	 */
	ObisValueLogWidget* m_LogWidget;
};

}
