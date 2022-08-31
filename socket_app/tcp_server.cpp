#include "tcp_server.h"
#include "tcp_server_win.h"
#include <QTextCodec>

CTcpServer::CTcpServer():m_server(this)
{
    m_map.clear();
}

CTcpServer::~CTcpServer()
{
    close();
}
/**
 * @brief CTcpServer::showWin 显示内置界面
 */
void CTcpServer::showWin()
{
        CTcpServerWin* win;
        win = new CTcpServerWin(this);
        win->setModal(false);
        win->setWindowTitle("tcp server");
        win->setAttribute(Qt::WA_DeleteOnClose);
        win->show();
        qDebug()<< "tcp server win show!";
}
/**
 * @brief CTcpClient::open
 * @param s_ip
 * @param port
 * @return
 * - -1 打开失败
 * - 0 打开成功
 */
int CTcpServer::open(const QString &s_ip, const quint16 port)
{
    TcpServerParam param = {s_ip, port};
    return open(param);
}
/**
 * @brief CTcpServer::open
 * @param param
 * @return -1 失败 0 成功
 */
int CTcpServer::open(const TcpServerParam &param)
{
    // 重启服务器？全部关闭
    close();

    if (m_server.listen(QHostAddress::Any, param.sport)) {
        qDebug() << "当前网络监听成功......!";
    }
    else {
        qDebug() << m_server.errorString();
        QObject::disconnect(&m_server, 0, 0, 0);
        return -1;
    }

    QObject::connect(&m_server, &QTcpServerPlus::signal_incomingConnection, this, &CTcpServer::slot_incomingConnection);

    m_param = param;
    qDebug() << "Server listenning!";
    return 0;

}
/**
 * @brief CTcpServer::close
 */
void CTcpServer::close()
{
    if(m_server.isListening()) {

        QObject::disconnect(&m_server, 0, 0, 0);// 防止新连接
        closeAllClinet();
        m_server.close();
    }
}

/**
 * @brief CTcpServer::send
 * @param data 发送字符串
 * @param hex 发送字符串的表示形式
 * - true hex形式
 * - false ASCII形式
 * @return 已发送字节数
 * - -1 发生错误
 */
qint64 CTcpServer::send(qintptr socketDescriptor, const QString &data, bool hex)
{
    TcpClinetInfo* info = getClient(socketDescriptor);
    if (NULL == info)
        return -1;
    QTcpSocketPlus * socket = info->socket;
    if(!socket->isOpen())
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

    qint64 written = socket->write(sendData);
    socket->flush();

    return written;
}
/**
 * @brief CTcpServer::isOpen 查看服务器是否打开
 * @return
 */
bool CTcpServer::isOpen()
{
    return m_server.isListening();
}
/**
 * @brief CTcpServer::get 获取服务器的ip 端口
 * @param ip
 * @param port
 * @return
 * - -1 未连接
 * - 0 正常
 */
int CTcpServer::get(QString &ip, quint16 &port)
{
    if(m_server.isListening()) {
        ip = m_param.sip;
        port = m_param.sport;
        return 0;
    } else {
        return -1;
    }
}
/**
 * @brief CTcpServer::getClient 通过描述符获取 保存在map中的客户端info结构体
 * @param socketDescriptor
 * @return
 */
TcpClinetInfo* CTcpServer::getClient(qintptr socketDescriptor)
{
    //    QMutableMapIterator<qintptr, TcpClinetInfo> j(m_map);//创建可读可写迭代器
    //    if(j.findNext(socketDescriptor))
    //    {
    //        j.setValue("010");
    //    }
    QMap<qintptr, TcpClinetInfo>::iterator it = m_map.find(socketDescriptor);
    if (it != m_map.end() && it.key() == socketDescriptor) {
        //return it.operator->();//得到value的指针
        return &it.value(); // 得到的是value的引用
    }
    else
        return NULL;
}

/**
 * @brief CTcpServer::read
 * @param data
 * @param hex 接收字符串的表示形式
 * - true hex形式
 * - false ASCII形式
 * @return data的长度
 */
