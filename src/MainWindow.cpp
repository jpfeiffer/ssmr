#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "ConnectionDialog.h"
#include "ConnectionWindow.h"
#include "ConnectionSerializer.h"
#include "HelpFunctions.h"

#include <QDebug>
#include <QMessageBox>

namespace Ssmr
{

namespace
{

ConnectionPtr GetConnectionForListWidgetItem(QListWidgetItem* item,
                                             QMap<ConnectionPtr, QPair<QListWidgetItem*, QWidget*>> mapping)
{
  if(nullptr == item) return {};

  for(const auto &connection : mapping.keys())
  {
    if(nullptr == connection) continue;

    if(mapping.value(connection).first == item) return connection;
  }

  return {};
}
//----------------------------------------------------------------------------------------------------------------------

}

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , m_Settings()
{
  ui->setupUi(this);

  ui->btnRemoveConnection->setEnabled(!ui->listWidget->selectedItems().isEmpty());

  connect(this, &MainWindow::connectionAdded, this, &MainWindow::onAddConnection);

  connect(ui->actionNewConnection, &QAction::triggered, this, &MainWindow::on_btnAddConnection_clicked);
  connect(ui->actionRemoveConnection, &QAction::triggered, this, &MainWindow::on_btnRemoveConnection_clicked);

  connect(ui->actionQuit, &QAction::triggered, []()
  {
    qDebug() << "MainWindow::MainWindow() quit clicked";
    qApp->exit(0);
  });

  m_Connections = ConnectionSerializer::DeserializeConnections(m_Settings);
  for(const auto &connection : m_Connections)
  {
    const ConnectionWeakPtr connectionWeakPtr = connection;

    connect(connection.get(), &Connection::connectionChanged,
            this, std::bind(&MainWindow::onChangedConnectionState, this, connectionWeakPtr, std::placeholders::_1));
  }

  rebuildConnectionList();
}
//----------------------------------------------------------------------------------------------------------------------

MainWindow::~MainWindow()
{
  delete ui;
}
//----------------------------------------------------------------------------------------------------------------------

QMenu *MainWindow::getConnectionsMenu()
{
  return ui->menuConnections;
}
//----------------------------------------------------------------------------------------------------------------------

void MainWindow::onAddConnection(const ConnectionData &data)
{
  const auto connection = std::make_shared<Connection>(data);
  const ConnectionWeakPtr connectionWeakPtr = connection;

  connect(connection.get(), &Connection::connectionChanged,
          this, [this, connectionWeakPtr](bool changed){ this->onChangedConnectionState(connectionWeakPtr, changed); });

  ConnectionSerializer s(connection);

  const auto saved = s.save(m_Settings);

  if(true == saved)
  {
    m_Connections.append(connection);
  }

  rebuildConnectionList();
}
//----------------------------------------------------------------------------------------------------------------------

void MainWindow::onChangedConnectionState(const ConnectionWeakPtr &connection, const bool &connected)
{
  const auto connectionPtr = connection.lock();
  if(nullptr == connectionPtr) return;

  const auto item = m_ConnectionMapping.value(connectionPtr).first;
  if(nullptr == item) return;

  item->setIcon(connected ? QIcon(":/connect.png") : QIcon(":/disconnect.png"));
}
//----------------------------------------------------------------------------------------------------------------------

void MainWindow::rebuildConnectionList()
{
  //we only need to clear the widgets, the items belong to the list widget and will get deleted automatically by clear()
  for(const auto &value : m_ConnectionMapping.values())
  {
    value.second->deleteLater();
  }
  m_ConnectionMapping.clear();

  ui->listWidget->clear();
  ui->stackedWidget->setCurrentWidget(ui->pageNoConnection);

  for(const auto &connection : m_Connections)
  {
    QListWidgetItem* item = new QListWidgetItem(QIcon(":/disconnect.png"), connection->getName());
    ConnectionWindow* window = new ConnectionWindow(connection, ui->stackedWidget);

    ui->listWidget->addItem(item);

    ui->stackedWidget->addWidget(window);

    m_ConnectionMapping[connection] = {item, window};
  }
}
//----------------------------------------------------------------------------------------------------------------------

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
  //reset in case the item is not found
  ui->stackedWidget->setCurrentWidget(ui->pageNoConnection);

  for(const auto &value : m_ConnectionMapping.values())
  {
    if(item == value.first) ui->stackedWidget->setCurrentWidget(value.second);
  }
}
//----------------------------------------------------------------------------------------------------------------------

void MainWindow::on_listWidget_itemSelectionChanged()
{
  ui->btnRemoveConnection->setEnabled(!ui->listWidget->selectedItems().isEmpty());
}
//----------------------------------------------------------------------------------------------------------------------

void MainWindow::on_btnAddConnection_clicked()
{
  qDebug() << "MainWindow::on_btnAddConnection_clicked() new connection clicked";

  ConnectionDialog* c = new ConnectionDialog({}, this);
  const auto reason = c->exec();

  if(QDialog::Accepted == reason)
  {
    emit connectionAdded(c->getConnectionData());
  }

  c->deleteLater();
}
//----------------------------------------------------------------------------------------------------------------------

void MainWindow::on_btnRemoveConnection_clicked()
{
  qDebug() << "MainWindow::on_btnRemoveConnection_clicked()";

  if(true == ui->listWidget->selectedItems().isEmpty()) return;

  const auto item = ui->listWidget->selectedItems().first();
  if(nullptr == item) return;

  const auto connection = GetConnectionForListWidgetItem(item, m_ConnectionMapping);
  if(nullptr == connection) return;

  QMessageBox msgBox;
  msgBox.setWindowTitle("Remove connection?");
  msgBox.setText(tr("Do you want to remove the connection '%1'?").arg(connection->getName()));
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::No);

  int ret = msgBox.exec();

  if(QMessageBox::Yes == ret)
  {
    const auto pair = m_ConnectionMapping.value(connection);
    m_ConnectionMapping.remove(connection);
    m_Connections.removeAll(connection);

    m_Settings.beginGroup("connections");
    m_Settings.remove(connection->getName());
    m_Settings.endGroup();

    delete pair.first;
    pair.second->deleteLater();
  }
}
//----------------------------------------------------------------------------------------------------------------------

}
