#pragma once

#include <QMap>
#include <QPair>
#include <QWidget>
#include <QPointer>
#include <QSettings>
#include <QMainWindow>
#include <QListWidgetItem>

#include "Connection.h"

namespace Ssmr
{

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:

	/**
	 * @brief MainWindow Default constructor
	 * @param parent
	 */
	explicit MainWindow(QWidget *parent = nullptr);

	/**
	 * @brief ~MainWindow Default destructor
	 */
	virtual ~MainWindow() override;

	/**
	 * @brief getConnectionsMenu
	 * @return The main connections menu instance
	 */
	QMenu* getConnectionsMenu();

signals:

	/**
	 * @brief connectionAdded Emitted when a new connection was established
	 * @param data
	 */
	void connectionAdded(const ConnectionData &data);

private slots:

	/**
	 * @brief onNewConnection Add a new connection
	 * @param data
	 */
	void onAddConnection(const ConnectionData &data);

	/**
	 * @brief onChangedConnectionState Called when a single connection has changed its connection state
	 * @param connection
	 * @param connected
	 */
	void onChangedConnectionState(const ConnectionWeakPtr &connection, const bool &connected);

	/**
	 * @brief on_listWidget_itemClicked Called when the user clicks a connection in the list
	 * @param item
	 */
	void on_listWidget_itemClicked(QListWidgetItem *item);

	/**
	 * @brief on_btnAddConnection_clicked Handle logic to add a new connection
	 */
	void on_btnAddConnection_clicked();

	void on_btnRemoveConnection_clicked();

	void on_listWidget_itemSelectionChanged();

private:

	/**
	 * @brief rebuildConnectionList Update ui to show available connections
	 */
	void rebuildConnectionList();

	Ui::MainWindow *ui;

	/**
	 * @brief m_Settings The persistent connection settings
	 */
	QSettings m_Settings;

	/**
	 * @brief m_Connections The already established connections
	 */
	QList<ConnectionPtr> m_Connections;

	/**
	 * @brief m_ConnectionMapping Contains the information which list item and stacked widget belongs to which connection
	 */
	QMap<ConnectionPtr, QPair<QListWidgetItem*, QWidget*>> m_ConnectionMapping;

};

}
