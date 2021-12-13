#include "ObisValueMappingWidget.h"
#include "ui_ObisValueMappingWidget.h"

namespace Ssmr
{

ObisValueMappingWidget::ObisValueMappingWidget(const QString &obisValue,
                                               const QString &description,
                                               const QString &unit,
                                               const Duration &interval,
                                               QWidget *parent)
  : QWidget(parent)
  , ui(new Ui::ObisValueMappingWidget)
{
  ui->setupUi(this);
  ui->edtObisValue->setText(obisValue);
  ui->edtDescription->setText(description);
  ui->edtUnit->setText(unit);
  ui->edtInterval->setText(QString("%1s").arg(interval.toSeconds()));
}
//----------------------------------------------------------------------------------------------------------------------

ObisValueMappingWidget::~ObisValueMappingWidget()
{
  delete ui;
}
//----------------------------------------------------------------------------------------------------------------------

ObisValueMapping ObisValueMappingWidget::getMapping() const
{
  return {ui->edtObisValue->text(),
        ui->edtDescription->text(),
        ui->edtUnit->text(),
        Duration::FromString(ui->edtInterval->text())};
}
//----------------------------------------------------------------------------------------------------------------------

}
