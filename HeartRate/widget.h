#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "server.h"
#include <QPlainTextEdit>
#include <QTextCursor>
#include <QScrollBar>
#include <QPalette>

#include "qcustomplot.h"
#include "smoothcurvecreator.h"


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void SetStyle(const QString &qssFile);
    QThread tcp_server_thread; //服务器工作线程
    TCPServer tcp_server_class;

    //绘制实时曲线
    QTimer *plot2_timer;
    QList <QCustomPlot *> plots;

    double plot2_key;
    unsigned int plot2_value;

    //曲线数据点缓冲区
    QCPDataMap *mData_0;
    QCPDataMap *mData_1;

signals:
    void StartServerThread(); //启动服务器线程
private slots:
    void Log_Display(QString text);
    void on_toolButton_server_stat_clicked();
    void initPlot2();
    void loadPlot2();
    void initPlot();
    void initForm();
    void on_toolButton_src_data_clicked();

    void on_toolButton_image_data_clicked();

    void on_toolButton_clear_clicked();

    //***
    void button_light();
    void button_dark();

protected:
    void closeEvent(QCloseEvent *event); //窗口关闭
private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
