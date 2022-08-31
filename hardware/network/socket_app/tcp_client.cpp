#include "tcp_client.h"
#include "tcp_client_win.h"
#include <QTextCodec>

CTcpClient::CTcpClient()
{
    m_rbuf.clear();
}

CTcpClient::~CTcpClient()
{
    close();
}

void CTcpClient::showWin()
{
    CTcpClientWin* win;
    win = new CTcpClientWin(this);
    win->setModal(false);
    win->setWindowTitle("tcp client");
    win->setAttribute(Qt::WA_DeleteOnClose);
    win->show();
}
/**
 * @brief CTcpClient::open
 * @param s_ip
 * @param port
 * @return
 * - -1 打开失败
 * - 0 打开成功
 */
int CTcpClient::open(const QString &s_ip, const quint16 port)
{
    TcpClientParam param = {s_ip, port};

    if(param.sip == "")
        param.sip = ("127.0.0.1");

    return open(param);
}
/**
 * @brief CTcpClient::open
 * @param param
 * @return -1 失败 0 成功
 */
int CTcpClient::open(const TcpClientParam &param)
{
    if(m_handle.isOpen())
        close();
    //取消已有的连接
    m_handle.abort();
    //连接信号槽
    QObject::connect(&m_handle, &QTcpSocket::readyRead, this, &CTcpClient::slot_readyRead);
    QObject::connect(&m_handle, &QTcpSocket::disconnected, this, &CTcpClient::slot_disconnected);
    QObject::connect(&m_handle, SIGNAL(error(QAbstractSocket::SocketError)),
                     this, SLOT(slotError(QAbstractSocket::SocketError)));

    //连接服务器
    m_handle.connectToHost(param.sip, param.sport);

    //等待连接成功
    if(!m_handle.waitForConnected(1000))
    {
        qDebug() << "Connection failed!";
        return -1;
    }
    m_param = param;
    qDebug() << "Connect successfully!";
    return 0;

}
/**
 * @brief CTcpClient::close
 */
void CTcpClient::close()
{
    //断开连接
    m_handle.disconnectFromHost();
    QObject::disconnect(&m_handle, 0, 0, 0);
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
qint64 CTcpClient::send(const QString &data, bool hex)
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

bool CTcpClient::isOpen()
{
    return m_handle.isOpen();
}
/**
 * @brief CTcpClient::get
 * @param ip
 * @param port
 * @return
 * - -1 未连接
 * - 0 正常
 */
int CTcpClient::get(QString &ip, quint16 &port)
{
    if(m_handle.isOpen()) {
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
int CTcpClient::read(QString &data, bool hex)
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
 * @brief CTcpClient::read
 * @param bytes
 * @return bytes 的长度
 */
int CTcpClient::read(QByteArray &bytes)
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
void CTcpClient::slot_readyRead()
{
    QByteArray buffer = m_handle.readAll();
    m_rbufMutex.lock();
    m_rbuf.append(buffer);
    m_rbufMutex.unlock();
}
/**
 * @brief CTcpClient::slotDisconnected
 */
void CTcpClient::slot_disconnected()
{
    //取消已有的连接
    m_handle.abort();
    qDebug() << "Disconnected!";
}

void CTcpClient::slot_error(QAbstractSocket::SocketError)
{
    //取消已有的连接
    m_handle.abort();
}
