#ifndef TCP_SERVER_WIN_H
#define TCP_SERVER_WIN_H

#include <QDialog>
#include "tcp_server.h"

class CTcpServerWin : public QDialog
{
    Q_OBJECT
public:
    CTcpServerWin( CTcpServer* tcp_server_class);
    ~CTcpServerWin();

private slots:
    void timerEvent(QTimerEvent *event);
    void clickBtnOpen();
    void clickBtnClose();
    void clickBtnSend();
    void clickBtnClear();
    void closeEvent(QCloseEvent *event);
    void slot_clientChanged();
private:

    CTcpServer* m_object;
    int m_timeID;

    void initUI();
    void deleteUI();
};

#endif // TCP_SERVER_WIN_H
