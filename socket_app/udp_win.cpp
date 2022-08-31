#include "udp_win.h"
#include <qplaintextedit.h>
#include <QTextEdit>
#include <QLineEdit>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <QLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QTimer>
// 定时器刷新时间
#define TIME_UPDATE 1000

CUdpWin::CUdpWin(CUdp* udp_class)
{
    m_object = udp_class;
    m_timeID = -1;
    initUI();

}

CUdpWin::~CUdpWin()
{

}
/**
 * @brief CUdpWin::timerEvent 定时器显示接收消息
 * @param event
 */
void CUdpWin::timerEvent(QTimerEvent *event)
{
    /* 判断是哪个定时器 event->timerId() */
    QPlainTextEdit * edtRecv = this->findChild<QPlainTextEdit *>("edtRecv");
    QCheckBox * chk16Format = this->findChild<QCheckBox *>("chk16Format");
    QString data;

    if(m_object->isOpen()){
        if(0 < m_object->read(data, chk16Format->isChecked()))
        {
            edtRecv->appendPlainText(data);
        }
    } else {
        clickBtnClose();
    }
}
/**
 * @brief CUdpWin::clickBtnOpen 连接
 */
void CUdpWin::clickBtnOpen()
{
    QLineEdit * edtBindPort = this->findChild<QLineEdit *>("edtBindPort");
    QPushButton * btnOpen = this->findChild<QPushButton *>("btnOpen");
    QPushButton * btnClose = this->findChild<QPushButton *>("btnClose");
    QPushButton * btnSend = this->findChild<QPushButton *>("btnSend");


    if( 0 == m_object->open(edtBindPort->text().toInt()))
    {
        btnOpen->setEnabled(false);
        btnClose->setEnabled(true);
        btnSend->setEnabled(true);

        if(m_timeID == -1)
            m_timeID = startTimer(TIME_UPDATE);
    }
}
/**
 * @brief CUdpWin::clickBtnClose 断开
 */
void CUdpWin::clickBtnClose()
{
    QPushButton * btnOpen = this->findChild<QPushButton *>("btnOpen");
    QPushButton * btnClose = this->findChild<QPushButton *>("btnClose");
    QPushButton * btnSend = this->findChild<QPushButton *>("btnSend");

    if(m_timeID != -1)
    {
        killTimer(m_timeID);
        m_timeID = -1;
    }

    m_object->close();
    btnOpen->setEnabled(true);
    btnClose->setEnabled(false);
    btnSend->setEnabled(false);
}
/**
 * @brief CUdpWin::clickBtnSend 发送
 */
void CUdpWin::clickBtnSend()
{
    QLineEdit * edtIP = this->findChild<QLineEdit *>("edtIP");
    QLineEdit * edtPort = this->findChild<QLineEdit *>("edtPort");
    QPlainTextEdit * edtSend = this->findChild<QPlainTextEdit *>("edtSend");
    QCheckBox * chk16Format = this->findChild<QCheckBox *>("chk16Format");
    QString data = edtSend->toPlainText();
    m_object->send(edtIP->text(), edtPort->text().toInt(), data, chk16Format->isChecked());
}
/**
 * @brief CUdpWin::clickBtnClear 清空接收窗口
 */
void CUdpWin::clickBtnClear()
{
    QPlainTextEdit * edtRecv = this->findChild<QPlainTextEdit *>("edtRecv");
    edtRecv->clear();
}
/**
 * @brief CUdpWin::closeEvent 窗口关闭事件
 * @param event
 * @note 不断开网络
 */
void CUdpWin::closeEvent(QCloseEvent *event)
{
    if(m_timeID != -1)
    {
        killTimer(m_timeID);
        m_timeID = -1;
    }
    deleteUI();

    QWidget::closeEvent(event);
}
/**
 * @brief CUdpWin::initUI 初始化界面元素和布局
 */
