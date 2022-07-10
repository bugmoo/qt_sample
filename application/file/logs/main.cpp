#include <QCoreApplication>
#include <logmessage.h>
#include <QTextStream>
#include <iostream>

// 要显示出console窗口 ， 项目 -> build&run -> run 设置中勾选 run in terminal
int main(int argc, char *argv[])
{
    QTextStream qin(stdin);
    QTextStream qout(stdout);
    QCoreApplication a(argc, argv);

    qout << QStringLiteral("输入输出log路径:")<<Qt::endl;// endl 结束，或者控制台不显示
    QString  str;
    qin >> str ;
    // qout << str <<Qt::endl;

    //    std::string cstr;
    //    std::cout << "输入log路径:\r\n";
    //    std::cin >> cstr;
    //    std::cout << cstr << "\r\n";

    qDebug("This is a debug message.");
    // 重定向消息输出
    CLogMessage::install(str);
    qWarning("This is a warning message.");
    qCritical("This is a critical message.");
    // qFatal("This is a fatal message.");// 会导致程序提前退出
    // 还原消息输出
    CLogMessage::uninstall();
    qInfo("This is a info message.");

    return a.exec();
}
