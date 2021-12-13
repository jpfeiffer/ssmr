#pragma once

#include <QWidget>

#include "TypeDefinitions.h"

namespace Ssmr
{

namespace Ui
{
class ObisValueMappingWidget;
}

/**
 * @brief The ObisValueMappingWidget class represents a single mapping from an OBIS number to a text description and
 * a unit string
 */
class ObisValueMappingWidget : public QWidget
{

	Q_OBJECT

public:

	/**
	 * @brief ObisValueMappingWidget Default constructor
	 * @param parent
	 */
	explicit ObisValueMappingWidget(const QString &obisValue = {},
																	const QString &description = {},
																	const QString &unit = {},
																	const Duration &interval = {},
																	QWidget *parent = nullptr);

	/**
	 * @brief ~ObisValueMappingWidget Default destructor
	 */
	virtual ~ObisValueMappingWidget() override;

	/**
	 * @brief getMapping
	 * @return The mapping instance represented by this widget
	 */
	ObisValueMapping getMapping() const;

private:

	/**
	 * @brief ui The user interface instance for this widget
	 */
	Ui::ObisValueMappingWidget *ui;
};

}
