#ifndef SERIALWIN_H
#define SERIALWIN_H

#include <QDialog>
#include "serial.h"

class serialwin : public QDialog
{
    Q_OBJECT
private:
    CSerial* m_serialClass;
    int m_timeID;

    void initUI();
    void deleteUI();
public:
    serialwin(CSerial* serial_class);
    ~serialwin();

private slots:
    void timerEvent(QTimerEvent *event);
    void clickBtnOpen();
    void clickBtnClose();
    void clickBtnSend();
    void clickBtnClear();
    void closeEvent(QCloseEvent *event);
};

#endif // SERIALWIN_H
