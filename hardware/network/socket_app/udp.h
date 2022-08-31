#ifndef UDP_H
#define UDP_H

#include <QObject>
#include <QMutex>
#include <QUdpSocket>


struct UdpParam {
    QString sip; // ip
    quint16 sport;  // 端口
};

class CUdp : public QObject
{
    Q_OBJECT
public:
    CUdp();
    ~CUdp();
    void showWin();
    int open(const quint16 port);
    void close();
    int read(QString &data, bool hex);
    int read(QByteArray &bytes);
    qint64 send(const QString &ip, const quint16 port, const QString &data, bool hex);
    bool isOpen();
    int get(QString &ip, quint16 &port);
private slots:
    void slot_readyRead();
private:
    QMutex m_rbufMutex;
    QUdpSocket m_handle;
    struct UdpParam m_param;
    QByteArray m_rbuf;
    bool m_bind;
};
/**
 * @brief The CSingleTcpClient class 单例
 */
class CSingleUdp : public CUdp
{
    Q_OBJECT
public:
    // serial();
    static CSingleUdp& instance()
    {
        static CSingleUdp qinstance;
        return qinstance;
    }

private:
    CSingleUdp(){}
    CSingleUdp(const CSingleUdp&){}
    CSingleUdp& operator==(const CSingleUdp&){}
};

#endif // UDP_H
