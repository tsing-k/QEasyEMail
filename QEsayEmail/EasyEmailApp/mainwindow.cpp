#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <qsmtpclient.h>
#include <QDateTime>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSmtpClient smtpClient(QSmtpClient::ServerType::NTES_126, "onlylinker@126.com", "Linker");
    smtpClient.setSubject("测试SMTP");
    smtpClient.setContent(getTestHtml(), QSmtpClient::SmtpFormat::Html);
    smtpClient.addReceiver("co.du@dct-china.cn");
    smtpClient.addCC("15822680713@139.com");
    if(smtpClient.sendMail())
    {
        QMessageBox::warning(this, "Send", "Send mail success!");
    }
    else
    {
        QMessageBox::warning(this, "Send", "Send mail failed with " + smtpClient.getLastError() + "!");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::getTestHtml() const
{
    return "<h1>测试QSmtpClient库发送邮件</h1><br/><p>收到此邮件说明功能正常。</p>" \
           "<footer>" + QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss") + "</footer>";
}
