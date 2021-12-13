#include "ConnectionDialog.h"
#include "ui_ConnectionDialog.h"

#include "ObisValueMappingWidget.h"

#include "TypeDefinitions.h"
#include "HelpFunctions.h"

#include <QDebug>
#include <QPushButton>
#include <QSerialPort>
#include <QSerialPortInfo>

#include <QTextStream>

namespace Ssmr
{

namespace
{

/*
 * The default monitored values with a 1h monitoring interval, except for the output which is instantanous
 */
const QList<ObisValueMapping> cDefaultObisValueMapping = {{"1-0:1.8.0*255", "Supply total", "Wh", {1, 0, 0}},
                                                          {"1-0:1.8.1*255", "Supply during T1", "Wh", {1, 0, 0}},
                                                          {"1-0:1.8.2*255", "Supply during T2", "Wh", {1, 0, 0}},
                                                          {"1-0:16.7.0*255", "Momentary output", "Wh", {0, 0, 0}}};

}

ConnectionDialog::ConnectionDialog(const ConnectionData &data,
                                   QWidget *parent)
  : QDialog(parent)
  , ui(new Ui::ConnectionDialog)
  , m_CurrentData(data)
  , m_CurrentMappingWidget(nullptr)
{
  ui->setupUi(this);

  loadSerialPorts();
  loadCommunicationProtocols();
  loadObisValueMappings();

  ui->edtName->setText(m_CurrentData.name);

  checkCompleteness();
}
//----------------------------------------------------------------------------------------------------------------------

ConnectionDialog::~ConnectionDialog()
{
  delete ui;
}
//----------------------------------------------------------------------------------------------------------------------

ConnectionData ConnectionDialog::getConnectionData()
{
  m_CurrentData.info = GetSerialPortInfoByPortName(m_CurrentData.serialPortName);

  m_CurrentData.mappings.clear();

  const auto widgets = findChildren<ObisValueMappingWidget*>();
  for(const auto &widget : widgets)
  {
    if(nullptr == widget) continue;

    m_CurrentData.mappings.append(widget->getMapping());
  }

  m_CurrentData.name = ui->edtName->text();
  return m_CurrentData;
}
//----------------------------------------------------------------------------------------------------------------------

void ConnectionDialog::on_comboBoxSerialPorts_activated(int index)
{
  QString existingConnectionName = tr("<unknown>");

  m_CurrentData.serialPortName = ui->comboBoxSerialPorts->currentData().toString();

  ui->lblSerialPortInfo->setToolTip((index < 0) ? QString() :
                                                  GetTooltipForSerialPortName(m_CurrentData.serialPortName));
  checkCompleteness();
}
//----------------------------------------------------------------------------------------------------------------------

void Ssmr::ConnectionDialog::on_comboBoxProtocol_activated(int index)
{
  Q_UNUSED(index)

  m_CurrentData.protocol = ui->comboBoxProtocol->currentData().value<Ssmr::CommunicationProtocol>();

  checkCompleteness();
}
//----------------------------------------------------------------------------------------------------------------------

void ConnectionDialog::on_btnAdd_clicked()
{
  const int pos = findChildren<ObisValueMappingWidget*>().count();
  ObisValueMappingWidget* widget = new ObisValueMappingWidget({}, {}, {}, {}, this);

  ui->verticalLayoutMappings->insertWidget(pos, widget);

  m_CurrentMappingWidget = widget;
}
//----------------------------------------------------------------------------------------------------------------------

void ConnectionDialog::on_btnRemove_clicked()
{
  if(nullptr != m_CurrentMappingWidget)
  {
    m_CurrentMappingWidget->close();
    m_CurrentMappingWidget->setParent(nullptr);
    m_CurrentMappingWidget->deleteLater();
  }

  const auto widgets = findChildren<ObisValueMappingWidget*>();
  m_CurrentMappingWidget = widgets.isEmpty() ? nullptr : widgets.last();
}
//----------------------------------------------------------------------------------------------------------------------

void ConnectionDialog::on_edtName_textChanged(const QString &)
{
  checkCompleteness();
}
//----------------------------------------------------------------------------------------------------------------------

void ConnectionDialog::loadSerialPorts()
{
  const auto serialPortInfos = QSerialPortInfo::availablePorts();
  for (const QSerialPortInfo &serialPortInfo : serialPortInfos)
  {
    ui->comboBoxSerialPorts->addItem(serialPortInfo.portName(), QVariant::fromValue(serialPortInfo.portName()));

    if(serialPortInfo.portName() == m_CurrentData.serialPortName)
    {
      on_comboBoxSerialPorts_activated(ui->comboBoxSerialPorts->currentIndex());
    }
  }

  //select first serial port if possible
  if(true == m_CurrentData.serialPortName.isEmpty())
  {
    m_CurrentData.serialPortName = ui->comboBoxSerialPorts->currentData().toString();
    m_CurrentData.info = GetSerialPortInfoByPortName(m_CurrentData.serialPortName);
  }
  else
  {
    m_CurrentData.info = GetSerialPortInfoByPortName(m_CurrentData.serialPortName);
  }

  const auto toolTip = GetTooltipForSerialPortInfo(m_CurrentData.info);
  ui->lblSerialPortInfo->setToolTip(toolTip);
}
//----------------------------------------------------------------------------------------------------------------------

void ConnectionDialog::loadCommunicationProtocols()
{
  const auto protocolMapping = GetCommunicationProtocolDescriptionsMapping();
  for(const auto &protocol : protocolMapping.keys())
  {
    const auto translator = protocolMapping.value(protocol);
    if(nullptr == translator) continue;

    ui->comboBoxProtocol->addItem(translator(), QVariant::fromValue(protocol));
  }

  //select the first protocol if no existing protocol is selected
  if(CommunicationProtocol::eUnknown == m_CurrentData.protocol)
  {
    ui->comboBoxProtocol->setCurrentIndex(0);
    m_CurrentData.protocol = ui->comboBoxProtocol->itemData(0).value<CommunicationProtocol>();
  }
  else
  {
    ui->comboBoxProtocol->setCurrentIndex(ui->comboBoxProtocol->findData(QVariant::fromValue(m_CurrentData.protocol)));
  }

  checkCompleteness();
}
//----------------------------------------------------------------------------------------------------------------------

void ConnectionDialog::loadObisValueMappings()
{
  if(true == m_CurrentData.mappings.isEmpty())
  {
    m_CurrentData.mappings = cDefaultObisValueMapping;
  }

  for(const auto &mapping : m_CurrentData.mappings)
  {
    const int pos = findChildren<ObisValueMappingWidget*>().count();
    ObisValueMappingWidget* widget = new ObisValueMappingWidget(mapping.obisNumber,
                                                                mapping.description,
                                                                mapping.unit,
                                                                mapping.interval,
                                                                this);

    ui->verticalLayoutMappings->insertWidget(pos, widget);
    m_CurrentMappingWidget = widget;
  }
}
//----------------------------------------------------------------------------------------------------------------------

void ConnectionDialog::checkCompleteness()
{
  auto okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
  if(nullptr != okButton)
  {
    const bool name = (false == ui->edtName->text().isEmpty());
    const bool protocol = (0 < ui->comboBoxProtocol->currentIndex());
    const bool serial = (0 <= ui->comboBoxSerialPorts->currentIndex());

    okButton->setEnabled(name && protocol && serial);
  }
}
//----------------------------------------------------------------------------------------------------------------------

}
