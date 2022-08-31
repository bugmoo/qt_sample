#ifndef SERIALWIN_H
#define SERIALWIN_H

#include <QDialog>
#include "serial.h"

class CSerialWin : public QDialog
{
    Q_OBJECT
private:
    CSerial* m_object;
    int m_timeID;

    void initUI();
    void deleteUI();
public:
    CSerialWin(CSerial* serial_class);
    ~CSerialWin();

private slots:
    void timerEvent(QTimerEvent *event);
    void clickBtnOpen();
    void clickBtnClose();
    void clickBtnSend();
    void clickBtnClear();
    void closeEvent(QCloseEvent *event);
};

#endif // SERIALWIN_H
