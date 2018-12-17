#include "qsmtpclient.h"

#include <QObject>
#include <QTcpSocket>
#include <QDateTime>
#include <QMap>

QSmtpClient::QSmtpClient(const QString &server, quint16 port, const QString &user, const QString &password)
    : mSocket(new QTcpSocket)
    , mUser(user)
    , mPassword(password)
    , mServerName(server)
    , mPort(port)
{
}

QSmtpClient::QSmtpClient(ServerType serverType, const QString &user, const QString &password)
{
    initCommonServers();
    ServerInfo info = mServerInfos.value(serverType);
    new (this)QSmtpClient(info.serverName, info.port, user, password);
}

void QSmtpClient::addReceiver(const QString &to)
{
    mToList.append(to);
}

void QSmtpClient::addBCC(const QString &bcc)
{
    mBCcList.append(bcc);
}

void QSmtpClient::setSubject(const QString &subject)
{
    mSubject = subject;
}

void QSmtpClient::setContent(const QString &content, QSmtpClient::SmtpFormat format)
{
    mContent = content;
    mFormat = format;
}

bool QSmtpClient::sendMail()
{
    return sendMail(mServerName, mPort, mUser, mPassword, mUser, mToList, mCcList, mBCcList, mSubject, mContent, mFormat);
}

QString QSmtpClient::getLastError()
{
    return mLastError;
}

void QSmtpClient::clear()
{
    mToList.clear();
    mCcList.clear();
    mBCcList.clear();
    mContent.clear();
    mSubject.clear();
    mLastError.clear();
}

void QSmtpClient::initCommonServers()
{
    mServerInfos[ServerType::NTES_126] = ServerInfo("smtp.126.com", 25);
    mServerInfos[ServerType::NTES_163] = ServerInfo("smtp.163.com", 25);
    mServerInfos[ServerType::NTES_Yeah_Net] = ServerInfo("smtp.yeah.net", 25);
    mServerInfos[ServerType::CMCC_139] = ServerInfo("smtp.139.com", 25);
    mServerInfos[ServerType::ALi] = ServerInfo("smtp.qiye.aliyun.com", 25);
    mServerInfos[ServerType::QQ] = ServerInfo("smtp.qq.com", 25);
    mServerInfos[ServerType::Sina] = ServerInfo("smtp.sina.com", 25);
    mServerInfos[ServerType::Sina_Vip] = ServerInfo("smtp.vip.sina.com", 25);
    mServerInfos[ServerType::Yahoo] = ServerInfo("smtp.mail.yahoo.cn", 25);
    mServerInfos[ServerType::GMail] = ServerInfo("smtp.gmail.com", 25);
}

void QSmtpClient::addCC(const QString &cc)
{
    mCcList.append(cc);
}

bool QSmtpClient::connectServer(const QString &serverName, quint16 port, int expectedCode)
{
    bool ret = false;
    mSocket->connectToHost(serverName, port);
    if(mSocket->waitForConnected() && mSocket->waitForReadyRead())
    {
        QString resp = mSocket->readAll();
        int index = resp.indexOf(QChar(' '));
        if(index > 0)
        {
            if(expectedCode == resp.left(index).toInt())
            {
                ret = true;
            }
            else
            {
                mLastError = resp.right(resp.length() - index - 1);
            }
        }
        else
        {
            mLastError = QObject::tr("Response error! - ") + resp;
        }
    }
    else
    {
        mLastError = QObject::tr("Socket error!");
    }

    return ret;
}

bool QSmtpClient::sendCommand(const QString &data, int expectedCode)
{
    bool ret = false;
    QByteArray datagram;
    datagram.append(data);
    mSocket->write(datagram);
    if(mSocket->waitForReadyRead())
    {
        QString resp = QString(mSocket->readAll());
        int index = resp.indexOf(QChar(' '));
        if(index > 0)
        {
            if(expectedCode == resp.left(index).toInt())
            {
                ret = true;
            }
            else
            {
                mLastError = resp.right(resp.length() - index - 1);
            }
        }
        else
        {
            mLastError = QObject::tr("Response error! - ") + resp;
        }
    }
    else
    {
        mLastError = QObject::tr("Socket error!");
    }

    return ret;
}

bool QSmtpClient::sendMail(const QString &serverName, quint16 port, const QString &user, const QString &pwd,
                           const QString &from, const QStringList &to, const QStringList &cc, const QStringList &bcc,
                           const QString &subject, const QString &content, QSmtpClient::SmtpFormat format)
{
    mLastError = "";
    QString detailContent = "";
    if(!connectServer(serverName, port, SuccessCode::Connect))
    {
        return false;
    }
    if(!sendCommand("HELO " + serverName + CRLF, SuccessCode::HELO))
    {
        return false;
    }
    if(!sendCommand("AUTH LOGIN" + CRLF, SuccessCode::Auth))
    {
        return false;
    }
    if(!sendCommand(QByteArray().append(user).toBase64() + CRLF, SuccessCode::User))
    {
        return false;
    }
    if(!sendCommand(QByteArray().append(pwd).toBase64() + CRLF, SuccessCode::Pwd))
    {
        return false;
    }
    if(!sendCommand(QString("mail from:<%1>").arg(from) + CRLF, SuccessCode::From))
    {
        return false;
    }
    detailContent += QString("From:%1").arg(from) + CRLF;
    foreach(QString _to, to)
    {
        if(!sendCommand(QString("rcpt to:<%1>").arg(_to) + CRLF, SuccessCode::Rcpt))
        {
            return false;
        }
        detailContent += QString("To:%1").arg(_to) + CRLF;
    }
    foreach(QString _cc, cc)
    {
        if(!sendCommand(QString("rcpt to:<%1>").arg(_cc) + CRLF, SuccessCode::Rcpt))
        {
            return false;
        }
        detailContent += QString("CC:%1").arg(_cc) + CRLF;
    }
    foreach(QString _bcc, bcc)
    {
        if(!sendCommand(QString("rcpt to:<%1>").arg(_bcc) + CRLF, SuccessCode::Rcpt))
        {
            return false;
        }
        detailContent += QString("BCC:%1").arg(_bcc) + CRLF;
    }
    if(!sendCommand("data" + CRLF, SuccessCode::Data))
    {
        return false;
    }
    detailContent += QString("Subject:%1").arg(subject) + CRLF;
    if(format == SmtpFormat::PlainText)
    {
        detailContent += QString("Content-Type: text/plain;") + CRLF;
    }
    else
    {
        detailContent += QString("Content-Type: text/html;") + CRLF;
    }
    detailContent += QString("charset = gb2312") + CRLF;
    //detailContent += QString("Date:%1").arg(QDateTime::currentDateTime().toString()) + CRLF;
    detailContent += CRLF;          //空行，头部和正文分界
    detailContent += content;
    detailContent += CRLF + ".";    //邮件内容结束
    qDebug() << detailContent;
    if(!sendCommand(detailContent + CRLF, SuccessCode::Send))
    {
        return false;
    }
    if(!sendCommand("quit" + CRLF, SuccessCode::Quit))
    {
        return false;
    }

    return true;
}
