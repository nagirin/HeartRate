#include "widget.h"
#include "ui_widget.h"

#define AppFontName "Microsoft YaHei"
#define AppFontSize 9

#define TextColor QColor(255,255,255)
#define Plot_NoColor QColor(0,0,0,0)

//曲线1的颜色
#define Plot2_DotColor QColor(236,110,0)
#define Plot2_LineColor QColor(246,98,0)
#define Plot2_BGColor QColor(246,98,0,80)

//曲线2的颜色
#define Plot2_DotColor_2 Qt::blue
#define Plot2_LineColor_2 Qt::blue
#define Plot2_BGColor_2 Qt::blue

#define TextWidth 1
#define LineWidth 2
#define DotWidth 5

//一个刻度里的小刻度数量--太小的话显示的时间会重叠
#define Plot2_Count 5
//Y轴最大范围值
#define Plot2_MaxY 4096

/*
 * 设置QT界面的样式
*/
void Widget::SetStyle(const QString &qssFile) {
    QFile file(qssFile);
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        qApp->setStyleSheet(qss);
        QString PaletteColor = qss.mid(20,7);
        qApp->setPalette(QPalette(QColor(PaletteColor)));
        file.close();
    }
    else
    {
        qApp->setStyleSheet("");
    }
}

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    /*服务器线程*/
    //开始信号
    connect(this,SIGNAL(StartServerThread()),&tcp_server_class,SLOT(run()));
    //日志信号
    connect(&tcp_server_class,SIGNAL(LogSend(QString)),this,SLOT(Log_Display(QString)));

    //***连接到客户端时按钮亮
    connect(&tcp_server_class,&TCPServer::Tcpconnection,this,&Widget::button_light);
    connect(&tcp_server_class,&TCPServer::Tcpdisconnection,this,&Widget::button_dark);

    //移动到线程
    tcp_server_class.moveToThread(&tcp_server_thread);
    tcp_server_thread.start(); //启动线程
    StartServerThread(); //创建服务器

    this->setWindowTitle("健康监控管家");

    //波形图界面初始化
    initForm();
    initPlot();
    initPlot2();
    loadPlot2();
    SetStyle(":/blue.css");

    //***
    //SetStyle(":/black.css");

    //***改变按钮的颜色***
    ui->toolButton_server_stat->setStyleSheet("color:#386487");


    //开始加载数据
    plot2_timer->start(20);
}

Widget::~Widget()
{
    delete ui;
}

//日志显示
void Widget::Log_Display(QString text)
{
    QPlainTextEdit *plainTextEdit_log=ui->plainTextEdit_log;
    //设置光标到文本末尾
    plainTextEdit_log->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
    //当文本数量超出一定范围就清除
    if(plainTextEdit_log->toPlainText().size()>1024*4)
    {
        plainTextEdit_log->clear();
    }
    plainTextEdit_log->insertPlainText(text);
    //移动滚动条到底部
    QScrollBar *scrollbar = plainTextEdit_log->verticalScrollBar();
    if(scrollbar)
    {
        scrollbar->setSliderPosition(scrollbar->maximum());
    }
}

//查看服务器状态
void Widget::on_toolButton_server_stat_clicked()
{
    QString text="服务器IP地址列表:\n";
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    for(int i=0;i<list.count();i++)
    {
        QHostAddress addr=list.at(i);
        if(addr.protocol() == QAbstractSocket::IPv4Protocol)
        {
            //***
            //qDebug()<<addr.toString();

          text+=addr.toString()+"\n";
        }
    }
    text+="服务器端口号:8080\n";
    if(ClientSocket)
    {
        if(ClientSocket->socketDescriptor()==-1)
        {
            text+="设备未连接\n";
        }
        else
        {
            text+="设备连接成功\n";
        }
    }
    else
    {
        text+="设备未连接\n";
    }
    //QMessageBox::about(this,"状态信息",text);
    QMessageBox::about(this,"本机信息",text);
}

//窗口关闭事件
void Widget::closeEvent(QCloseEvent *event)
{
    tcp_server_thread.quit();
    tcp_server_thread.wait();
}

void Widget::initForm()
{
    //初始化随机数种子
    QTime time = QTime::currentTime();
    qsrand(time.msec() + time.second() * 1000);

    //初始化动态曲线定时器
    plot2_timer = new QTimer(this);
    connect(plot2_timer, SIGNAL(timeout()), this, SLOT(loadPlot2()));

    plots.append(ui->plot2);
}

void Widget::initPlot()
{
    //设置纵坐标名称
    plots.at(0)->yAxis->setLabel("Pulse(%)");
    //设置纵坐标范围
    plots.at(0)->yAxis->setRange(0, Plot2_MaxY);
    //设置支持鼠标移动缩放波形界面
    plots.at(0)->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    //设置背景颜色
#if 1
    foreach (QCustomPlot *plot, plots)
    {
        //设置字体大小
        QFont font = QFont(AppFontName, AppFontSize - 2);
        plot->legend->setFont(font);
        plot->xAxis->setLabelFont(font);
        plot->yAxis->setLabelFont(font);
        plot->xAxis->setTickLabelFont(font);
        plot->yAxis->setTickLabelFont(font);

        //设置坐标颜色/坐标名称颜色
        plot->yAxis->setLabelColor(TextColor);
        plot->xAxis->setTickLabelColor(TextColor);
        plot->yAxis->setTickLabelColor(TextColor);
        plot->xAxis->setBasePen(QPen(TextColor, TextWidth));
        plot->yAxis->setBasePen(QPen(TextColor, TextWidth));
        plot->xAxis->setTickPen(QPen(TextColor, TextWidth));
        plot->yAxis->setTickPen(QPen(TextColor, TextWidth));
        plot->xAxis->setSubTickPen(QPen(TextColor, TextWidth));
        plot->yAxis->setSubTickPen(QPen(TextColor, TextWidth));

        //设置画布背景色
        QLinearGradient plotGradient;
        plotGradient.setStart(0, 0);
        plotGradient.setFinalStop(0, 350);
        plotGradient.setColorAt(0, QColor(80, 80, 80));
        plotGradient.setColorAt(1, QColor(50, 50, 50));
        plot->setBackground(plotGradient);

        //设置坐标背景色
        QLinearGradient axisRectGradient;
        axisRectGradient.setStart(0, 0);
        axisRectGradient.setFinalStop(0, 350);
        axisRectGradient.setColorAt(0, QColor(80, 80, 80));
        axisRectGradient.setColorAt(1, QColor(30, 30, 30));
        plot->axisRect()->setBackground(axisRectGradient);

        //设置图例提示位置及背景色
        plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);
        plot->legend->setBrush(QColor(255, 255, 255, 200));
        plot->replot();

    }
