#include "qpop3client.h"

#include <QDebug>
#include <QObject>
#include <QTcpSocket>
#include <QDateTime>

QPop3Client::QPop3Client(const QString &server, quint16 port, const QString &user, const QString &password)
    : mSocket(new QTcpSocket)
    , mUser(user)
    , mPassword(password)
    , mServerName(server)
    , mPort(port)
{
    connectServer(mServerName, mPort, "+OK");
    sendCommand("user " + mUser + CRLF, "+OK");
    sendCommand("pass " + mPassword + CRLF, "+OK");
    int iCount = getMailCount();
    qDebug() << iCount;
    fetchMail(iCount);
    sendCommand("quit" + CRLF, "+OK");
}

QPop3Client::QPop3Client(QPop3Client::ServerType serverType, const QString &user, const QString &password)
{
    initCommonServers();
    ServerInfo info = mServerInfos.value(serverType);
    new (this)QPop3Client(info.serverName, info.port, user, password);
}

void QPop3Client::initCommonServers()
{
    mServerInfos[ServerType::NTES_126] = ServerInfo("pop3.126.com", 110);
    mServerInfos[ServerType::NTES_163] = ServerInfo("pop3.163.com", 110);
    mServerInfos[ServerType::NTES_Yeah_Net] = ServerInfo("pop3.yeah.net", 110);
    mServerInfos[ServerType::CMCC_139] = ServerInfo("pop3.139.com", 110);
    mServerInfos[ServerType::ALi] = ServerInfo("pop3.qiye.aliyun.com", 110);
    mServerInfos[ServerType::QQ] = ServerInfo("pop3.qq.com", 110);
    mServerInfos[ServerType::Sina] = ServerInfo("pop3.sina.com", 110);
    mServerInfos[ServerType::Sina_Vip] = ServerInfo("pop3.vip.sina.com", 110);
    mServerInfos[ServerType::Yahoo] = ServerInfo("pop3.mail.yahoo.cn", 110);
    mServerInfos[ServerType::GMail] = ServerInfo("pop3.gmail.com", 110);
}

bool QPop3Client::connectServer(const QString &serverName, quint16 port, const QString &expectedCode)
{
    bool ret = false;
    mSocket->connectToHost(serverName, port);
    if(mSocket->waitForConnected() && mSocket->waitForReadyRead())
    {
        QString resp = mSocket->readAll();
        int index = resp.indexOf(QChar(' '));
        if(index > 0)
        {
            if(expectedCode == resp.left(index))
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

bool QPop3Client::sendCommand(const QString &data, const QString &expectedCode)
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
            if(expectedCode == resp.left(index))
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

bool QPop3Client::fetchMail(quint64 num)
{
    QByteArray datagram;
    datagram.append("retr " + QString::number(num) + CRLF);
    mSocket->write(datagram);
    if(mSocket->waitForReadyRead())
    {
//        while(mSocket->bytesAvailable())
//        {
//            QString line = QString(mSocket->readLine()).trimmed();
//            qDebug() << line;
//        }

        QString resp = QString(mSocket->readAll()).trimmed();
        QStringList line = resp.split(CRLF);
        qDebug() << resp;
        if(line.count() > 1)
        {

        }
        else
        {
            return false;
        }
    }

    return true;
}

quint64 QPop3Client::getMailCount()
{
    quint64 count = 0;
    QByteArray datagram;
    datagram.append("stat" + CRLF);
    mSocket->write(datagram);
    if(mSocket->waitForReadyRead())
    {
        QString resp = QString(mSocket->readAll()).trimmed();
        QStringList line = resp.split(" ");
        if(line.count() > 1 && line[0] == "+OK")
        {
            count = line[1].toULongLong();
        }
    }

    return count;
}
