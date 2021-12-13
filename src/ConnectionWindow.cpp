#include "ConnectionWindow.h"
#include "ui_ConnectionWindow.h"

#include <QDir>
#include <QDebug>
#include <QFile>
#include <QTime>
#include <QFileInfo>
#include <QSerialPort>

#include "qtcsv/stringdata.h"
#include "qtcsv/writer.h"

#include "Connection.h"
#include "ConnectionDialog.h"
#include "ConnectionSerializer.h"
#include "HelpFunctions.h"

#include "ObisValueDiagramWidget.h"
#include "ObisValueLogWidget.h"

namespace Ssmr
{

ConnectionWindow::ConnectionWindow(ConnectionPtr connection, QWidget *parent)
  : QWidget(parent)
  , ui(new Ui::ConnectionWindow)
  , m_Connection(connection)
  , m_Settings()
  , m_CsvDir(qApp->applicationDirPath())
  , m_LogWidget(new ObisValueLogWidget(m_Connection, this))
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
  //ui->tabWidget->addTab(new ObisValueDiagramWidget(), QString("Diagram"));

  QStringList header;

  header << QString("timestamp");
  header << QString("value");
  header << QString("unit");

  m_CsvDir.cdUp();
  m_CsvDir.mkdir(QString("log"));
  m_CsvDir.cd(QString("log"));

  for(const auto &mapping : m_Connection->getConnectionData().getMappingObisNumbers())
  {
    QtCSV::StringData data;
    data.addRow(header);

    QString filePath = m_CsvDir.absoluteFilePath(QString("%1_%2.csv").arg(m_Connection->getName()).arg(mapping));

    QFileInfo fi(filePath);
    if(false == fi.exists())
    {
      QtCSV::Writer::write(filePath, data);
    }
  }

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

  connect(m_Connection.get(), &Connection::connectionTimeChanged, this, &ConnectionWindow::onConnectionTimeChanged);
  connect(m_Connection.get(), &Connection::connectionChanged, this, &ConnectionWindow::onConnectionChanged);
  connect(m_Connection.get(), &Connection::dataValueReceived, this, &ConnectionWindow::onDataValueReceived);
}
//----------------------------------------------------------------------------------------------------------------------

ConnectionWindow::~ConnectionWindow()
{
  qDebug() << "ConnectionWindow::~ConnectionWindow()";
  delete ui;
}
//----------------------------------------------------------------------------------------------------------------------

void ConnectionWindow::onConnectionTimeChanged(qint64 elapsed) const
{
  ui->lblStatus->setText(tr("Connected since %1").arg(QTime(0,0).addMSecs(elapsed).toString("hh:mm:ss")));
}
//----------------------------------------------------------------------------------------------------------------------

void ConnectionWindow::on_btnConnect_clicked()
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

void ConnectionWindow::on_btnDisconnect_clicked()
{
  m_Connection->disconnect();
  ui->lblStatus->setText(tr("Disconnected"));

  ui->btnConnect->setVisible((true == m_Connection->isValid()) && (false == m_Connection->isConnected()));
  ui->btnDisconnect->setVisible((true == m_Connection->isValid()) && (true == m_Connection->isConnected()));
}
//----------------------------------------------------------------------------------------------------------------------

void ConnectionWindow::on_btnSettings_clicked()
{
  ConnectionDialog* c = new ConnectionDialog(m_Connection->getConnectionData(), this);
  const auto reason = c->exec();

  if(QDialog::Accepted == reason)
  {
    m_Connection->setConnectionData(c->getConnectionData());
  }

  ConnectionSerializer s(m_Connection);
  s.save(m_Settings);

  c->deleteLater();
}
//----------------------------------------------------------------------------------------------------------------------

void ConnectionWindow::onConnectionChanged(bool connected) const
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

void ConnectionWindow::onDataValueReceived(const QString &obisValue, const qint64 &timestamp, const QVariant &dataValue)
{
  const auto mappings = m_Connection->getConnectionData().mappings;

  for(const auto &mapping : mappings)
  {
    if(mapping.obisNumber != obisValue) continue;

    if(nullptr != m_LogWidget)
    {
      const auto logged = m_LogWidget->onDataValueReceived(mapping.obisNumber, timestamp, dataValue, mapping.unit);

      if(true == logged)
      {

        QtCSV::StringData data;
        data.addRow(QStringList() << QString::number(timestamp) << dataValue.toString() << mapping.unit);

        QString filePath = m_CsvDir.absoluteFilePath(QString("%1_%2.csv").arg(m_Connection->getName())
                                                                         .arg(mapping.obisNumber));
        QtCSV::Writer::write(filePath, data, QString(","), QString("\""), QtCSV::Writer::WriteMode::APPEND);
      }
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------

}

