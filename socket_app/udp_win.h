#ifndef UDP_WIN_H
#define UDP_WIN_H

#include <QDialog>
#include "udp.h"

class CUdpWin : public QDialog
{
    Q_OBJECT
public:
    CUdpWin( CUdp* udp_class);
    ~CUdpWin();

private slots:
    void timerEvent(QTimerEvent *event);
    void clickBtnOpen();
    void clickBtnClose();
    void clickBtnSend();
    void clickBtnClear();
    void closeEvent(QCloseEvent *event);

private:

    CUdp* m_object;
    int m_timeID;

    void initUI();
    void deleteUI();
};

#endif // UDP_WIN_H
