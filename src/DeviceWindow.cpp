#include "DeviceWindow.h"
#include "ui_DeviceWindow.h"

#include <QDebug>
#include <QFile>
#include <QTime>
#include <QSerialPort>

#include "Connection.h"
#include "HelpFunctions.h"

#include "ObisValueDiagramWidget.h"
#include "ObisValueLogWidget.h"

namespace Ssmr
{

DeviceWindow::DeviceWindow(ConnectionPtr connection, QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::DeviceWindow)
	, m_Connection(connection)
	, m_LogWidget(new ObisValueLogWidget(this))
{
	ui->setupUi(this);
	ui->lblConnectionName->setText(tr("Connection: %2").arg(m_Connection->getName()));
	ui->lblComPort->setText(m_Connection->getSerialPortName());

	const auto mapping = GetCommunicationProtocolDescriptionsMapping();
	for(const auto &protocol : mapping.keys())
	{
		if(protocol != m_Connection->getProtocol()) continue;

		const auto translator = mapping.value(protocol);
		if(nullptr == translator) continue;

		ui->lblProtocol->setText(translator());
	}

	ui->tabWidget->addTab(m_LogWidget, QString("Log"));
	ui->tabWidget->addTab(new ObisValueDiagramWidget(), QString("Diagram"));

	for(int i = 0; i < ui->tabWidget->count(); ++i)
	{
		ui->tabWidget->setTabVisible(i, false);
	}

	ui->lblWarningInvalidConnection->setVisible(!m_Connection->isValid());
	ui->btnConnect->setVisible(m_Connection->isValid());

	const auto toolTip = GetTooltipForSerialPortInfo(m_Connection->getSerialPortInfo());
	ui->lblSerialPortInfo->setToolTip(toolTip);

	ui->btnConnect->setVisible((true == m_Connection->isValid()) && (false == m_Connection->isConnected()));
	ui->btnDisconnect->setVisible((true == m_Connection->isValid()) && (true == m_Connection->isConnected()));

	connect(m_Connection.get(), &Connection::connectionTimeChanged, this, &DeviceWindow::onConnectionTimeChanged);
	connect(m_Connection.get(), &Connection::connectionChanged, this, &DeviceWindow::onConnectionChanged);
  connect(m_Connection.get(), &Connection::dataValueReceived, this, &DeviceWindow::onDataValueReceived);
}
//----------------------------------------------------------------------------------------------------------------------

DeviceWindow::~DeviceWindow()
{
	qDebug() << "DeviceWindow::~DeviceWindow()";
	delete ui;
}
//----------------------------------------------------------------------------------------------------------------------

void DeviceWindow::onConnectionTimeChanged(qint64 elapsed) const
{
	ui->lblStatus->setText(tr("Connected since %1").arg(QTime(0,0).addMSecs(elapsed).toString("hh:mm:ss")));
}
//----------------------------------------------------------------------------------------------------------------------

void DeviceWindow::on_btnConnect_clicked()
{
	const auto connected = m_Connection->connect();

	const auto serialPort = m_Connection->getSerialPort();
	const auto lastError = (nullptr != serialPort) ? serialPort->errorString() : tr("unknown error");
	ui->lblStatus->setText(connected ? tr("Connected") : tr("Not connected (%1)").arg(lastError));

	//disconnect only possible after a connect
	ui->btnConnect->setVisible((true == m_Connection->isValid()) && (false == m_Connection->isConnected()));
	ui->btnDisconnect->setVisible((true == m_Connection->isValid()) && (true == m_Connection->isConnected()));
}
//----------------------------------------------------------------------------------------------------------------------

void DeviceWindow::on_btnDisconnect_clicked()
{
	m_Connection->disconnect();
	ui->lblStatus->setText(tr("Disconnected"));

	ui->btnConnect->setVisible((true == m_Connection->isValid()) && (false == m_Connection->isConnected()));
	ui->btnDisconnect->setVisible((true == m_Connection->isValid()) && (true == m_Connection->isConnected()));
}
//----------------------------------------------------------------------------------------------------------------------

void DeviceWindow::onConnectionChanged(bool connected) const
{
	const auto serialPort = m_Connection->getSerialPort();
	const auto lastError = (nullptr != serialPort) ? serialPort->errorString() : tr("unknown error");
	ui->lblStatus->setText(connected ? tr("Connected") : tr("Not connected (%1)").arg(lastError));

	ui->btnConnect->setVisible((true == m_Connection->isValid()) && (false == m_Connection->isConnected()));
	ui->btnDisconnect->setVisible((true == m_Connection->isValid()) && (true == m_Connection->isConnected()));

	for(int i = 0; i < ui->tabWidget->count(); ++i)
	{
		ui->tabWidget->setTabVisible(i, connected);
	}
}
//----------------------------------------------------------------------------------------------------------------------

void DeviceWindow::onDataValueReceived(const QString &obisValue, const qint64 &timestamp, const QVariant &dataValue)
{
	const auto mappings = m_Connection->getConnectionData().mappings;

	for(const auto &mapping : mappings)
	{
		if(mapping.obisNumber != obisValue) continue;

		if(nullptr != m_LogWidget) m_LogWidget->onDataValueReceived(mapping.description,
																																timestamp,
																																dataValue.toString(),
																																mapping.unit);
	}

}
//----------------------------------------------------------------------------------------------------------------------

//void DeviceWindow::on_pushButton_clicked()
//{
//	QFile file(ui->lineEdit->text());

//	if(false == file.open(QIODevice::ReadOnly)) return;

//	QByteArray bytes = file.readAll();
//	m_Connection->parseSmlData(bytes);
//}
////----------------------------------------------------------------------------------------------------------------------

}
