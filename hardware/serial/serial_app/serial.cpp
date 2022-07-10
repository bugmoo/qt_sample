#include "serial.h"
#include "serialwin.h"

#include <QSerialPortInfo>              // 提供系统中存在的串口的信息
#include <QTextCodec>

CSerial::CSerial()
{
    m_recvBuffer.clear();
}
/**
 * @brief serial::showWin
 */
void CSerial::showWin()
{
    serialwin* serialWin;
    serialWin = new serialwin(this);
    serialWin->setModal(true);
    serialWin->show();

}
/**
 * @brief CSerial::getPortsList
 * @return 所有串口的name
 */
QList<QString> CSerial::getPortsList()
{
    QList<QString> listPorts;
    // QSerialPortInfo查找所有串口
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        listPorts.append(info.portName());
    }
    return listPorts;
}
/**
 * @brief CSerial::getBaudRates
 * @return 支持的波特率列表
 */
QList<qint32> CSerial::getBaudRates()
{
    QList<qint32> listBaudRates;
    listBaudRates = QSerialPortInfo::standardBaudRates();
    return listBaudRates;
}
/**
 * @brief CSerial::open
 * @param name
 * @param baud_rate
 * @param data_bits
 * @param parity
 * - 0 无校验
 * - 1 偶校验
 * - 2 奇校验
 * @param stop_bits
 * - 0 1
 * - 1 1.5
 * - 2 2
 * @return
 * - -1 打开失败
 * - 0 打开成功
 */
int CSerial::open(const QString &name, const qint32 baud_rate, const qint32 data_bits, const qint32 parity, const qint32 stop_bits)
{
    SerialParam param = {name, baud_rate, data_bits, parity, stop_bits};

    return open(param);
}
/**
 * @brief CSerial::open
 * @param param
 * @return  -1 失败 0 成功
 */
int CSerial::open(const SerialParam &param)
{
    if(m_serial.isOpen())
    {
        m_serial.close();
        QObject::disconnect(&m_serial, 0, 0, 0);
    }
    //连接信号和槽
    QObject::connect(&m_serial, &QSerialPort::readyRead, this, &CSerial::serialReadyRead);
    //设置串口名
    m_serial.setPortName(param.name);
    //设置波特率
    m_serial.setBaudRate(param.baud_rate);
    //设置数据位数
    switch(param.data_bits)
    {
    case 0: m_serial.setDataBits(QSerialPort::Data5); break;
    case 1: m_serial.setDataBits(QSerialPort::Data6); break;
    case 2: m_serial.setDataBits(QSerialPort::Data7); break;
    case 3: m_serial.setDataBits(QSerialPort::Data8); break;
    default: break;
    }
    //设置奇偶校验
    switch(param.parity)
    {
    case 0: m_serial.setParity(QSerialPort::NoParity); break;
    case 1: m_serial.setParity(QSerialPort::EvenParity); break;
    case 2: m_serial.setParity(QSerialPort::OddParity); break;
    default: break;
    }
    //设置停止位
    switch(param.stop_bits)
    {
    case 0: m_serial.setStopBits(QSerialPort::OneStop); break;
    case 1: m_serial.setStopBits(QSerialPort::OneAndHalfStop); break;
    case 2: m_serial.setStopBits(QSerialPort::TwoStop); break;
    default: break;
    }
    //设置流控制
    m_serial.setFlowControl(QSerialPort::NoFlowControl);
    //打开串口
    if(!m_serial.open(QIODevice::ReadWrite))
    {
        return -1;
    }
    m_param = param;
    return 0;
}

/**
 * @brief CSerial::set
 * @param baud_rate
 * @param data_bits
 * @param parity
 * - 0 无校验
 * - 1 偶校验
 * - 2 奇校验
 * @param stop_bits
 * - 0 1
 * - 1 1.5
 * - 2 2
 * @return -1 失败 0 成功
 */
int CSerial::set(const qint32 baud_rate, const qint32 data_bits, const qint32 parity, const qint32 stop_bits)
{
    if(!m_serial.isOpen())
    {
        return -1;
    }

    SerialParam param = {m_param.name, baud_rate, data_bits, parity, stop_bits};

    return open(param);
}
/**
 * @brief CSerial::close
 */
void CSerial::close()
{
    //关闭串口
    m_serial.close();
}
/**
 * @brief CSerial::serialReadyRead
 */
void CSerial::serialReadyRead()
{
    QByteArray buffer = m_serial.readAll();
    m_recvBufMutex.lock();
    m_recvBuffer.append(buffer);
    m_recvBufMutex.unlock();
}


/**
 * @brief CSerial::serialSend
 * @param data 发送字符串
 * @param hex 发送字符串的表示形式
 * - true hex形式
 * - false ASCII形式
 * @return 已发送字节数
 * - -1 发生错误
 */
qint64 CSerial::serialSend(const QString &data, bool hex)
{
    if(!m_serial.isOpen())
    {
        return -1;
    }

    QByteArray sendData;

    if(true == hex)
    {
        sendData = QByteArray::fromHex (data.toLatin1().data());//按十六进制编码发送
    }
    else
    {
        sendData = data.toUtf8();
    }

    return m_serial.write(sendData);
}

/**
 * @brief CSerial::serialRead
 * @param data
 * @param hex 接收字符串的表示形式
 * - true hex形式
 * - false ASCII形式
 * @return data的长度
 */
int CSerial::serialRead(QString &data, bool hex)
{
    if(!m_serial.isOpen())
    {
        return -1;
    }

    int length = 0;
    data.clear();

    if(m_recvBuffer.length())
    {
        m_recvBufMutex.lock();
        QByteArray buffer = m_recvBuffer;
        m_recvBuffer.clear() ;
        m_recvBufMutex.unlock();

        if(true == hex)
        {
            if(!buffer.isEmpty())//如果非空说明有数据接收
            {   //转换成16进制大写
                QString str=buffer.toHex().data();
                str=str.toUpper();
                //一个16进制占4位，8位为一字节，所以每两位16进制空一格
                for(int i=0;i<str.length();i+=2)
                {
                    QString str_1 = str.mid (i,2);
                    data += str_1;
                    data += " ";
                }

            }
        }
        else
        {
            QTextCodec *tc = QTextCodec::codecForName("GBK");
            data = tc->toUnicode(buffer);
        }

        length = data.length();
        buffer.clear();
    }
    return length;
}

/**
 * @brief CSerial::serialRead
 * @param bytes
 * @return bytes 的长度
 */
int CSerial::serialRead(QByteArray &bytes)
{
    if(!m_serial.isOpen())
    {
        return -1;
    }

    bytes.clear();

    if(m_recvBuffer.length())
    {
        m_recvBufMutex.lock();
        QByteArray bytes = m_recvBuffer;
        m_recvBuffer.clear() ;
        m_recvBufMutex.unlock();
    }
    return bytes.length();
}

