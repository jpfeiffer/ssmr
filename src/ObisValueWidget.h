#pragma once

#include <QWidget>
#include <QElapsedTimer>
#include <QTimer>

#include "TypeDefinitions.h"

namespace Ssmr
{

namespace Ui {
class ObisValueWidget;
}

/**
 * @brief The ObisValueWidget class Displays a single value
 */
class ObisValueWidget : public QWidget
{
	Q_OBJECT

public:

	/**
	 * @brief ObisValueWidget Default constructor
	 * @param mapping
	 * @param parent
	 */
	explicit ObisValueWidget(const ObisValueMapping &mapping, QWidget *parent = nullptr);

	/**
	 * @brief ~ObisValueWidget Default destructor
	 */
	virtual ~ObisValueWidget();

	/**
	 * @brief onNewValue Called when a new value is received
	 * @param newValue
	 */
	bool onNewValue(const qint64 &timestamp, const double &newValue);

	/**
	 * @brief reset Reset the timer
	 *
	 * This will accept the next new value received
	 */
	void invalidate();

private:

	Ui::ObisValueWidget *ui;

	/**
	 * @brief m_Mapping What mapping to follow
	 */
	ObisValueMapping m_Mapping;

	/**
	 * @brief m_Interval When to accept new data
	 */
	QElapsedTimer m_Interval;
};

}
