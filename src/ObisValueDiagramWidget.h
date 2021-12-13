#ifndef OBISVALUEDIAGRAMWIDGET_H
#define OBISVALUEDIAGRAMWIDGET_H

#include <QWidget>

namespace Ui {
class ObisValueDiagramWidget;
}

class ObisValueDiagramWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ObisValueDiagramWidget(QWidget *parent = nullptr);
	~ObisValueDiagramWidget();

private:
	Ui::ObisValueDiagramWidget *ui;
};

#endif // OBISVALUEDIAGRAMWIDGET_H
