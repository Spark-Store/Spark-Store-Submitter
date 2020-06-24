#include "makezip.h"
#include "mainwindow.h"
#include "ui_makezip.h"
#include <QTimer>
#include <QThread>
#include <QProcess>
#include <QDir>
#include <QDebug>

QProcess *m_processMkPkg=new QProcess();

MakeZip::MakeZip(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MakeZip)
{
    ui->setupUi(this);
    ui->textBrowser->setText("一切就绪！");
    m_processMkPkg->setReadChannel(QProcess::StandardOutput);
    connect(m_processMkPkg, &QProcess::readyReadStandardOutput, [&]
    {
        ui->textBrowser->append(m_processMkPkg->readAllStandardOutput());
    });
}

MakeZip::~MakeZip()
{
    delete ui;
}
void MakeZip::make_zip()
{
    m_processMkPkg->start("");
    m_processMkPkg->waitForFinished();
    QString name=m_processMkPkg->readAllStandardOutput();
    qDebug()<<name<<endl;
    //m_processMkPkg->start("zip -r "+QDir::homePath()+"/");
    //m_processMkPkg->waitForFinished();
}

void MakeZip::on_pushButton_clicked(bool checked)
{
    make_zip();
}
