#include "server.h"
QTcpSocket *ClientSocket=nullptr;
class UART_DATA uart_queue_data;

//开始执行程序
void TCPServer::run()
{
    /*1. 实例化服务器*/
    LocalTcpServer= new QTcpServer;
    /*2. 设置监听的端口和IP地址*/
    LocalTcpServer->listen(QHostAddress::Any,8080);
    connect(LocalTcpServer,SIGNAL(newConnection()),this,SLOT(NewTcpConnection()));

    //***收到连接后按钮变亮
    connect(LocalTcpServer,&QTcpServer::newConnection,this,&TCPServer::Tcpconnection);

}

//服务器模式：响应新连接的客户端
void TCPServer::NewTcpConnection()
{
    /*创建本地服务器套接字*/
    if(ClientSocket)
    {
        ClientSocket->close();
        delete  ClientSocket;
        ClientSocket=nullptr;
    }
    //说明: 本服务器只支持同时与一个客户端通信.这里没有保存连接上来的客户端
    ClientSocket=LocalTcpServer->nextPendingConnection();

    /*关联可读信号*/
    connect(ClientSocket,SIGNAL(readyRead()),this,SLOT(ReadTcpClientData()));
    /*关联断开信号*/
    connect(ClientSocket,SIGNAL(disconnected()),this,SLOT(TcpClientDisconnected()));

    //***

}

//服务器模式：响应断开的客户端
void TCPServer::TcpClientDisconnected()
{
    LogSend("断开上一个客户端\n");
    emit Tcpdisconnection();
}


//计算字符串长度
inline int my_strlen(const char *p)
{
    const char *tmp=p;
    while(1)
    {
        if(*tmp!='\0' &&
           *tmp>=' '&& *tmp<='~')
        {
            tmp++;
        }
        else {
            break;
        }
    }
    return tmp-p;
}

//字符串查找函数
inline const char* my_strstr(const char *p1,const char *p2)
{
    int len1=my_strlen(p1);
    int len2=my_strlen(p2);
    if(len1<len2)return nullptr;

    int i,j;
    for(i=0;i<len1;i++)
    {
        for(j=0;j<len2;j++)
        {
            if(p1[j+i]!=p2[j])break;
        }
        if(j==len2)return p1+i;
    }
    return nullptr;
}

/*
函数功能: 字符串转整数
函数形参: 传入待转换的字符串(0~9)
返回值: 返回转换的整数结果,如果转换失败返回0
*/
//"123ABC"
inline unsigned int my_atoi(const char *str)
{
    unsigned int data=0;
    int flag=0;
    while(*str!='\0')
    {
        if(*str=='-')
        {
            flag=1; //表示是负数
        }
        else if(*str>='0' && *str<='9')
        {
            data*=10; //0  10 120
            data+=*str-'0';//1 12 123
        }
        else
        {
            break;
        }
        str++;
    }
    //if(flag)data*=-1;
    return data;
}

//提取字符串数据
//数据格式:
/*
A:心电数据1,B:新电数据2,C:运动步数,D:运动距离,E:体表温度
例如: "A:1234,B:12,C:45,D:28,E:66.55"
*/
void TCPServer::StringProcessing(const char *src_str)
{
    const char *p=src_str;
    while(1)
    {
        //查找字符串  A
        p=my_strstr(p,"A"); //心电数据1
        if(p==nullptr)break;
        p+=2;
        uart_queue_data.write_queueA(my_atoi(p));
       // qDebug()<<"A="<<my_atoi(p);

        //查找字符串  B
        p=my_strstr(p,"B"); //心电数据2
        if(p==nullptr)break;
        p+=2;
        uart_queue_data.write_queueB(my_atoi(p));
       // qDebug()<<"B="<<my_atoi(p);

        //查找字符串  C
        p=my_strstr(p,"C"); //运动步数
        if(p==nullptr)break;
        p+=2;
        uart_queue_data.write_queueC(my_atoi(p));
        //qDebug()<<"C="<<my_atoi(p);

        //查找字符串  D
        p=my_strstr(p,"D"); //运动距离
        if(p==nullptr)break;
        p+=2;
        uart_queue_data.write_queueD(my_atoi(p));
      //  qDebug()<<"D="<<my_atoi(p);

        //查找字符串  E
        p=my_strstr(p,"E"); //运动距离
        if(p==nullptr)break;
        p+=2;
        uart_queue_data.write_queueE(atof(p));
       // qDebug()<<"E="<<atof(p);
    }
}

//服务器模式:读数据
/*
向服务器上传的数据格式: 使用字符串格式上传
A:心电数据1,B:新电数据2,C:运动步数,D:运动距离,E:体表温度

例如: "A:1234,B:12,C:45,D:28,E:66"
*/
void TCPServer::ReadTcpClientData()
{
    QByteArray array;
    //判断该客户端是否已经断开
    if(ClientSocket)
    {
        array=ClientSocket->readAll();
        StringProcessing(array.data()); //解析数据
        LogSend(array);
        LogSend("\n");
    }
}
