#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDesktopWidget>

#include <QMimeData>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include <QFont>
#include <QFontDatabase>
#include <QDateTime>
#include <QEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);    // 禁止最大化按钮
    setFixedSize(this->width(),this->height());     // 固定窗口大小
    // 初始化窗口在屏幕中央
    move((qApp->desktop()->availableGeometry().width() - width()) / 2 + qApp->desktop()->availableGeometry().x(), (qApp->desktop()->availableGeometry().height() - height()) / 2 + qApp->desktop()->availableGeometry().y());

    this->setAcceptDrops(true);
    ui->icon->setScaledContents(true);
    ShowUI();
    Initialize();

    /* deepin 系统设置里的个性化-字体设置会覆盖程序自定义字体
     * 非 deepin 系统可以调用自定义字体并初始化窗体
     *
     * LoadCustomFont();
     */

    process = new QProcess();
    process->setReadChannel(QProcess::StandardOutput);
    connect(process, &QProcess::readyReadStandardOutput, [&]
    {
        ui->textBrowser->append(process->readAllStandardOutput());
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QString name = event->mimeData()->urls().at(0).toLocalFile();
    suf = QFileInfo(name);
    QString suffix = suf.suffix();
    /*
     * if(suffix == "deb")
     * {
     *     ui->filename->setText(suf.filePath());
     *     ui->name->setText(suf.baseName());
     * }
     * else
     * {
     */
        if(suffix == "png")     // 或者 suffix == "jpg"，以后可能会支持？
        {
            /*
             * if(ui->image->toPlainText() == "")
             * {
             *     num = 1;
             * }
             * QProcess mv;
             * QString afterMV = QString(suf.absolutePath() + "/screen_%1." + suf.suffix()).arg(num);
             * mv.start("cp -a " + suf.filePath() + " " + afterMV);
             * mv.waitForFinished();
             */
            ui->image->append(suf.filePath());
            // num++;
        }
        else
        {
            QMessageBox::critical(this, "呐！", "内个...只支持png文件呢～");
        }
    // }
}


void MainWindow::on_pushButton_clicked()
{
    if(QFileInfo::exists(ui->filename->text()))
    {
        if(QFileInfo(ui->filename->text()).suffix() == "deb")
        {
            if(ui->name->text() == "" || ui->filename->text() == "" || ui->pkgname->text() == "" || ui->version->text() == "" || ui->contributor->text() == "" || icon == "" || ui->image->toPlainText() == "" || ui->info->toPlainText() == "")
            {
                QMessageBox::information(NULL, tr("诶？"), tr("你好像忘记了什么～"));
            }
            else
            {
                if(ui->author->text() == "")
                    ui->author->setText(" ");
                if(ui->website->text() == "")
                    ui->website->setText(" ");
                HideUI();
                MakeJson();
            }
        }
        else
        {
            QMessageBox::critical(this, "呐！", "内个...你选择的好像不是deb文件呐～");
        }
    }
    else
    {
        QMessageBox::critical(this, "呐！", "这是个不存在的文件啊喂！");
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    this->close();
}

void MainWindow::on_pushButton_3_clicked()
{
    icon = QFileDialog::getOpenFileName(this, tr("选择图标"), QDir::homePath(), tr("Image Files(*.png)"));  // 或者 "*.jpg"，以后可能会支持？
    if(icon.length() == 0) {
        QMessageBox::information(NULL, tr("呐！"), tr("根本没有选中东西嘛！"));
        icon = tmpicon;
    } else {
        // icon = QFileInfo(icon).absolutePath() + "/icon." + QFileInfo(icon).suffix();  // 先留着，可能用得上......
        tmpicon = icon;
        ui->pushButton_3->setText("");
        ui->icon->setPixmap(QIcon(icon).pixmap(135,135));
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    Initialize();
    ShowUI();
}

void MainWindow::on_pushButton_5_clicked()
{
    ui->textBrowser->show();
    ui->pushButton_5->hide();
    MakeTar();
    // RemoveDir();     // 有需要可以删除构建文件夹
}

void MainWindow::on_pushButton_6_clicked()
{
    filename = QFileDialog::getOpenFileName(this, tr("选择安装包"), QDir::homePath(), tr("Deb Files(*.deb)"));
    if(filename.length() == 0) {
        QMessageBox::information(NULL, tr("呐！"), tr("根本没有选中东西嘛！"));
    } else {
        ui->filename->setText(filename);
        QProcess getPkgInfo;
        getPkgInfo.start("dpkg-deb -W \"" + filename + "\"");
        getPkgInfo.waitForFinished();
        QString pkgInfo = getPkgInfo.readAllStandardOutput();
        pkgInfo.replace(QRegExp("[\\s]+"), " ");
        QStringList list = pkgInfo.split(" ");
        pkgname = list.at(0);
        version = list.at(1);
        ui->pkgname->setText(pkgname);
        ui->version->setText(version);
    }
}

void MainWindow::LoadCustomFont()   // 未测试，谨慎使用
{
    QString font;

    int loadedFontID = QFontDatabase::addApplicationFont(":/font/华康少女字体.ttf");
    QStringList loadedFontFamilies = QFontDatabase::applicationFontFamilies(loadedFontID);
    if(!loadedFontFamilies.empty())
        font = loadedFontFamilies.at(0);

    QApplication::setFont(font);
}

void MainWindow::Initialize()
{
    ui->name->clear();
    ui->filename->clear();
    ui->pkgname->clear();
    ui->version->clear();
    ui->author->clear();
    ui->website->clear();
    ui->contributor->clear();
    ui->image->clear();
    ui->info->clear();
    ui->icon->clear();
    ui->textBrowser->clear();
    ui->pushButton_3->setText("单击此处选择Logo图标");
}

void MainWindow::ShowUI()
{
    ui->label->show();
    ui->label_2->show();
    ui->label_3->show();
    ui->label_4->show();
    ui->label_5->show();
    ui->label_6->show();
    ui->label_7->show();
    ui->label_8->show();
    ui->label_9->show();
    ui->lable22->show();
    ui->lable33->hide();
    ui->name->show();
    ui->filename->show();
    ui->pkgname->show();
    ui->version->show();
    ui->author->show();
    ui->website->show();
    ui->contributor->show();
    ui->image->show();
    ui->info->show();
    ui->icon->show();
    ui->pushButton->show();
    ui->pushButton_2->show();
    ui->pushButton_3->show();
    ui->pushButton_4->hide();
    ui->pushButton_5->hide();
    ui->pushButton_6->show();
    ui->textBrowser->hide();
}

void MainWindow::HideUI()
{
    ui->label->hide();
    ui->label_2->hide();
    ui->label_3->hide();
    ui->label_4->hide();
    ui->label_5->hide();
    ui->label_6->hide();
    ui->label_7->hide();
    ui->label_8->hide();
    ui->label_9->hide();
    ui->lable22->hide();
    ui->lable33->show();
    ui->name->hide();
    ui->filename->hide();
    ui->pkgname->hide();
    ui->version->hide();
    ui->author->hide();
    ui->website->hide();
    ui->contributor->hide();
    ui->image->hide();
    ui->info->hide();
    ui->icon->hide();
    ui->pushButton->hide();
    ui->pushButton_2->hide();
    ui->pushButton_3->hide();
    ui->pushButton_4->show();
    ui->pushButton_5->show();
    ui->pushButton_6->hide();
    ui->textBrowser->hide();
}

void MainWindow::MakeJson()
{
    name = ui->name->text();
    /*
     * local = name;
     * local.replace(QRegExp("[\\s]+"), "");    // 本地创建的文件夹自动删除空格，避免写入json失败，原因未知!
     */
    filename = ui->filename->text();
    QString origin = QFileInfo(filename).fileName();  // 原始文件名
    origin.replace("+", "_plus_");    // 替换"+"为"_plus_"，避免识别问题
    pkgname = ui->pkgname->text();  // 更换标准，使用pkgname作为文件夹名，方便上传
    version = ui->version->text();
    author = ui->author->text();
    website = ui->website->text();
    contributor = ui->contributor->text();
    update = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm");
    screenshots = ui->image->toPlainText();
    info = ui->info->toPlainText();
    info.replace("\n", "\\n");     // 将简介信息中的换行符转义为\\n，配合json读取需要
    info.replace("\"", "\\\"");     // 将简介信息中的 \" 转义为\\"，配合json读取需要
    size = CalculateSize(QFileInfo(filename).size());

    QProcess build;
    build.start("mkdir " + QDir::homePath() + "/Desktop/" + pkgname + "/");
    build.waitForFinished();
    build.start("cp \"" + filename + "\" " + QDir::homePath() + "/Desktop/" + pkgname + "/" + origin);  // 替换原有文件名
    build.waitForFinished();
    build.start("cp \"" + icon + "\" " + QDir::homePath() + "/Desktop/" + pkgname + "/icon.png");   // + QFileInfo(icon).suffix());   //保留图片后缀识别，便于后期扩展图片类型支持
    build.waitForFinished();
    build.start("mogrify -resize 128x128 " + QDir::homePath() + "/Desktop/" + pkgname + "/icon.png");
    build.waitForFinished();
    build.start("mv " + QDir::homePath() + "/Desktop/" + pkgname + "/icon.png " + QDir::homePath() + "/Desktop/" + pkgname + "/icon.png.update");
    build.waitForFinished();
    build.close();
    /*
     * build.start("cp \"" + screenshots + "\" -t " + QDir::homePath() + "/Desktop/" + local + "/");
     * build.waitForFinished();
     */
    QStringList list = screenshots.split("\n");
    for(int i = 0; i < list.length(); i++)
    {
        QString des = QString(QDir::homePath() + "/Desktop/" + pkgname + "/screen_%1.png").arg(i + 1);
        build.start("cp \"" + list.at(i) + "\" " + des);
        build.waitForFinished();
        build.start("mogrify -resize 800x600 " + des);
        build.waitForFinished();
        build.start("mv " + des + " " + QString(QDir::homePath() + "/Desktop/" + pkgname + "/screen_%1.png.update").arg(i + 1));
        build.waitForFinished();
        build.close();
    }

    QFile json(QDir::homePath() + "/Desktop/" + pkgname + "/app.json.update");   // 若文件夹中有空格，此处写入失败，加了引号也是，原因未知!
    if(!json.open(QIODevice::Append | QIODevice::Text))  // append追加新内容到文件末尾
    {
        QMessageBox::critical(this, "错误", "写入控制文件失败！", "确定");
        ShowUI();
        RemoveDir();
        return;
    }
    QTextStream write(&json);   // 写入 json
    write <<"{"<<endl;
    write <<"   \"Name\":\"" + name + "\","<<endl;
    write <<"   \"Version\":\"" + version + "\","<<endl;
    write <<"   \"Filename\":\"" + origin + "\","<<endl;    // 替换原有文件名
    write <<"   \"Pkgname\":\"" + pkgname + "\","<<endl;
    write <<"   \"Author\":\"" + author + "\","<<endl;
    write <<"   \"Contributor\":\"" + contributor + "\","<<endl;
    write <<"   \"Website\":\"" + website + "\","<<endl;
    write <<"   \"Update\":\"" + update + "\","<<endl;
    write <<"   \"Size\":\"" + size + "\","<<endl;
    write <<"   \"More\":\"" + info + "\""<<endl;
    write <<"}"<<endl;
    write.flush();
    json.close();
}

void MainWindow::MakeTar()
{
    process->setWorkingDirectory(QDir::homePath() + "/Desktop/");
    process->start("tar -zcvf " + pkgname + ".tar.gz ./" + pkgname + "/");
    process->waitForFinished();
    process->close();
}

void MainWindow::RemoveDir()    // 考虑后期压缩后直接删除文件夹
{
    QProcess rm;
    rm.start("rm -rf " + QDir::homePath() + "/Desktop/" + pkgname + "/");
    rm.waitForFinished();
    rm.close();
}

QString MainWindow::CalculateSize(qint64 size)
{
    int integer = 0;  //整数位
    int decimal = 0;  //小数位，保留两位
    QString unit = "B";
    qint64 standardSize = size;
    qint64 curSize = size;

    if(standardSize > 1024) {
        curSize = standardSize * 1000;
        curSize /= 1024;
        integer = curSize / 1000;
        decimal = curSize % 1000;
        standardSize /= 1024;
        unit = "KB";
        if(standardSize > 1024) {
            curSize = standardSize * 1000;
            curSize /= 1024;
            integer = curSize / 1000;
            decimal = curSize % 1000;
            standardSize /= 1024;
            unit = "MB";
            if(standardSize > 1024) {
                curSize = standardSize * 1000;
                curSize /= 1024;
                integer = curSize / 1000;
                decimal = curSize % 1000;
                unit = "GB";
            }
        }
    }

    QString dec = "0";
    if (0 <= decimal && decimal <= 4) {
        dec += "0";
    }

    if (5 <= decimal && decimal <= 9) {
        dec += "1";
    }

    if (10 <= decimal && decimal <= 99) {
        int temp = decimal % 10;
        decimal /= 10;
        if(5 <= temp && temp <= 9)
            decimal++;
        dec += QString::number(decimal);
    }

    if (100 <= decimal && decimal <= 999) {
        int temp = decimal % 10;
        decimal /= 10;
        if(5 <= temp && temp <= 9)
            decimal++;
        dec = QString::number(decimal);
    }

    return QString::number(integer) + "." + dec + unit;
}
