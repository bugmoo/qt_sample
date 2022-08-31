#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <QObject>
#include <QMutex>
#include <QTcpSocket> // 用于连接
#include <QTcpServer> // 用于监听

/**
 * @brief The QTcpSocketPlus class
 * @note 重写QTcpSocket，使修改readyread等信号，支持公布socketDescriptor和socket句柄
 */
class QTcpSocketPlus : public QTcpSocket
{
    Q_OBJECT
public:
    QTcpSocketPlus(QObject *parent = nullptr) {
        connect(this, &QTcpSocketPlus::readyRead, [this](){
            while (this->bytesAvailable() > 0)
            {
                //int length = this->bytesAvailable();
                emit signal_readyRead(this->socketDescriptor(),this);
            }
        });

        connect(this, &QTcpSocketPlus::disconnected, [this](){
            // 由于该信号是在disconnect之后发送的，以下结果未可知。
            qDebug() << this->socketDescriptor();// -1 ，已经销毁
            emit signal_disconnected(descriptor,this);
        });
        connect(this, &QTcpSocketPlus::stateChanged,[this](QAbstractSocket::SocketState state){
            // 该信号应在disconnect之前。
            if (state == QAbstractSocket::UnconnectedState) {
                //如果当前客户端断开或无效客户端时,关闭当前客户端并从list中删除
                qDebug() << "IP:" << this->peerAddress().toString().trimmed()
                         << "port:" << this->peerPort() << "exit";
                this->close();//触发disconnected事件
                //                qDebug() << descriptor<<this->socketDescriptor();
                //                // this->socketDescriptor() 可能获取到-1 ，已经销毁
                //                emit signal_unconnectedState(descriptor,this);// disconnected 事件二选一进行通知
            }
        });
    }

signals:
    void signal_readyRead(qintptr, QObject*);
    void signal_disconnected(qintptr, QObject*);
    void signal_unconnectedState(qintptr, QObject*);
public:
    qintptr descriptor;
    QString ip;
    quint16 port;
};

/**
 * @brief The QTcpServerPlus
 * @note 重写QTcpServer，增加incomingConnection的对外信号，使调用者可以获取连接的socketDescriptor，和socket句柄
 */
class QTcpServerPlus : public QTcpServer{
    Q_OBJECT
protected:
    void incomingConnection(qintptr socketDescriptor){
        // 采用自定义QTcpSocket 绑定 描述符
        QTcpSocketPlus *socket = new QTcpSocketPlus();
        // 设置本 socket 唯一标识符
        socket->setSocketDescriptor(socketDescriptor);
        // 保存信息，以免在disconnect时查不到信息。
        socket->ip = socket->peerAddress().toString().trimmed();
        socket->port = socket->peerPort();
        socket->descriptor = socketDescriptor;
        emit signal_incomingConnection(socketDescriptor,socket);
    }
signals:
    void signal_incomingConnection(qintptr,QObject*);
public:
    QTcpServerPlus(QObject* parent){};
    ~QTcpServerPlus(){};
};

/**
 * @brief The TcpServerParam struct
 */
struct TcpServerParam {
    QString sip; // 服务器ip
    quint16 sport;  // 服务器端口
};
/**
 * @brief The TcpClinetInfo struct
 */
struct TcpClinetInfo {
    QTcpSocketPlus * socket;// 保存连接的客户端
    QByteArray rbuf; // 收到的信息
};

/**
 * @brief The CTcpServer class
 */
class CTcpServer : public QObject
{
    Q_OBJECT
public:
    CTcpServer();
    ~CTcpServer();
    void showWin();
    int open(const QString &ip, const quint16 port);
    int open(const TcpServerParam& param);
    void close();
    int read(qintptr socketDescriptor,QString &data, bool hex);
    int read(qintptr socketDescriptor,QByteArray &bytes);
    qint64 send(qintptr socketDescriptor,const QString &data, bool hex);
    bool isOpen();
    int get(QString &ip, quint16 &port);
    TcpClinetInfo* getClient(qintptr socketDescriptor);
    void closeAllClinet();
    QMap<qintptr, QString> getClientList();

private slots:
    void slot_incomingConnection(qintptr descriptor,QObject* socket);
    void slot_readyRead(qintptr descriptor,QObject* socket);
    void slot_disconnected(qintptr descriptor,QObject* socket);
    void slot_unconnectedState(qintptr descriptor,QObject* socket);


signals:
    void signal_clientChanged();
private:
    QMutex m_rbufMutex, m_mapMutex;
    QTcpServerPlus m_server; // 服务器
    struct TcpServerParam m_param;
public:
    QMap<qintptr, TcpClinetInfo> m_map; // 存放所有socket收到的数据,描述符作为键

};

#endif // TCP_SERVER_H
