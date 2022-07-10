#include "logmessage.h"

#include <QDateTime.h>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QTextCodec>

QString CLogMessage::m_logPath;
QMutex CLogMessage::m_mutex;

CLogMessage::CLogMessage()
{

}
/**
 * @brief LogMessage::install 重定向QT的消息输出
 * @param log_path 指定输出log，NULL改变输出形式
 */
void CLogMessage::install(QString log_path)
{
    m_mutex.lock();
    m_logPath = log_path;
    qInstallMessageHandler(logMessageOut);
    m_mutex.unlock();
}
/**
 * @brief LogMessage::uninstall。还原QT的消息输出
 */
void CLogMessage::uninstall()
{
    m_mutex.lock();
    qInstallMessageHandler(0);
    m_mutex.unlock();
}
/**
 * @brief LogMessage::logMessageOut
 * @param type
 * @param context
 * @param msg
 * @par 说明
 * @code
 * qDebug()//调试消息
 * qInfo()//信息消息
 * qWarning()//警告消息和可恢复的错误
 * qCritical()//关键错误和系统错误
 * qFatal()//致命错误
 * @endcode
 */
void CLogMessage::logMessageOut(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    m_mutex.lock();

    QByteArray localMsg = msg.toLocal8Bit();
    QString strMsg("");

    switch(type)
    {
    case QtDebugMsg:
        strMsg = QString("Debug");
        break;
    case QtWarningMsg:
        strMsg = QString("Warning");
        break;
    case QtCriticalMsg:
        strMsg = QString("Critical");
        break;
    case QtFatalMsg:
        strMsg = QString("Fatal");
        break;
    case QtInfoMsg:
        strMsg = QString("Info");
        break;
    }

    // 设置输出信息
    QString message = QString("[%1]:{%2} File:%3  Line:%4  Function:%5  DateTime:%6")
            .arg(strMsg).arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function)
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd")
                 );

    // 输出至文件
    QFile file(m_logPath);

    if(true == file.open(QIODevice::ReadWrite | QIODevice::Append))
    {
        QTextStream stream(&file);
        stream << message << "\r\n";
        file.flush();
        file.close();
    }
    else
    {
        // 转换到中文编码，用于控制台显示
        QTextCodec *codec = QTextCodec::codecForName("GBK");
        QByteArray encodedStr = codec->fromUnicode(message);

        fprintf(stderr, "%s",encodedStr.data());
    }
    // 模仿qFatal
    if(type == QtFatalMsg)
        abort();

    m_mutex.unlock();
}
