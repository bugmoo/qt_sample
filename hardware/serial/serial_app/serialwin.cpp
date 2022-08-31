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
// 定时器刷新时间
#define TIME_UPDATE 1000

CSerialWin::CSerialWin(CSerial* serial_class)
{
    m_object = serial_class;
    m_timeID = -1;
    initUI();
}

CSerialWin::~CSerialWin()
{
}
/**
 * @brief CSerialWin::initUI 初始化界面元素和布局
 */
void CSerialWin::initUI()
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
    foreach(const QString &info, m_object->getPortsList())
    {
        cbbPortName->addItem(info);
    }
    // 下拉框：波特率
    QComboBox* cbbBaudRates = new QComboBox(this);
    cbbBaudRates->setObjectName(QString::fromUtf8("cbbBaudRates"));
    cbbBaudRates->clear();
    // 添加项目
    foreach(const qint32 &info, m_object->getBaudRates())
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

    connect(btnOpen, &QPushButton::clicked, this, &CSerialWin::clickBtnOpen);
    connect(btnClose, &QPushButton::clicked, this, &CSerialWin::clickBtnClose);
    connect(btnSend, &QPushButton::clicked, this, &CSerialWin::clickBtnSend);
    connect(btnClear, &QPushButton::clicked, this, &CSerialWin::clickBtnClear);


    // 状态设置
    if(m_object->isOpen()) {
        btnOpen->setEnabled(false);
        btnClose->setEnabled(true);
        btnSend->setEnabled(true);

        QString name;
        qint32 baudRate;
        qint32 dataBits;
        qint32 parity;
        qint32 stopBits;
        // 获取当前设置
        if(0 == m_object->get(name, baudRate, dataBits, parity, stopBits)) {
            cbbPortName->setCurrentText(name);
            cbbBaudRates->setCurrentText(QString::number(baudRate));
            cbbDataBits->setCurrentText(QString::number(dataBits));
            cbbParitys->setCurrentIndex(parity);
            cbbStopBits->setCurrentIndex(stopBits);
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
 * @brief CSerialWin::deleteUI 清空界面元素
 */
void CSerialWin::deleteUI()
{
    QList<QWidget *> widgets = this->findChildren<QWidget*>();

    foreach(QWidget* widget, widgets)
    {
        widget->deleteLater();
    }
}
/**
 * @brief serialwin::timerEvent 利用定时器来显示串口消息
 * @param event
 */
void CSerialWin::timerEvent(QTimerEvent *event)
{
    /* 判断是哪个定时器 event->timerId() */
    QPlainTextEdit * edtRecv = this->findChild<QPlainTextEdit *>("edtRecv");
    QCheckBox * chk16Format = this->findChild<QCheckBox *>("chk16Format");
    QString data;

    if(0 < m_object->read(data, chk16Format->isChecked()))
    {
        edtRecv->appendPlainText(data);
    }
}
/**
 * @brief serialwin::clickBtnOpen 打开串口
 */
void CSerialWin::clickBtnOpen()
{
    QComboBox * cbbPortName = this->findChild<QComboBox *>("cbbPortName");
    QComboBox * cbbBaudRates = this->findChild<QComboBox *>("cbbBaudRates");
    QComboBox * cbbDataBits = this->findChild<QComboBox *>("cbbDataBits");
    QComboBox * cbbParitys = this->findChild<QComboBox *>("cbbParitys");
    QComboBox * cbbStopBits = this->findChild<QComboBox *>("cbbStopBits");
    QPushButton * btnOpen = this->findChild<QPushButton *>("btnOpen");
    QPushButton * btnClose = this->findChild<QPushButton *>("btnClose");
    QPushButton * btnSend = this->findChild<QPushButton *>("btnSend");


    if( 0 == m_object->open(cbbPortName->currentText(), cbbBaudRates->currentText().toInt()
                            ,cbbDataBits->currentText().toInt(),cbbParitys->currentIndex()
                            ,cbbStopBits->currentIndex()))
    {
        btnOpen->setEnabled(false);
        btnClose->setEnabled(true);
        btnSend->setEnabled(true);

        if(m_timeID == -1)
            m_timeID = startTimer(TIME_UPDATE);
    }
}
/**
 * @brief serialwin::clickBtnClose 关闭串口
 */
void CSerialWin::clickBtnClose()
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
 * @brief CSerialWin::clickBtnSend 串口发送
 */
void CSerialWin::clickBtnSend()
{
    QPlainTextEdit * edtSend = this->findChild<QPlainTextEdit *>("edtSend");
    QCheckBox * chk16Format = this->findChild<QCheckBox *>("chk16Format");
    QString data = edtSend->toPlainText();
    m_object->send(data, chk16Format->isChecked());
}
/**
 * @brief CSerialWin::clickBtnClear 接收窗口清空
 */
void CSerialWin::clickBtnClear()
{
    QPlainTextEdit * edtRecv = this->findChild<QPlainTextEdit *>("edtRecv");
    edtRecv->clear();
}
/**
 * @brief CSerialWin::closeEvent 窗口关闭事件
 * @param event
 * @note 不关闭串口
 */
void CSerialWin::closeEvent(QCloseEvent *event)
{
    if(m_timeID != -1)
    {
        killTimer(m_timeID);
        m_timeID = -1;
    }
    deleteUI();

    QWidget::closeEvent(event);
}
