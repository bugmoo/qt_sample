#include "tcp_server_win.h"
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

CTcpServerWin::CTcpServerWin(CTcpServer* tcp_server_class)
{
    m_object = tcp_server_class;
    m_timeID = -1;
    initUI();

}

CTcpServerWin::~CTcpServerWin()
{

}
/**
 * @brief CTcpServerWin::timerEvent 定时器显示网络消息
 * @param event
 */
void CTcpServerWin::timerEvent(QTimerEvent *event)
{
    /* 判断是哪个定时器 event->timerId() */
    QPlainTextEdit * edtRecv = this->findChild<QPlainTextEdit *>("edtRecv");
    QCheckBox * chk16Format = this->findChild<QCheckBox *>("chk16Format");
    QString data;

    if(m_object->isOpen()){
        QComboBox* cbbClient = this->findChild<QComboBox *>("cbbClient");

        qintptr desc = cbbClient->currentData().toLongLong();
        if(0 < m_object->read(desc, data, chk16Format->isChecked()))
        {
            edtRecv->appendPlainText(data);
        }
    } else {
        clickBtnClose();
    }
}
/**
 * @brief CTcpServerWin::clickBtnOpen 连接
 */
void CTcpServerWin::clickBtnOpen()
{
    //QLineEdit * edtIP = this->findChild<QLineEdit *>("edtIP");
    QLineEdit * edtPort = this->findChild<QLineEdit *>("edtPort");
    QPushButton * btnOpen = this->findChild<QPushButton *>("btnOpen");
    QPushButton * btnClose = this->findChild<QPushButton *>("btnClose");
    QPushButton * btnSend = this->findChild<QPushButton *>("btnSend");


    if( 0 == m_object->open(/*edtIP->text()*/"", edtPort->text().toInt()))
    {
        btnOpen->setEnabled(false);
        btnClose->setEnabled(true);
        btnSend->setEnabled(true);

        if(m_timeID == -1)
            m_timeID = startTimer(TIME_UPDATE);
    }
}
/**
 * @brief CTcpServerWin::clickBtnClose 断开
 */
void CTcpServerWin::clickBtnClose()
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
 * @brief CTcpServerWin::clickBtnSend 发送
 */
void CTcpServerWin::clickBtnSend()
{
    QPlainTextEdit * edtSend = this->findChild<QPlainTextEdit *>("edtSend");
    QCheckBox * chk16Format = this->findChild<QCheckBox *>("chk16Format");
    QComboBox* cbbClient = this->findChild<QComboBox *>("cbbClient");

    QString data = edtSend->toPlainText();
    qintptr desc = cbbClient->currentData().toLongLong();
    m_object->send(desc, data, chk16Format->isChecked());
}
/**
 * @brief CTcpServerWin::clickBtnClear 清空接收窗口
 */
void CTcpServerWin::clickBtnClear()
{
    QPlainTextEdit * edtRecv = this->findChild<QPlainTextEdit *>("edtRecv");
    edtRecv->clear();
}
/**
 * @brief CTcpServerWin::closeEvent 窗口关闭事件
 * @param event
 * @note 不断开网络
 */
void CTcpServerWin::closeEvent(QCloseEvent *event)
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
 * @brief CTcpServerWin::slot_clientChanged 客户端连接数发生变化
 */
void CTcpServerWin::slot_clientChanged()
{
    // TODO BUG
    QComboBox* cbbClient = this->findChild<QComboBox *>("cbbClient");
    if(nullptr == cbbClient) {
        qDebug()<<"slot_clientChanged error occur";
        return;
    }
    //QString lastStr = cbbClient->currentText();
    int index = cbbClient->currentIndex();
    qDebug()<<"index"<<index;
    qintptr lastDesc;

    if(index!=-1)
        lastDesc= cbbClient->currentData().toLongLong();

    //if(index)
    cbbClient->clear();
    // 添加项目，并保存自定义项目
    QMap<qintptr, QString> map = m_object->getClientList();

    foreach(const qintptr desc, map.keys())
    {
        //        QVariant useVar;
        //        useVar.setValue(s1);
        cbbClient->addItem(map.value(desc), desc);
    }

    if(index!=-1) {
        index =  cbbClient->findData(lastDesc);
        cbbClient->setCurrentIndex(index);
    }
}
/**
 * @brief CTcpServerWin::initUI 初始化界面元素和布局
 */
void CTcpServerWin::initUI()
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
    //QLineEdit* edtIP = new QLineEdit(this);
    //edtIP->setObjectName(QString::fromUtf8("edtIP"));
    //QRegExp rx("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    //edtIP->setValidator(new QRegExpValidator(rx,this));
    // port输入框
    QLineEdit* edtPort = new QLineEdit(this);
    edtPort->setObjectName(QString::fromUtf8("edtPort"));
    // 复选框 16进制
    QCheckBox* chk16Format = new QCheckBox (QStringLiteral("16进制"), this);
    chk16Format->setObjectName(QString::fromUtf8("chk16Format"));
    // 按钮
    QPushButton* btnOpen = new QPushButton(QStringLiteral("打开"), this);
    btnOpen->setObjectName(QString::fromUtf8("btnOpen"));
    QPushButton* btnClose = new QPushButton(QStringLiteral("关闭"), this);
    btnClose->setObjectName(QString::fromUtf8("btnClose"));
    // 选择客户端
    QComboBox* cbbClient = new QComboBox(this);
    cbbClient->setObjectName(QString::fromUtf8("cbbClient"));
    //    cbbClient->clear();
    //    // 添加项目，并保存自定义项目
    //    QMap<qintptr, QString> map = m_object->getClientList();
    //    foreach(const qintptr desc, map.keys())
    //    {
    ////        QVariant useVar;
    ////        useVar.setValue(s1);
    //        cbbClient->addItem(map.value(desc), desc);
    //    }
    slot_clientChanged();

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
    //QLabel* labIP = new QLabel(QStringLiteral("IP"), this);
    QLabel* labPort = new QLabel(QStringLiteral("Port"), this);
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
    //vLay2->addWidget(labIP);
    //vLay2->addWidget(edtIP);
    vLay2->addSpacing(10);
    vLay2->addWidget(labPort);
    vLay2->addWidget(edtPort);
    vLay2->addWidget(chk16Format);
    vLay2->addWidget(btnOpen);
    vLay2->addWidget(btnClose);
    vLay2->addWidget(cbbClient);
    vLay2->addWidget(btnSend);
    vLay2->addWidget(btnClear);
    vLay2->addStretch();

    setLayout(hLay); //设置为窗体的主布局
    // 添加槽

    connect(btnOpen, &QPushButton::clicked, this, &CTcpServerWin::clickBtnOpen);
    connect(btnClose, &QPushButton::clicked, this, &CTcpServerWin::clickBtnClose);
    connect(btnSend, &QPushButton::clicked, this, &CTcpServerWin::clickBtnSend);
    connect(btnClear, &QPushButton::clicked, this, &CTcpServerWin::clickBtnClear);
    connect(m_object, &CTcpServer::signal_clientChanged, this , &CTcpServerWin::slot_clientChanged);

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
            edtPort->setText(QString::number(port)  );
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
 * @brief CTcpServerWin::deleteUI 清空界面元素和布局
 */
void CTcpServerWin::deleteUI()
{
    QObject::disconnect(m_object, 0, 0 , 0);// 防止下一次界面起来的时候产生消息槽响应
    QList<QWidget *> widgets = this->findChildren<QWidget*>();
    foreach(QWidget* widget, widgets)
    {
        widget->deleteLater();
    }
}
