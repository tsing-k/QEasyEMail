#ifndef QSMTPCLIENT_H
#define QSMTPCLIENT_H

#include "qsmtpclient_global.h"

#include <QStringList>
#include <QMap>

#define CRLF    QString("\r\n")

class QTcpSocket;

class QSMTPCLIENTSHARED_EXPORT QSmtpClient
{
public:
    enum SmtpFormat
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

    QSmtpClient(const QString &server, quint16 port, const QString &user, const QString &password);

    QSmtpClient(ServerType serverType, const QString &user, const QString &password);

    void addReceiver(const QString &to);

    void addCC(const QString &cc);

    void addBCC(const QString &bcc);

    void setSubject(const QString &subject);

    void setContent(const QString &content, QSmtpClient::SmtpFormat format = QSmtpClient::SmtpFormat::PlainText);

    bool sendMail();

    QString getLastError();

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

    enum SuccessCode
    {
        Connect = 220,
        HELO = 250,
        Auth = 334,
        User = 334,
        Pwd = 235,
        From = 250,
        Rcpt = 250,
        Data = 354,
        Send = 250,
        Quit = 221
    };

    void initCommonServers(); //初始化常用的邮箱

    bool connectServer(const QString &serverName, quint16 port, int expectedCode);

    bool sendCommand(const QString &data, int expectedCode);

    bool sendMail(const QString &serverName, quint16 port, const QString &user, const QString &pwd,
                  const QString &from, const QStringList &to, const QStringList &cc,
                  const QStringList &bcc, const QString &subject, const QString &content, QSmtpClient::SmtpFormat format);

private:
    QTcpSocket  *mSocket = 0;
    QString     mUser;          //账户名,及发件人From
    QString     mPassword;      //密码

    QStringList mToList;        //收件人
    QStringList mCcList;        //抄送人
    QStringList mBCcList;       //秘密抄送人

    QString     mSubject;       //主题
    QString     mContent;       //邮件内容
    QSmtpClient::SmtpFormat  mFormat = PlainText;    //邮件格式 普通文本，html

    QString     mLastError;     //保存最后一次错误

    QString mServerName;
    quint16 mPort;

    QMap<ServerType, ServerInfo> mServerInfos;
};

#endif // QSMTPCLIENT_H
