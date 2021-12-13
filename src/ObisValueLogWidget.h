#pragma once

#include <QWidget>
#include <QTimer>

#include "Connection.h"
#include "ObisValueWidget.h"

namespace Ssmr
{

namespace Ui
{
class ObisValueLogWidget;
}

/**
 * @brief The ObisValueLogWidget class provides the ui to log received values
 */
class ObisValueLogWidget : public QWidget
{
	Q_OBJECT

public:

	/**
	 * @brief ObisValueLogWidget Default constructor
	 * @param parent
	 */
	explicit ObisValueLogWidget(const ConnectionPtr &connection, QWidget *parent = nullptr);

	/**
	 * @brief ~ObisValueLogWidget Default destructor
	 */
	virtual ~ObisValueLogWidget() override;

public slots:

	/**
	 * @brief onDataValueReceived Add a new line to the list of received values
	 * @param obisValue
	 * @param timestamp
	 * @param dataValue
	 * @param unit
	 */
	bool onDataValueReceived(const QString &obisValue,
													 const qint64 &timestamp,
													 const QVariant &dataValue,
													 const QString &unit);

private slots:

	/**
	 * @brief onConnectionChanged The connection status has changed. This will invalidate all log widgets
	 * @param connected
	 */
	void onConnectionChanged(const bool &connected);

	/**
	 * @brief onMappingAdded A new mapping added for this connection
	 * @param mapping
	 */
	void onMappingAdded(const ObisValueMapping &mapping);

	/**
	 * @brief onMappingRemoved A mapping has been removed for this connection
	 * @param mapping
	 */
	void onMappingRemoved(const ObisValueMapping &mapping);

private:

	/**
	 * @brief ui The user interface
	 */
	Ui::ObisValueLogWidget *ui;

	/**
	 * @brief m_Connection Which connection to represent
	 */
	ConnectionPtr m_Connection;

	/**
	 * @brief m_ValueWidgets Where to log each number
	 */
	QMap<QString, ObisValueWidget*> m_ValueWidgets;
};

}
