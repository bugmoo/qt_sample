#include "serialwin.h"
#include <qplaintextedit.h>
#include <QTextEdit>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <QLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QTimer>

serialwin::serialwin(CSerial* serial_class)
{
    m_serialClass = serial_class;
    m_timeID = -1;
    initUI();
}

serialwin::~serialwin()
{
}

void serialwin::initUI()
{
//    // 网格布局
//    QGridLayout *gridLayout;
//    gridLayout = new QGridLayout(this);
//    gridLayout->setSpacing(6);
//    gridLayout->setContentsMargins(11, 11, 11, 11);//设置左侧、顶部、右侧和底部边距
//    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
//    gridLayout->setContentsMargins(0, 0, 0, 0);
//    gridLayout->addLayout(hLay,0,0);

    QHBoxLayout *hLay=new QHBoxLayout(this);
    QVBoxLayout *vLay1=new QVBoxLayout(this);
    QVBoxLayout *vLay2=new QVBoxLayout(this);
    // 水平布局
    vLay2->setContentsMargins(0,0,0,0);
    hLay->addLayout(vLay1);
    hLay->addSpacing(10);
    hLay->addLayout(vLay2);
    // 下拉框：串口号
    QComboBox* cbbPortName = new QComboBox(this);
    cbbPortName->setObjectName(QString::fromUtf8("cbbPortName"));
    cbbPortName->clear();
    // 添加项目
    foreach(const QString &info, m_serialClass->getPortsList())
    {
        cbbPortName->addItem(info);
    }
    // 下拉框：波特率
    QComboBox* cbbBaudRates = new QComboBox(this);
    cbbBaudRates->setObjectName(QString::fromUtf8("cbbBaudRates"));
    cbbBaudRates->clear();
    // 添加项目
    foreach(const qint32 &info, m_serialClass->getBaudRates())
    {
        cbbBaudRates->addItem(QString::number(info));
    }
    // 下拉框：数据位
    QComboBox* cbbDataBits = new QComboBox(this);
    cbbDataBits->setObjectName(QString::fromUtf8("cbbDataBits"));
    cbbDataBits->clear();
    // 添加项目
    cbbDataBits->addItem(QString::number(5));
    cbbDataBits->addItem(QString::number(6));
    cbbDataBits->addItem(QString::number(7));
    cbbDataBits->addItem(QString::number(8));
    // 下拉框：校验位
    QComboBox* cbbParitys = new QComboBox(this);
    cbbParitys->setObjectName(QString::fromUtf8("cbbParitys"));
    cbbParitys->clear();
    // 添加项目
    cbbParitys->addItem("none");
    cbbParitys->addItem("even");
    cbbParitys->addItem("odd");
    // 下拉框：校验位
    QComboBox* cbbStopBits = new QComboBox(this);
    cbbStopBits->setObjectName(QString::fromUtf8("cbbStopBits"));
    cbbStopBits->clear();
    // 添加项目
    cbbStopBits->addItem("1");
    cbbStopBits->addItem("1.5");
    cbbStopBits->addItem("2");
    // 复选框 16进制
    QCheckBox* chk16Format = new QCheckBox ("16进制", this);
    chk16Format->setObjectName(QString::fromUtf8("chk16Format"));
    // 按钮
    QPushButton* btnOpen = new QPushButton("打开", this);
    btnOpen->setObjectName(QString::fromUtf8("btnOpen"));
    QPushButton* btnClose = new QPushButton("关闭", this);
    btnClose->setObjectName(QString::fromUtf8("btnClose"));
    QPushButton* btnSend = new QPushButton("发送", this);
    btnSend->setObjectName(QString::fromUtf8("btnSend"));
    QPushButton* btnClear = new QPushButton("清空", this);
    btnClear->setObjectName(QString::fromUtf8("btnClear"));
    //btnClear->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    // 文本框
    QPlainTextEdit* edtRecv=new QPlainTextEdit(this);
    edtRecv->setObjectName(QString::fromUtf8("edtRecv"));
    edtRecv->setReadOnly(true);
    QPlainTextEdit* edtSend=new QPlainTextEdit(this);
    edtSend->setObjectName(QString::fromUtf8("edtSend"));
    // label
    QLabel* labRecv = new QLabel("接收窗口", this);
    QLabel* labSend = new QLabel("发送窗口", this);
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
    vLay2->addWidget(cbbPortName);
    vLay2->addSpacing(10);
    vLay2->addWidget(cbbBaudRates);
    vLay2->addWidget(cbbDataBits);
    vLay2->addWidget(cbbParitys);
    vLay2->addWidget(cbbStopBits);
    vLay2->addWidget(chk16Format);
    vLay2->addWidget(btnOpen);
    vLay2->addWidget(btnClose);
    vLay2->addWidget(btnSend);
    vLay2->addWidget(btnClear);
    vLay2->addStretch();

    setLayout(hLay); //设置为窗体的主布局
    // 添加槽

    connect(btnOpen, &QPushButton::clicked, this, &serialwin::clickBtnOpen);
    connect(btnClose, &QPushButton::clicked, this, &serialwin::clickBtnClose);
    connect(btnSend, &QPushButton::clicked, this, &serialwin::clickBtnSend);
    connect(btnClear, &QPushButton::clicked, this, &serialwin::clickBtnClear);

    // 状态设置
    btnClose->setEnabled(false);
    return;
}

