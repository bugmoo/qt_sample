#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btnTcpClientWin_clicked()
{
    m_tcpClient.showWin();
}


void MainWindow::on_btnTcpServerWin_clicked()
{
    m_tcpServer.showWin();
}


void MainWindow::on_btnUdpWin_clicked()
{
    m_udp.showWin();
}

