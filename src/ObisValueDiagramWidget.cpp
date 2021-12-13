#include "ObisValueDiagramWidget.h"
#include "ui_ObisValueDiagramWidget.h"

#include <QChart>
#include <QValueAxis>
#include <QChartView>
#include <QLineSeries>
#include <QDateTimeAxis>


ObisValueDiagramWidget::ObisValueDiagramWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::ObisValueDiagramWidget)
{
  ui->setupUi(this);

  QtCharts::QLineSeries *series = new QtCharts::QLineSeries();

  QtCharts::QChart *chart = new QtCharts::QChart();
  chart->addSeries(series);
  chart->legend()->hide();

  QtCharts::QDateTimeAxis *axisX = new QtCharts::QDateTimeAxis;
  axisX->setTickCount(10);
  axisX->setFormat("dd.MM");
  axisX->setTitleText("Date");
  chart->addAxis(axisX, Qt::AlignBottom);
  series->attachAxis(axisX);

  QtCharts::QValueAxis *axisY = new QtCharts::QValueAxis;
  axisY->setLabelFormat("%f");
  axisY->setTitleText("Power");
  chart->addAxis(axisY, Qt::AlignLeft);
  series->attachAxis(axisY);

  QtCharts::QChartView *chartView = new QtCharts::QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);

  ui->gridLayoutContent->addWidget(chartView);
}
//----------------------------------------------------------------------------------------------------------------------

ObisValueDiagramWidget::~ObisValueDiagramWidget()
{
  delete ui;
}
//----------------------------------------------------------------------------------------------------------------------
