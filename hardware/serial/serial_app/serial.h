﻿#ifndef SERIAL_H
#define SERIAL_H

#include <QObject>
                                        //
#include <QSerialPort>                  // 提供访问串口的功能
#include <QMutex>
/**
 * @brief The SerialParam struct
 */
struct SerialParam {
    QString name;
    qint32 baud_rate;
    qint32 data_bits;
    qint32 parity;
    qint32 stop_bits;
};

/**
 * @brief The CSerial class
 */
class CSerial : public QObject
{
    Q_OBJECT
public:
    CSerial();
    ~CSerial();
    void showWin();                     // 显示界面
    QList<QString> getPortsList();
    QList<qint32> getBaudRates();
    int open(const QString &name, const qint32 baud_rate, const qint32 data_bits, const qint32 parity, const qint32 stop_bits);
    int open(const SerialParam &param);
    int set( const qint32 baud_rate, const qint32 data_bits, const qint32 parity, const qint32 stop_bits);
    int get(QString& name, qint32& baud_rate, qint32& data_bits, qint32& parity, qint32& stop_bits);

    void close();
    qint64 send(const QString &data, bool hex);
    int read(QString &data, bool hex);
    int read(QByteArray &bytes);
    bool isOpen();
private slots:
    void slotReadyRead();private:
private:
    QSerialPort m_handle;
    struct SerialParam m_param;
    QByteArray m_rbuf;
    QMutex m_rbufMutex;
};
/**
 * @brief The CSingleSerial class 单例
 */
class CSingleSerial : public CSerial
{
    Q_OBJECT
public:
    // serial();
    static CSingleSerial& instance()    // 懒汉模式
    {
        static CSingleSerial qinstance;
        return qinstance;
    }

private:
    CSingleSerial(){}
    CSingleSerial(const CSingleSerial&){}
    CSingleSerial& operator==(const CSingleSerial&){}
};
#endif // SERIAL_H
