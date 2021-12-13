#include "ObisValueLogWidget.h"
#include "ui_ObisValueLogWidget.h"

#include <QDateTime>

namespace Ssmr
{

ObisValueLogWidget::ObisValueLogWidget(const ConnectionPtr &connection, QWidget *parent)
  : QWidget(parent)
  , ui(new Ui::ObisValueLogWidget)
  , m_Connection(connection)
{
  ui->setupUi(this);

  connect(m_Connection.get(), &Connection::connectionChanged, this, &ObisValueLogWidget::onConnectionChanged);
  connect(m_Connection.get(), &Connection::mappingAdded, this, &ObisValueLogWidget::onMappingAdded);
  connect(m_Connection.get(), &Connection::mappingRemoved, this, &ObisValueLogWidget::onMappingRemoved);

  for(const auto &mapping : m_Connection->getConnectionData().mappings)
  {
    const auto &widget = new ObisValueWidget(mapping, this);
    m_ValueWidgets[mapping.obisNumber] = widget;

    ui->verticalLayoutObisValueWidgets->addWidget(widget);
  }
}
//----------------------------------------------------------------------------------------------------------------------

ObisValueLogWidget::~ObisValueLogWidget()
{
  delete ui;
}
//----------------------------------------------------------------------------------------------------------------------

bool ObisValueLogWidget::onDataValueReceived(const QString &obisValue,
                                             const qint64 &timestamp,
                                             const QVariant &dataValue,
                                             const QString &unit)
{
  Q_UNUSED(unit)

  const auto &widget = m_ValueWidgets.value(obisValue);
  if(nullptr == widget) return false;

  bool ok{};
  const double value = dataValue.toDouble(&ok);

  if(false == ok)
  {
    qDebug() << "ObisValueLogWidget::onDataValueReceived() discarding invald value=" << dataValue
             << " for obis number=" << obisValue;
    return false;
  }

  return widget->onNewValue(timestamp, value);
}
//----------------------------------------------------------------------------------------------------------------------

void ObisValueLogWidget::onConnectionChanged(const bool &)
{
  for(const auto &widget : m_ValueWidgets.values())
  {
    if(nullptr == widget) continue;

    widget->invalidate();
  }
}
//----------------------------------------------------------------------------------------------------------------------

void ObisValueLogWidget::onMappingAdded(const ObisValueMapping &mapping)
{
  if(false == mapping.isValid()) return;

  const auto &widget = new ObisValueWidget(mapping, this);
  m_ValueWidgets[mapping.obisNumber] = widget;

  ui->verticalLayoutObisValueWidgets->addWidget(widget);
}
//----------------------------------------------------------------------------------------------------------------------

void ObisValueLogWidget::onMappingRemoved(const ObisValueMapping &mapping)
{
  auto widget = m_ValueWidgets.value(mapping.obisNumber);
  if(nullptr!= widget)
  {
    widget->close();
    widget->deleteLater();
  }

  m_ValueWidgets.remove(mapping.obisNumber);
}
//----------------------------------------------------------------------------------------------------------------------

}
