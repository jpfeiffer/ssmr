#pragma once

#include <QDialog>

#include "Connection.h"

namespace Ssmr
{

namespace Ui {
class ConnectionDialog;
}

class ObisValueMappingWidget;

/**
 * @brief The ConnectionDialog class Create a new connection
 */
class ConnectionDialog : public QDialog
{

	Q_OBJECT

public:

	/**
	 * @brief ConnectionDialog Default constructor
	 * @param data
	 * @param parent
	 */
	explicit ConnectionDialog(const ConnectionData &data,
														QWidget *parent = nullptr);

	/**
	 * @brief ~ConnectionDialog Default destructor
	 */
	virtual ~ConnectionDialog() override;

	/**
	 * @brief getConnectionData
	 * @return The selected connection data
	 */
	ConnectionData getConnectionData();

private slots:

	/**
	 * @brief on_comboBoxSerialPorts_activated Update the serial port tooltip when a new serial port is selected
	 * @param index
	 */
	void on_comboBoxSerialPorts_activated(int index);

	/**
	 * @brief on_comboBoxProtocol_activated Update the logic to detect is a valid protocol is selected
	 * @param index
	 */
	void on_comboBoxProtocol_activated(int index);

	/**
	 * @brief on_btnAdd_clicked Add an empty obis mapping
	 */
	void on_btnAdd_clicked();

	/**
	 * @brief on_btnRemove_clicked Remove the latest added obis mapping
	 */
	void on_btnRemove_clicked();

	void on_edtName_textChanged(const QString &arg1);

private:

	/**
	 * @brief loadSerialPorts fill serial ports list and select current port if available
	 */
	void loadSerialPorts();

	/**
	 * @brief loadCommunicationProtocols fill protocol list and select current protocol if available
	 */
	void loadCommunicationProtocols();

	/**
	 * @brief loadObisValueMappings fill list with existing obis value mappings if available
	 */
	void loadObisValueMappings();

	/**
	 * @brief checkCompleteness Check if complete information are entered
	 */
	void checkCompleteness();

	Ui::ConnectionDialog *ui;

	/**
	 * @brief m_CurrentData Set to the current data if editing is enabled
	 */
	ConnectionData m_CurrentData;

	/**
	 * @brief m_CurrentMappingWidget Used to add/remove properly
	 */
	ObisValueMappingWidget* m_CurrentMappingWidget;
};

}
