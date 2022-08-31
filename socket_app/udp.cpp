#include "udp.h"
#include "udp_win.h"
#include <QTextCodec>

CUdp::CUdp()
{
    m_rbuf.clear();
    m_bind = 0;
}

CUdp::~CUdp()
{
    close();
}

void CUdp::showWin()
{
    CUdpWin* win;
    win = new CUdpWin(this);
    win->setModal(false);
    win->setWindowTitle("udp");
    win->setAttribute(Qt::WA_DeleteOnClose);
    win->show();
}

/**
 * @brief CTcpClient::open
 * @param param
 * @return -1 失败 0 成功
 */
int CUdp::open(const quint16 port)
{
    //绑定发送端，ShareAddress允许多接收端绑定端口
    m_handle.bind(port,QUdpSocket::ShareAddress);
    //当接收端接收到数据时，就会发送readRead信号
    connect(&m_handle,&QUdpSocket::readyRead,this,&CUdp::slot_readyRead);

    m_param.sport = port;
    m_bind = 1;
    qDebug() << "Bind successfully!";
    return 0;

}
/**
 * @brief CTcpClient::close
 */
void CUdp::close()
{
    //断开连接
    m_bind = 0;
    QObject::disconnect(&m_handle, 0, 0, 0);
    m_rbuf.clear();
}

/**
 * @brief CTcpClient::send
 * @param data 发送字符串
 * @param hex 发送字符串的表示形式
 * - true hex形式
 * - false ASCII形式
 * @return 已发送字节数
 * - -1 发生错误
 */
qint64 CUdp::send(const QString &ip, const quint16 port, const QString &data, bool hex)
{

    QByteArray sendData;

    if(true == hex)
    {
        sendData = QByteArray::fromHex (data.toLatin1().data());//按十六进制编码发送
    }
    else
    {
        sendData = data.toUtf8();
    }

    qint64 written = m_handle.writeDatagram(sendData.data(),
                                            sendData.size(),
                                            QHostAddress(ip),    //udp广播地址QHostAddress::Broadcast
                                            port);

    m_handle.flush();

    return written;
}
/**
 * @brief CUdp::isOpen
 * @return
 */
bool CUdp::isOpen()
{
    return m_bind;
}
/**
 * @brief CTcpClient::get
 * @param ip
 * @param port
 * @return
 * - -1 未连接
 * - 0 正常
 */
int CUdp::get(QString &ip, quint16 &port)
{
    if(isOpen()) {
        ip = m_param.sip;
        port = m_param.sport;
        return 0;
    } else {
        return -1;
    }
}

/**
 * @brief CTcpClient::read
 * @param data
 * @param hex 接收字符串的表示形式
 * - true hex形式
 * - false ASCII形式
 * @return data的长度
 */
int CUdp::read(QString &data, bool hex)
{
    if(!isOpen())
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
 * @brief CTcpClient::read
 * @param bytes
 * @return bytes 的长度
 */
int CUdp::read(QByteArray &bytes)
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

/**
 * @brief CTcpClient::slotReadyRead
 */
void CUdp::slot_readyRead()
{
    QByteArray buffer;
    m_rbufMutex.lock();

    //有未处理的数据报
    if(m_handle.hasPendingDatagrams()){
        //读取的数据报大小
        buffer.resize(m_handle.pendingDatagramSize());
        m_handle.readDatagram(buffer.data(),buffer.size());
    }

    m_rbuf.append(buffer);
    m_rbufMutex.unlock();
}

