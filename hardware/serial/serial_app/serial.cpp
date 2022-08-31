#include "serial.h"
#include "serialwin.h"

#include <QSerialPortInfo>              // 提供系统中存在的串口的信息
#include <QTextCodec>

CSerial::CSerial()
{
    m_rbuf.clear();
}

CSerial::~CSerial()
{
    close();
}
/**
 * @brief serial::showWin
 */
void CSerial::showWin()
{
    CSerialWin* win;
    win = new CSerialWin(this);
    win->setModal(false);
    win->setWindowTitle("serial");
    win->setAttribute(Qt::WA_DeleteOnClose);
    win->show();

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
    if(m_handle.isOpen())
    {
        close();
    }
    //连接信号和槽
    QObject::connect(&m_handle, &QSerialPort::readyRead, this, &CSerial::slotReadyRead);
    //设置串口名
    m_handle.setPortName(param.name);
    //设置波特率
    m_handle.setBaudRate(param.baud_rate);
    //设置数据位数
    switch(param.data_bits)
    {
    case 0: m_handle.setDataBits(QSerialPort::Data5); break;
    case 1: m_handle.setDataBits(QSerialPort::Data6); break;
    case 2: m_handle.setDataBits(QSerialPort::Data7); break;
    case 3: m_handle.setDataBits(QSerialPort::Data8); break;
    default: break;
    }
    //设置奇偶校验
    switch(param.parity)
    {
    case 0: m_handle.setParity(QSerialPort::NoParity); break;
    case 1: m_handle.setParity(QSerialPort::EvenParity); break;
    case 2: m_handle.setParity(QSerialPort::OddParity); break;
    default: break;
    }
    //设置停止位
    switch(param.stop_bits)
    {
    case 0: m_handle.setStopBits(QSerialPort::OneStop); break;
    case 1: m_handle.setStopBits(QSerialPort::OneAndHalfStop); break;
    case 2: m_handle.setStopBits(QSerialPort::TwoStop); break;
    default: break;
    }
    //设置流控制
    m_handle.setFlowControl(QSerialPort::NoFlowControl);
    //打开串口
    if(!m_handle.open(QIODevice::ReadWrite))
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
    if(!m_handle.isOpen())
    {
        return -1;
    }

    SerialParam param = {m_param.name, baud_rate, data_bits, parity, stop_bits};

    return open(param);
}
/**
 * @brief CSerial::get
 * @param baud_rate
 * @param data_bits
 * @param parity
 * @param stop_bits
 * @return
 * - -1 未连接
 * - 0 正常
 */
int CSerial::get(QString& name, qint32 &baud_rate, qint32 &data_bits, qint32 &parity, qint32 &stop_bits)
{
    if(m_handle.isOpen()) {
        name = m_param.name;
        baud_rate = m_param.baud_rate;
        data_bits = m_param.data_bits;
        parity = m_param.parity;
        stop_bits = m_param.stop_bits;
        return 0;
    } else {
        return -1;
    }
}
/**
 * @brief CSerial::close
 */
void CSerial::close()
{
    //关闭串口
    m_handle.close();
    QObject::disconnect(&m_handle, 0, 0, 0);
}
/**
 * @brief CSerial::serialReadyRead
 */
void CSerial::slotReadyRead()
{
    QByteArray buffer = m_handle.readAll();
    m_rbufMutex.lock();
    m_rbuf.append(buffer);
    m_rbufMutex.unlock();
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
qint64 CSerial::send(const QString &data, bool hex)
{
    if(!m_handle.isOpen())
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

    qint64 written = m_handle.write(sendData);
    m_handle.flush();
    return written;
}

/**
 * @brief CSerial::serialRead
 * @param data
 * @param hex 接收字符串的表示形式
 * - true hex形式
 * - false ASCII形式
 * @return data的长度
 */
int CSerial::read(QString &data, bool hex)
{
    if(!m_handle.isOpen())
    {
        return -1;
    }

    int length = 0;
    data.clear();

    if(m_rbuf.length())
    {
        m_rbufMutex.lock();
        QByteArray buffer = m_rbuf;
        m_rbuf.clear() ;
        m_rbufMutex.unlock();

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
int CSerial::read(QByteArray &bytes)
{
    if(!m_handle.isOpen())
    {
        return -1;
    }

    bytes.clear();

    if(m_rbuf.length())
    {
        m_rbufMutex.lock();
        QByteArray bytes = m_rbuf;
        m_rbuf.clear() ;
        m_rbufMutex.unlock();
    }
    return bytes.length();
}

bool CSerial::isOpen()
{
    return m_handle.isOpen();
}

