#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QProcess>
#include <QFileInfo>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void LoadCustomFont();
    void Initialize();
    void ShowUI();
    void HideUI();
    void MakeJson();
    void MakeTar();
    void RemoveDir();
    QString CalculateSize(qint64 size);

private:
    Ui::MainWindow *ui;

    QString name;
    QString local;
    QString filename;
    QString pkgname;
    QString version;
    QString author;
    QString website;
    QString contributor;
    QString update;
    QString size;
    QString info;
    QString screenshots;
    QString icon;

    QFileInfo suf;

    QProcess *process;

    // int num = 1;

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();

};
#endif // MAINWINDOW_H
