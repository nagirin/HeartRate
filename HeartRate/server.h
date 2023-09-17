#ifndef SERVER_H
#define SERVER_H
#include <QObject>
#include <QWidget>
#include <QTcpServer>
#include <QHostInfo>  //获取计算机网络信息
#include <QUdpSocket>
#include <QtNetwork>
#include <QHostInfo>
#include <QDebug>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>
#include <QMessageBox>
#include <QQueue>
#include <QThread>

extern "C"
{
    #include <stdlib.h>
}

extern QTcpSocket *ClientSocket;

//队列里最大存放的数据
#define QUEUE_DATA_CNT 100
class UART_DATA
{
private:
    QReadWriteLock lock1; //读写锁
    QReadWriteLock lock2; //读写锁
    QReadWriteLock lock3; //读写锁
    QReadWriteLock lock4; //读写锁
    QReadWriteLock lock5; //读写锁
    QQueue<unsigned int> queue1; //缓存数据的队列
    QQueue<unsigned int> queue2; //缓存数据的队列
    QQueue<unsigned int> queue3; //缓存数据的队列
    QQueue<unsigned int> queue4; //缓存数据的队列
    QQueue<double> queue5; //缓存数据的队列
public:
    UART_DATA()
    {
        queue1.clear();
        queue2.clear();
        queue3.clear();
        queue4.clear();
        queue5.clear();
    }

    //向队列里插入一条数据
    void write_queueA(unsigned int data)
    {
        lock1.lockForWrite();
        queue1.enqueue(data);
        if(queue1.size()>QUEUE_DATA_CNT) //最大缓存数据
        {
            queue1.clear();
        }
        lock1.unlock();
    }

    //返回队列里的一条数据
    unsigned int read_queueA()
    {
        unsigned int data=0;
        lock1.lockForWrite();
        //判断队列是否为空,有数据才取
        if(queue1.isEmpty()==false)
        {
            data=queue1.dequeue();
        }
        lock1.unlock();
        return data;
    }

    //返回队列里剩余的数据数量
    int get_queueA_cnt()
    {
        int data=-1;
        lock1.lockForWrite();
        data=queue1.count();
        lock1.unlock();
        return data;
    }


    //向队列里插入一条数据
    void write_queueB(unsigned int data)
    {
        lock2.lockForWrite();
        queue2.enqueue(data);
        if(queue2.size()>QUEUE_DATA_CNT) //最大缓存数据
        {
            queue2.clear();
        }
        lock2.unlock();
    }

    //返回队列里的一条数据
    unsigned int read_queueB()
    {
        unsigned int data=0;
        lock2.lockForWrite();
        //判断队列是否为空,有数据才取
        if(queue2.isEmpty()==false)
        {
            data=queue2.dequeue();
        }
        lock2.unlock();
        return data;
    }

    //返回队列里剩余的数据数量
    int get_queueB_cnt()
    {
        int data=-1;
        lock2.lockForWrite();
        data=queue2.count();
        lock2.unlock();
        return data;
    }

    //向队列里插入一条数据
    void write_queueC(unsigned int data)
    {
        lock3.lockForWrite();
        queue3.enqueue(data);
        if(queue3.size()>QUEUE_DATA_CNT) //最大缓存数据
        {
            queue3.clear();
        }
        lock3.unlock();
    }

    //返回队列里的一条数据
    unsigned int read_queueC()
    {
        unsigned int data=0;
        lock3.lockForWrite();
        //判断队列是否为空,有数据才取
        if(queue3.isEmpty()==false)
        {
            data=queue3.dequeue();
        }
        lock3.unlock();
        return data;
    }

    //返回队列里剩余的数据数量
    int get_queueC_cnt()
    {
        int data=-1;
        lock3.lockForWrite();
        data=queue3.count();
        lock3.unlock();
        return data;
    }

    //向队列里插入一条数据
    void write_queueD(unsigned int data)
    {
        lock4.lockForWrite();
        queue4.enqueue(data);
        if(queue4.size()>QUEUE_DATA_CNT) //最大缓存数据
        {
            queue4.clear();
        }
        lock4.unlock();
    }

    //返回队列里的一条数据
    unsigned int read_queueD()
    {
        unsigned int data=0;
        lock4.lockForWrite();
        //判断队列是否为空,有数据才取
        if(queue4.isEmpty()==false)
        {
            data=queue4.dequeue();
        }
        lock4.unlock();
        return data;
    }

    //返回队列里剩余的数据数量
    int get_queueD_cnt()
    {
        int data=-1;
        lock4.lockForWrite();
        data=queue4.count();
        lock4.unlock();
        return data;
    }

    //向队列里插入一条数据
    void write_queueE(double data)
    {
        lock5.lockForWrite();
        queue5.enqueue(data);
        if(queue5.size()>QUEUE_DATA_CNT) //最大缓存数据
        {
            queue5.clear();
        }
        lock5.unlock();
    }

    //返回队列里的一条数据
    float read_queueE()
    {
        float data=0;
        lock5.lockForWrite();
        //判断队列是否为空,有数据才取
        if(queue5.isEmpty()==false)
        {
            data=queue5.dequeue();
        }
        lock5.unlock();
        return data;
    }

    //返回队列里剩余的数据数量
    int get_queueE_cnt()
    {
        int data=-1;
        lock5.lockForWrite();
        data=queue5.count();
        lock5.unlock();
        return data;
    }
};
extern class UART_DATA uart_queue_data;


//用于连接服务器，接收服务器下发的命令
class TCPServer : public QObject
{
    Q_OBJECT
public:
    //关于服务器相关的变量定义
    QTcpServer *LocalTcpServer;
    TCPServer(QObject* parent=nullptr):QObject(parent){LocalTcpServer=nullptr;}
    void StringProcessing(const char *src_str);
public slots:
    void run();
    void NewTcpConnection();
    void TcpClientDisconnected();
    void ReadTcpClientData();
signals:
    void LogSend(QString text);
    void Tcpconnection();
    void Tcpdisconnection();
};

#endif // SERVER_H