void CUdpWin::initUI()
{
    // 布局
    QHBoxLayout *hLay=new QHBoxLayout(this);
    QVBoxLayout *vLay1=new QVBoxLayout(this);
    QVBoxLayout *vLay2=new QVBoxLayout(this);
    // 水平布局
    vLay2->setContentsMargins(0,0,0,0);
    hLay->addLayout(vLay1);
    hLay->addSpacing(10);
    hLay->addLayout(vLay2);
    // IP输入框
    QLineEdit* edtIP = new QLineEdit(this);
    edtIP->setObjectName(QString::fromUtf8("edtIP"));
    QRegExp rx("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    edtIP->setValidator(new QRegExpValidator(rx,this));
    // port输入框
    QLineEdit* edtPort = new QLineEdit(this);
    edtPort->setObjectName(QString::fromUtf8("edtPort"));
    // 绑定port输入框
    QLineEdit* edtBindPort = new QLineEdit(this);
    edtBindPort->setObjectName(QString::fromUtf8("edtBindPort"));
    // 复选框 16进制
    QCheckBox* chk16Format = new QCheckBox (QStringLiteral("16进制"), this);
    chk16Format->setObjectName(QString::fromUtf8("chk16Format"));
    // 按钮
    QPushButton* btnOpen = new QPushButton(QStringLiteral("打开"), this);
    btnOpen->setObjectName(QString::fromUtf8("btnOpen"));
    QPushButton* btnClose = new QPushButton(QStringLiteral("关闭"), this);
    btnClose->setObjectName(QString::fromUtf8("btnClose"));
    QPushButton* btnSend = new QPushButton(QStringLiteral("发送"), this);
    btnSend->setObjectName(QString::fromUtf8("btnSend"));
    QPushButton* btnClear = new QPushButton(QStringLiteral("清空"), this);
    btnClear->setObjectName(QString::fromUtf8("btnClear"));
    //btnClear->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    // 文本框
    QPlainTextEdit* edtRecv=new QPlainTextEdit(this);
    edtRecv->setObjectName(QString::fromUtf8("edtRecv"));
    edtRecv->setReadOnly(true);
    edtRecv->document()->setMaximumBlockCount(100);// 限制最大存储行数
    QPlainTextEdit* edtSend=new QPlainTextEdit(this);
    edtSend->setObjectName(QString::fromUtf8("edtSend"));
    // label
    QLabel* labRecv = new QLabel(QStringLiteral("接收窗口"), this);
    QLabel* labSend = new QLabel(QStringLiteral("发送窗口"), this);
    QLabel* labIP = new QLabel(QStringLiteral("IP"), this);
    QLabel* labPort = new QLabel(QStringLiteral("Port"), this);
    QLabel* labBindPort = new QLabel(QStringLiteral("BindPort"), this);
    // 垂直布局1
    vLay1->setSpacing(5);
    vLay1->setContentsMargins(0,0,0,0);
    vLay1->addWidget(labRecv);
    vLay1->addWidget(edtRecv);
    vLay1->addWidget(labSend);
    vLay1->addWidget(edtSend);
    // 垂直布局2
    vLay2->setSpacing(5);
    vLay2->setContentsMargins(0,0,0,0);
    vLay2->addWidget(labIP);
    vLay2->addWidget(edtIP);
    vLay2->addSpacing(10);
    vLay2->addWidget(labPort);
    vLay2->addWidget(edtPort);
    vLay2->addSpacing(10);
    vLay2->addWidget(labBindPort);
    vLay2->addWidget(edtBindPort);
    vLay2->addWidget(chk16Format);
    vLay2->addWidget(btnOpen);
    vLay2->addWidget(btnClose);
    vLay2->addWidget(btnSend);
    vLay2->addWidget(btnClear);
    vLay2->addStretch();

    setLayout(hLay); //设置为窗体的主布局
    // 添加槽

    connect(btnOpen, &QPushButton::clicked, this, &CUdpWin::clickBtnOpen);
    connect(btnClose, &QPushButton::clicked, this, &CUdpWin::clickBtnClose);
    connect(btnSend, &QPushButton::clicked, this, &CUdpWin::clickBtnSend);
    connect(btnClear, &QPushButton::clicked, this, &CUdpWin::clickBtnClear);

    // 状态设置
    if(m_object->isOpen()) {
        btnOpen->setEnabled(false);
        btnClose->setEnabled(true);
        btnSend->setEnabled(true);

        QString ip;
        quint16 port;
        // 获取当前设置
        if (0 == m_object->get(ip,port)) {
            //edtIP->setText(ip);
            edtBindPort->setText(QString::number(port)  );
        }

        if(m_timeID == -1)
            m_timeID = startTimer(TIME_UPDATE);

    } else {
        btnOpen->setEnabled(true);
        btnClose->setEnabled(false);
        btnSend->setEnabled(false);

    }
    return;
}
/**
 * @brief CUdpWin::deleteUI 清空界面元素和布局
 */
void CUdpWin::deleteUI()
{
    QList<QWidget *> widgets = this->findChildren<QWidget*>();

    foreach(QWidget* widget, widgets)
    {
        widget->deleteLater();
    }
}
