#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <QObject>
#include <QMutex>
#include <QTcpSocket>


struct TcpClientParam {
    QString sip; // 服务器ip
    quint16 sport;  // 服务器端口
};

class CTcpClient : public QObject
{
    Q_OBJECT
public:
    CTcpClient();
    ~CTcpClient();
    void showWin();
    int open(const QString &ip, const quint16 port);
    int open(const TcpClientParam& param);
    void close();
    int read(QString &data, bool hex);
    int read(QByteArray &bytes);
    qint64 send(const QString &data, bool hex);
    bool isOpen();
    int get(QString &ip, quint16 &port);
private slots:
    void slot_readyRead();
    void slot_disconnected();
    void slot_error(QAbstractSocket::SocketError);
private:
    QMutex m_rbufMutex;
    QTcpSocket m_handle;
    struct TcpClientParam m_param;
    QByteArray m_rbuf;
};
/**
 * @brief The CSingleTcpClient class 单例
 */
class CSingleTcpClient : public CTcpClient
{
    Q_OBJECT
public:
    // serial();
    static CSingleTcpClient& instance()
    {
        static CSingleTcpClient qinstance;
        return qinstance;
    }

private:
    CSingleTcpClient(){}
    CSingleTcpClient(const CSingleTcpClient&){}
    CSingleTcpClient& operator==(const CSingleTcpClient&){}
};
#endif // TCP_CLIENT_H