void serialwin::deleteUI()
{
    QList<QWidget *> widgets = this->findChildren<QWidget*>();

    foreach(QWidget* widget, widgets)
    {
        widget->deleteLater();
    }
}
/**
 * @brief serialwin::timerEvent 利用定时器来接收串口消息
 * @param event
 */
void serialwin::timerEvent(QTimerEvent *event)
{
    /* 判断是哪个定时器 event->timerId() */
    QPlainTextEdit * edtRecv = this->findChild<QPlainTextEdit *>("edtRecv");
    QCheckBox * chk16Format = this->findChild<QCheckBox *>("chk16Format");
    QString data;

    if(0 < m_serialClass->serialRead(data, chk16Format->isChecked()))
    {
        edtRecv->appendPlainText(data);
    }
}
/**
 * @brief serialwin::clickBtnOpen 打开串口
 */
void serialwin::clickBtnOpen()
{
    QComboBox * cbbPortName = this->findChild<QComboBox *>("cbbPortName");
    QComboBox * cbbBaudRates = this->findChild<QComboBox *>("cbbBaudRates");
    QComboBox * cbbDataBits = this->findChild<QComboBox *>("cbbDataBits");
    QComboBox * cbbParitys = this->findChild<QComboBox *>("cbbParitys");
    QComboBox * cbbStopBits = this->findChild<QComboBox *>("cbbStopBits");
    QPushButton * btnOpen = this->findChild<QPushButton *>("btnOpen");
    QPushButton * btnClose = this->findChild<QPushButton *>("btnClose");


    if( 0 == m_serialClass->open(cbbPortName->currentText(), cbbBaudRates->currentText().toInt()
                        ,cbbDataBits->currentText().toInt(),cbbParitys->currentIndex()
                        ,cbbStopBits->currentIndex()))
    {
        btnOpen->setEnabled(false);
        btnClose->setEnabled(true);
        if(m_timeID == -1)
            m_timeID = startTimer(1000);
    }
}
/**
 * @brief serialwin::clickBtnClose 关闭串口
 */
void serialwin::clickBtnClose()
{
    QPushButton * btnOpen = this->findChild<QPushButton *>("btnOpen");
    QPushButton * btnClose = this->findChild<QPushButton *>("btnClose");
    if(m_timeID != -1)
    {
        killTimer(m_timeID);
        m_timeID = -1;
    }

    m_serialClass->close();
    btnOpen->setEnabled(true);
    btnClose->setEnabled(false);
}

void serialwin::clickBtnSend()
{
    QPlainTextEdit * edtSend = this->findChild<QPlainTextEdit *>("edtSend");
    QCheckBox * chk16Format = this->findChild<QCheckBox *>("chk16Format");
    QString data = edtSend->toPlainText();
    m_serialClass->serialSend(data, chk16Format->isChecked());
}

void serialwin::clickBtnClear()
{
    QPlainTextEdit * edtRecv = this->findChild<QPlainTextEdit *>("edtRecv");
    edtRecv->clear();
}

void serialwin::closeEvent(QCloseEvent *event)
{
    if(m_timeID != -1)
    {
        killTimer(m_timeID);
        m_timeID = -1;
    }
    deleteUI();

    QWidget::closeEvent(event);
}