#endif
}


void Widget::initPlot2()
{
    plots.at(0)->addGraph();

    plots.at(0)->graph(0)->setName("CHI");
    plots.at(0)->graph(0)->setPen(QPen(Plot2_LineColor, LineWidth));
    plots.at(0)->graph(0)->setScatterStyle(
        QCPScatterStyle(QCPScatterStyle::ssCircle,
                        QPen(Plot2_DotColor, LineWidth),
                        QBrush(Plot2_DotColor), DotWidth));
    //设置动态曲线的横坐标格式及范围
    plots.at(0)->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    plots.at(0)->xAxis->setDateTimeFormat("HH:mm:ss");
    plots.at(0)->xAxis->setAutoTickStep(true);
    plots.at(0)->xAxis->setTickStep(0.5);
    plots.at(0)->xAxis->setRange(0, Plot2_Count, Qt::AlignRight);

    plots.at(0)->addGraph();//相当于添加一条新的曲线
    plots.at(0)->graph(1)->setName("GUAN");
    plots.at(0)->graph(1)->setPen(QPen(Plot2_LineColor_2, LineWidth));
    plots.at(0)->graph(1)->setScatterStyle(
        QCPScatterStyle(QCPScatterStyle::ssCircle,
                        QPen(Plot2_DotColor_2, LineWidth),
                        QBrush(Plot2_DotColor_2), DotWidth));

    //***设置上下轴、左右轴范围同步***
    //connect(plots.at(0)->yAxis, SIGNAL(rangeChanged(QCPRange)), plots.at(0)->yAxis2, SLOT(setRange(QCPRange)));
    //connect(plots.at(0)->xAxis, SIGNAL(rangeChanged(QCPRange)), plots.at(0)->xAxis2, SLOT(setRange(QCPRange)));

    //***调整当前plot的XY的显示范围使之恰好容纳本图层曲线
    //plots.at(0)->graph(0)->rescaleAxes();
    //plots.at(0)->graph(1)->rescaleAxes(true);

    //设置是否需要显示曲线的图例说明
    foreach (QCustomPlot *plot, plots)
    {
        plot->legend->setVisible(true);
        plot->replot();
    }


    //得到数据指针
    mData_0 = plots.at(0)->graph(0)->data();
    mData_1 = plots.at(0)->graph(1)->data();
}

void addToDataBuffer(QCPDataMap *mData,double x, double y)
{
    QCPData newData;

    newData.key = x;
    newData.value = y;

    mData->insert(x, newData);
}


//加载曲线数据
void Widget::loadPlot2()
{
    int i;
    bool flag=false;

    for(i=0;i<5;i++)
    {
        //得到秒单位的时间
        plot2_key = QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000.0;
        //心电数据1
        plot2_value=uart_queue_data.read_queueA();
        if(plot2_value>0)
        {
            flag=true;
            addToDataBuffer(mData_0,plot2_key,plot2_value);
        }

        //心电数据2
        plot2_value=uart_queue_data.read_queueB();
        if(plot2_value>0)
        {
            flag=true;
            addToDataBuffer(mData_1,plot2_key,plot2_value);
        }
    }
    if(flag)
    {
        //plots.at(0)->xAxis->setRange(plot2_key, Plot2_Count , Qt::AlignRight);

        //***x轴方向上动态移动
        //plots.at(0)->xAxis->setRange(mData_0->count()>100?(mData_0->count()-100):0 , mData_0->count());

        plots.at(0)->rescaleAxes(false);  //设置图表完全可见

        plots.at(0)->yAxis->setRangeLower(0);
        //plots.at(0)->replot();


        //***另一种绘制曲线的方式***
        plots.at(0)->replot(QCustomPlot::rpQueued);
    }

    /*
    A:心电数据1,B:新电数据2,C:运动步数,D:运动距离,E:体表温度
    例如: "A:1633215,B:1833215,C:45,D:28,E:66.55"
    */
}

void Widget::on_toolButton_src_data_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void Widget::on_toolButton_image_data_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void Widget::on_toolButton_clear_clicked()
{
    mData_0->clear();
    mData_1->clear();
}

void Widget::button_light()
{
    ui->toolButton_image_data->setStyleSheet("color:#386487");
    ui->toolButton_clear->setStyleSheet("color:#386487");
    ui->toolButton_src_data->setStyleSheet("color:#386487");

}


void Widget::button_dark()
{
    ui->toolButton_image_data->setStyleSheet("color:#CACAD0");
    ui->toolButton_clear->setStyleSheet("color:#CACAD0");
    ui->toolButton_src_data->setStyleSheet("color:#CACAD0");

}
