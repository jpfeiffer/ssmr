#include "TrayElementController.h"

#include <QSystemTrayIcon>
#include <QMenu>
#include <QUrl>

namespace Ssmr
{

TrayElementController::TrayElementController(QObject *parent)
  : QObject(parent)
    , m_TrayIcon(new QSystemTrayIcon(QIcon(":/icon.ico"), this))
    , m_TrayMenu(new QMenu())
  , m_DefaultAction(nullptr)
{
  m_TrayIcon->show();

  connect(m_TrayIcon, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason reason){
    if (QSystemTrayIcon::ActivationReason::DoubleClick == reason)
    {
      if (nullptr != m_DefaultAction)
      {
        m_DefaultAction->trigger();
      }
    }
  });
}
//----------------------------------------------------------------------------------------------------------------------

TrayElementController::~TrayElementController()
{
  m_TrayMenu->deleteLater();
}
//----------------------------------------------------------------------------------------------------------------------

void TrayElementController::setConnectionMenu(QMenu *menu)
{
  m_TrayIcon->setContextMenu(menu);
}
//----------------------------------------------------------------------------------------------------------------------

void TrayElementController::triggerDefaultAction()
{
  if(m_DefaultAction != nullptr)
  {
    m_DefaultAction->trigger();
  }
}
//------------------------------------------------------------------------------

}
