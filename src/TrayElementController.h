#pragma once

#include <QObject>

class QSystemTrayIcon;
class QMenu;
class QAction;

namespace Ssmr
{

/**
 * @brief The TrayElementController class wraps a System Tray Icon and a context menu.
 */
class TrayElementController : public QObject
{

	Q_OBJECT

public:
	/**
	 * @brief TrayElementController standard ctor
	 * @param parent the parent object.
	 */
	explicit TrayElementController(QObject *parent = 0);

	/**
	 * @brief ~TrayElementController Default destructor
	 */
	virtual ~TrayElementController() override;

	/**
	 * @brief setMenu The menu to use
	 * @param menu
	 *
	 * @note This instance does NOT take ownership for the provided menu
	 */
	void setConnectionMenu(QMenu* menu);

signals:

	/**
	 * @brief exit the user clicked the show Application item.
	 */
	void show();

public slots:

	/**
		 * @brief triggerDefaultAction Triggers the default action
		 */
	void triggerDefaultAction();

private:

	/**
	 * @brief m_TrayIcon the tray icon that will be displayed in the OS tray area.
	 */
	QSystemTrayIcon *const m_TrayIcon;

	/**
	 * @brief m_TrayMenu The menu used as the tray menu
	 */
	QMenu* const m_TrayMenu;

	/**
	 * @brief m_DefaultAction the default action that will be performed on double click on the app's system tray icon.
	 */
	QAction *m_DefaultAction;
};

}
