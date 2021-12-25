#include "Connection.h"

#include "sml/sml_file.h"
#include "sml/sml_boolean.h"

#include <QDebug>
#include <QDateTime>
#include <QSerialPort>

#include <QtMath>

namespace Ssmr
{

Connection::Connection(const ConnectionData &data, QObject *parent)
  : QObject(parent)
  , m_ConnectionData(data)
  , m_ConnectionDuration()
  , m_ConnectionUpdate(new QTimer(this))
  , m_SerialPort(new QSerialPort(this))
{
  QObject::connect(m_ConnectionUpdate, &QTimer::timeout, this, &Connection::onConnectionUpdate);
  QObject::connect(m_SerialPort, &QSerialPort::readyRead, this, &Connection::onDataReceived);

  m_ConnectionUpdate->start(100);
}
//----------------------------------------------------------------------------------------------------------------------

Connection::~Connection()
{
}
//----------------------------------------------------------------------------------------------------------------------

ConnectionData Connection::getConnectionData() const
{
  return m_ConnectionData;
}
//----------------------------------------------------------------------------------------------------------------------

bool Connection::isValid() const
{
  return (false == m_ConnectionData.info.isNull()) && (nullptr != m_SerialPort);
}
//----------------------------------------------------------------------------------------------------------------------

QString Connection::getName() const
{
  return m_ConnectionData.name;
}
//----------------------------------------------------------------------------------------------------------------------

QString Connection::getSerialPortName() const
{
  return m_ConnectionData.serialPortName;
}
//----------------------------------------------------------------------------------------------------------------------

QSerialPortInfo Connection::getSerialPortInfo() const
{
  return m_ConnectionData.info;
}
//----------------------------------------------------------------------------------------------------------------------

CommunicationProtocol Connection::getProtocol() const
{
  return m_ConnectionData.protocol;
}
//----------------------------------------------------------------------------------------------------------------------

QSerialPort *Connection::getSerialPort()
{
  return m_SerialPort;
}
//----------------------------------------------------------------------------------------------------------------------

const QSerialPort *Connection::getSerialPort() const
{
  return m_SerialPort;
}
//----------------------------------------------------------------------------------------------------------------------

bool Connection::isConnected() const
{
  return (nullptr != m_SerialPort) ? m_SerialPort->isOpen() : false;
}
//----------------------------------------------------------------------------------------------------------------------

void Connection::disconnect()
{
  if(nullptr != m_SerialPort)
  {
    m_ConnectionDuration.invalidate();
    m_SerialPort->clearError();
    m_SerialPort->close();

    emit connectionChanged(false);
  }
}
//----------------------------------------------------------------------------------------------------------------------

void Connection::onConnectionUpdate()
{
  if(false == isValid()) return;

  if(true == isConnected())
  {
     emit connectionTimeChanged(m_ConnectionDuration.elapsed());
  }
}
//----------------------------------------------------------------------------------------------------------------------

void Connection::onDataReceived()
{
  if(nullptr == m_SerialPort) return;

  //append raw bytes
  m_ReceiveBuffer += m_SerialPort->readAll();

  const auto processedBytes = parseSmlData(m_ReceiveBuffer);

  //we can remove the bytes we have already parsed, this allows also for partial messages to be received
  if(0 < processedBytes)
  {
    #ifdef QT_DEBUG
    qDebug() << "Connection::onDataReceived() consumed" << processedBytes << "bytes";
    #endif
    m_ReceiveBuffer.remove(0, processedBytes);
  }
}
//----------------------------------------------------------------------------------------------------------------------

int Connection::parseSmlData(const QByteArray &message)
{
  const QByteArray cbaMessageStart = QByteArray::fromHex("1B1B1B1B01010101");
  const QByteArray cbaMessageEnd = QByteArray::fromHex("1B1B1B1B1A");

  int lastEndIndex{};

  int newStartIndex = message.indexOf(cbaMessageStart, lastEndIndex);
  int newEndIndex = message.indexOf(cbaMessageEnd, newStartIndex + cbaMessageStart.size());

  //nothing to consume
  if((0 > newStartIndex) || (0 > newEndIndex)) return 0;

  #ifdef QT_DEBUG
  qDebug() << "Connection::onDataReceived() received=" << message.size() << "bytes";
  #endif
  while((0 <= newStartIndex) && (0 < newEndIndex))
  {
    const qint64 timestamp  = QDateTime::currentMSecsSinceEpoch();

    #ifdef QT_DEBUG
    qDebug() << "Connection::onDataReceived() frame from=" << newStartIndex << " to=" << newEndIndex;
    #endif
    //skip the start and end bytes
    QByteArray data = message.mid(newStartIndex + cbaMessageStart.size(),
                                  newEndIndex - (newStartIndex + cbaMessageStart.size()));

    //the new message ends after the cbaMessageEnd bytes
    lastEndIndex = newEndIndex + cbaMessageEnd.size();

    #ifdef QT_DEBUG
    qDebug() << "Connection::parseSmlData() raw message=" << QString(data.toHex());
    #endif
    //the buffer contains the whole message, with transport escape sequences, these escape sequences are stripped here
    sml_file* file = sml_file_parse(reinterpret_cast<unsigned char*>(const_cast<char*>(data.data())), data.size());
    //we simply pretend we parsed nothing when we cannot parse the received file
    if(nullptr == file) return {};

    //the sml file is parsed now
    for(int i = 0; i < file->messages_len; i++)
    {
      sml_message *message = file->messages[i];

      if (*message->message_body->tag == SML_MESSAGE_GET_LIST_RESPONSE)
      {
        sml_get_list_response *body = reinterpret_cast<sml_get_list_response*>(message->message_body->data);

        for(sml_list *entry = body->val_list; entry != NULL; entry = entry->next)
        {
          // do not crash on null value
          if(nullptr == entry->value)
          {
            qCritical() << "Connection::parseSmlData() error in data stream, entry->value should not be a nullptr, "
                           << "skipping";
            continue;
          }

          const QString obisValue = QString("%1-%2:%3.%4.%5*%6").arg(entry->obj_name->str[0])
                                                                .arg(entry->obj_name->str[1])
                                                                .arg(entry->obj_name->str[2])
                                                                .arg(entry->obj_name->str[3])
                                                                .arg(entry->obj_name->str[4])
                                                                .arg(entry->obj_name->str[5]);

          QVariant value;

          if (entry->value->type == SML_TYPE_OCTET_STRING)
          {
            char *str;
            value = QVariant::fromValue(QString(sml_value_to_strhex(entry->value, &str, true)));

            free(str);
          }
          else if (entry->value->type == SML_TYPE_BOOLEAN)
          {
            value = QVariant::fromValue(*entry->value->data.boolean ? true : false);
          }
          else if (((entry->value->type & SML_TYPE_FIELD) == SML_TYPE_INTEGER) ||
                   ((entry->value->type & SML_TYPE_FIELD) == SML_TYPE_UNSIGNED))
          {
            bool ok{};
            double doubleValue{};

            {
              double value = sml_value_to_double(entry->value);
              int scaler = (nullptr != entry->scaler) ? *entry->scaler : 0;

              int prec = -scaler;
              if (prec < 0) prec = 0;

              value = value * qPow(10, scaler);
              QString doubleString = QString::asprintf("%.*f", prec, value);

              doubleValue = doubleString.toDouble(&ok);
            }

            if(true == ok) value = QVariant::fromValue(doubleValue);
          }

          m_ObisValueMapping[obisValue].append(timestamp, value);
          #ifdef QT_DEBUG
          qDebug() << "Connection::parseSmlData() found obisValue=" << obisValue << " timestamp=" << timestamp
                   << " value=" << value;
          #endif

          emit dataValueReceived(obisValue, timestamp, value);
        }
      }
    }

    newStartIndex = message.indexOf(cbaMessageStart, lastEndIndex);
    newEndIndex = message.indexOf(cbaMessageEnd, newStartIndex + cbaMessageStart.size());

    // free the malloc'd memory
    sml_file_free(file);
  }

  //where we stopped reading
  return newEndIndex + cbaMessageEnd.size();
}
//----------------------------------------------------------------------------------------------------------------------

void Connection::setConnectionData(const ConnectionData &data)
{
  if(true == isConnected())
  {
    disconnect();
  }

  QList<ObisValueMapping> addedMapping;
  QList<ObisValueMapping> removedMapping;

  const auto toQSet = [](QStringList l) { return QSet<QString>(l.begin(), l.end()); };

  const auto existingObisNumbers = toQSet(m_ConnectionData.getMappingObisNumbers());
  const auto newObisNumbers = toQSet(data.getMappingObisNumbers());

  //we remove all numbers which are already in the existing set -> these are added
  auto addedObisNumbers = newObisNumbers;
  addedObisNumbers.subtract(existingObisNumbers);

  //we remove all numbers which are also in the new set -> the remaining are removed
  auto removedObisNumbers = existingObisNumbers;
  removedObisNumbers.subtract(newObisNumbers);

  for(const auto &addedObisNumber : addedObisNumbers)
  {
    const auto mapping = data.getMappingByObisNumber(addedObisNumber);
    if(false == mapping.isValid()) continue;

    addedMapping.append(mapping);
  }

  for(const auto &removedObisNumber : removedObisNumbers)
  {
    const auto mapping = m_ConnectionData.getMappingByObisNumber(removedObisNumber);
    if(false == mapping.isValid()) continue;

    removedMapping.append(mapping);
  }

  m_ConnectionData = data;

  for(const auto &mapping : addedMapping) emit mappingAdded(mapping);
  for(const auto &mapping : removedMapping) emit mappingRemoved(mapping);
}
//----------------------------------------------------------------------------------------------------------------------

bool Connection::connect()
{
  if(false == isValid()) return false;

  if(true == isConnected()) disconnect();

  m_SerialPort->setPortName(getSerialPortInfo().portName());
  m_SerialPort->setBaudRate(9600);
  m_SerialPort->setDataBits(QSerialPort::Data8);
  m_SerialPort->setParity(QSerialPort::NoParity);
  m_SerialPort->setStopBits(QSerialPort::OneStop);
  m_SerialPort->setFlowControl(QSerialPort::NoFlowControl);

  const bool connected = m_SerialPort->open(QIODevice::ReadWrite);

  if(true == connected)
  {
    m_ConnectionDuration.restart();
    emit connectionChanged(connected);
  }

  return connected;
}
//----------------------------------------------------------------------------------------------------------------------

}
