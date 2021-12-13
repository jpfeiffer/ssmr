#include "ObisValueWidget.h"
#include "ui_ObisValueWidget.h"

#include <QDebug>
#include <QDateTime>

namespace Ssmr
{

const QMap<QString, int> siPrefixFactors = {{"T", 12},
                                            {"G", 9},
                                            {"M", 6},
                                            {"k", 3},
                                            { "", 0},
                                            {"m", -3},
                                            {"µ", -6},
                                            {"n", -9},
                                            {"p", -12}};

ObisValueWidget::ObisValueWidget(const ObisValueMapping &mapping, QWidget *parent)
  : QWidget(parent)
  , ui(new Ui::ObisValueWidget)
  , m_Mapping(mapping)
  , m_Interval()
{
  ui->setupUi(this);
  ui->lblObisValueDescription->setText(mapping.description);
  ui->lblObisValueDescription->setToolTip(mapping.obisNumber);

  ui->lblValue->setText(QString("%1%2").arg(QString::number(0.0, 'g', 1)).arg(mapping.unit));

  m_Interval.invalidate();
}
//----------------------------------------------------------------------------------------------------------------------

ObisValueWidget::~ObisValueWidget()
{
  delete ui;
}
//----------------------------------------------------------------------------------------------------------------------

bool ObisValueWidget::onNewValue(const qint64 &timestamp, const double &newValue)
{
  const auto elapsed = (static_cast<quint64>(m_Interval.elapsed()) > m_Mapping.interval.toMilliseconds());
  qDebug() << "ObisValueWidget::onNewValue() accepting new value=" << newValue;

  if((true == elapsed) || (false == m_Interval.isValid()))
  {
    qDebug() << "ObisValueWidget::onNewValue() accepting new value=" << newValue
             << " for obis number=" << m_Mapping.obisNumber;

    double value = newValue;
    QString unit = m_Mapping.unit;

    if(false == m_Mapping.unit.isEmpty())
    {
      QString baseUnit = m_Mapping.unit;
      const QString prefix = m_Mapping.unit.left(1);
      int factor = siPrefixFactors.value(prefix, 0);

      if(factor != 0)
      {
        baseUnit = m_Mapping.unit.mid(1);
      }


      while(1000.0 < value)
      {
        value /= 1000.0;
        factor += 3;

        if(false == siPrefixFactors.values().contains(factor)) break;
      }
      while(1.0 > value)
      {
        value *= 1000.0;
        factor -= 3;

        if(false == siPrefixFactors.values().contains(factor)) break;
      }

      if(false == siPrefixFactors.values().contains(factor))
      {
        value = newValue;
        unit = m_Mapping.unit;
      }
      else
      {
        unit = QString("%1%2").arg(siPrefixFactors.key(factor)).arg(baseUnit);
      }

    }

    ui->lblValue->setText(QString("%1 %2").arg(QString::number(value, 'f', 1)).arg(unit));
    ui->lblTimestamp->setText(QDateTime::fromMSecsSinceEpoch(timestamp).toString());
    m_Interval.restart();

    return true;
  }

  return false;
}
//----------------------------------------------------------------------------------------------------------------------

void ObisValueWidget::invalidate()
{
  m_Interval.invalidate();
}
//----------------------------------------------------------------------------------------------------------------------

}
