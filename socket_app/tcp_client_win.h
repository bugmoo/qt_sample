#ifndef TCP_CLIENT_WIN_H
#define TCP_CLIENT_WIN_H

#include <QDialog>
#include "tcp_client.h"

class CTcpClientWin : public QDialog
{
    Q_OBJECT
public:
    CTcpClientWin( CTcpClient* tcp_client_class);
    ~CTcpClientWin();

private slots:
    void timerEvent(QTimerEvent *event);
    void clickBtnOpen();
    void clickBtnClose();
    void clickBtnSend();
    void clickBtnClear();
    void closeEvent(QCloseEvent *event);

private:

    CTcpClient* m_object;
    int m_timeID;

    void initUI();
    void deleteUI();
};

#endif // TCP_CLIENT_WIN_H