int CTcpServer::read(qintptr socketDescriptor, QString &data, bool hex)
{
    TcpClinetInfo* info = getClient(socketDescriptor);
    if (NULL == info)
        return -1;
    QTcpSocketPlus * socket = info->socket;
    QByteArray* buf = &info->rbuf;

    if(!socket->isOpen())
    {
        return -1;
    }

    int length = 0;
    data.clear();

    if(buf->length())
    {
        m_rbufMutex.lock();
        QByteArray buffer = *buf;
        buf->clear() ;
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
 * @brief CTcpServer::read
 * @param bytes
 * @return bytes 的长度
 */
int CTcpServer::read(qintptr socketDescriptor, QByteArray &bytes)
{
    TcpClinetInfo* info = getClient(socketDescriptor);
    if (NULL == info)
        return -1;
    QTcpSocketPlus * socket = info->socket;
    QByteArray* buf = &info->rbuf;


    if(!socket->isOpen())
    {
        return -1;
    }

    bytes.clear();

    if(buf->length())
    {
        m_rbufMutex.lock();
        QByteArray bytes = *buf;
        buf->clear() ;
        m_rbufMutex.unlock();
    }
    return bytes.length();
}

/**
 * @brief CTcpServer::slot_incomingConnection
 * @param descriptor
 * @param socket
 */
void CTcpServer::slot_incomingConnection(qintptr descriptor,QObject* socket)
{
    if(NULL == getClient(descriptor)) {
        TcpClinetInfo info = {(QTcpSocketPlus*)socket, ""};
        m_map.insert(descriptor, info);
        QObject::connect((QTcpSocketPlus*)socket, &QTcpSocketPlus::signal_readyRead, this, &CTcpServer::slot_readyRead);
        QObject::connect((QTcpSocketPlus*)socket, &QTcpSocketPlus::signal_disconnected, this, &CTcpServer::slot_disconnected);
        QObject::connect((QTcpSocketPlus*)socket, &QTcpSocketPlus::signal_unconnectedState, this, &CTcpServer::slot_unconnectedState);

        emit signal_clientChanged();
    } else {
        qDebug()<<descriptor<<" already exist!";
    }
}
/**
 * @brief CTcpServer::slot_readyRead
 * @param descriptor
 * @param socket
 */
void CTcpServer::slot_readyRead(qintptr descriptor, QObject *socket)
{
    TcpClinetInfo* info;
    if(NULL != (info = getClient(descriptor))) {
        QByteArray buffer = info->socket->readAll();
        m_rbufMutex.lock();
        info->rbuf.append(buffer);
        m_rbufMutex.unlock();
    } else {
        qDebug()<<descriptor<<" readyread error!";
    }
}
/**
 * @brief CTcpServer::slot_disconnected
 * @param descriptor
 * @param socket
 */
void CTcpServer::slot_disconnected(qintptr descriptor, QObject *socket)
{
    qDebug()<<descriptor<<" laji has gone!";
    int num = m_map.remove(descriptor);
    qDebug()<<descriptor<<" we removed it !"<< num;
    emit signal_clientChanged();
}
/**
 * @brief CTcpServer::slot_unconnectedState
 * @param descriptor 描述符
 * @param socket 客户端套接字
 */
void CTcpServer::slot_unconnectedState(qintptr descriptor, QObject *socket)
{
    int num = m_map.remove(descriptor);
    qDebug()<<descriptor<<" we removed it !"<< num;
    emit signal_clientChanged();
}
/**
 * @brief CTcpServer::closeAllClinet
 */
void CTcpServer::closeAllClinet()
{
    for(QMap<qintptr, TcpClinetInfo>::iterator it=m_map.begin();it!=m_map.end();it++)
    {
        //qDebug()<<" "<<it.key()<<" "<<it.value().rbuf;
        QObject::disconnect((it->socket), 0, 0, 0);// 防止close时进入disconnect消息槽
        it->socket->close();

    }
    m_map.clear();
    emit signal_clientChanged();
}
/**
 * @brief CTcpServer::getClientList
 * @return QMap<qintptr, QString>
 * - qintptr 是 描述符
 * - QString 是 IP:PORT
 */
QMap<qintptr, QString> CTcpServer::getClientList()
{
    QMap<qintptr, QString> map;

    for(QMap<qintptr, TcpClinetInfo>::iterator it=m_map.begin();it!=m_map.end();it++)
    {
        QString str = it->socket->peerAddress().toString()/*.trimmed()*/ +
                ":" + QString::number(it->socket->peerPort());
        map.insert(it.key(), str);

    }

    return map;
}

