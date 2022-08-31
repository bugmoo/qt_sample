#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "tcp_client.h"
#include "tcp_server.h"
#include "udp.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnTcpClientWin_clicked();

    void on_btnTcpServerWin_clicked();

    void on_btnUdpWin_clicked();

private:
    Ui::MainWindow *ui;
    CTcpClient m_tcpClient;
    CTcpServer m_tcpServer;
    CUdp m_udp;
};
#endif // MAINWINDOW_H
