#ifndef QSMTPCLIENT_H
#define QSMTPCLIENT_H

#include "qsmtpclient_global.h"

#include <QStringList>

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

    QSmtpClient(const QString &server, quint16 port, const QString &user, const QString &password);

    void addReceiver(const QString &to);

    void addCC(const QString &cc);

    void addBCC(const QString &bcc);

    void setSubject(const QString &subject);

    void setContent(const QString &content, QSmtpClient::SmtpFormat format = QSmtpClient::SmtpFormat::PlainText);

    bool sendMail();

    QString getLastError();

private:
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
};

#endif // QSMTPCLIENT_H
