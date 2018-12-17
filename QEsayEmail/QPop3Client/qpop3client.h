#ifndef QPOP3CLIENT_H
#define QPOP3CLIENT_H

#include "qpop3client_global.h"

#include <QStringList>
#include <QMap>

#define CRLF    QString("\r\n")

class QTcpSocket;

class QPOP3CLIENTSHARED_EXPORT QPop3Client
{
public:
    enum Pop3Format
    {
        PlainText = 0,
        Html = 1
    };

    enum ServerType
    {
        NTES_126 = 0,           //网易126
        NTES_163,               //网易163
        NTES_Vip_163,           //网易vip.163
        NTES_Yeah_Net,          //网易yeah.net
        CMCC_139,               //移动139
        ALi,                    //阿里
        QQ,                     //QQ邮箱
        Sina,                   //新浪
        Sina_Vip,               //新浪vip
        Yahoo,                  //雅虎
        GMail,                  //gmail
    };

    QPop3Client(const QString &server, quint16 port, const QString &user, const QString &password);

    QPop3Client(ServerType serverType, const QString &user, const QString &password);

    QStringList getReceivers() { return mToList; }

    QStringList getCCs() { return mCcList; }

    QStringList getBCCs() { return mBCcList; }

    QString getSubject() { return mSubject; }

    QString getContent() { return mContent; }

    QString getLastError() { return mLastError; }

private:
    struct ServerInfo
    {
        QString serverName;
        quint16 port;
        ServerInfo(const QString &server, quint16 port)
            : serverName(server)
            , port(port)
        {}
        ServerInfo(){}
    };

    void initCommonServers(); //初始化常用的邮箱

    bool connectServer(const QString &serverName, quint16 port, const QString &expectedCode);

    bool sendCommand(const QString &data, const QString &expectedCode);

    bool fetchMail(quint64 num);

    quint64 getMailCount();

private:
    QTcpSocket  *mSocket = 0;
    QString     mUser;          //账户名,及发件人From
    QString     mPassword;      //密码

    QStringList mToList;        //收件人
    QStringList mCcList;        //抄送人
    QStringList mBCcList;       //秘密抄送人

    QString     mSubject;       //主题
    QString     mContent;       //邮件内容
    Pop3Format  mFormat = PlainText;    //邮件格式 普通文本，html

    QString     mLastError;     //保存最后一次错误

    QString mServerName;
    quint16 mPort;

    QMap<ServerType, ServerInfo> mServerInfos;
};

#endif // QPOP3CLIENT_H
