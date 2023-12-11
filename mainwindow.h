#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void aPropos();
    void clickOnParcourir();
    void clickOnLoad();
    void clickOnAddY();
    void clickOnRmY();
    void clickOnRazY();
    void clickOnYList();
    void clickOnGrf();


private:
    Ui::MainWindow *ui;
    QString mFilePath;
    QStringList mEntetes;
    QStringList mYaxisList;
    QStringListModel *mEntetesModel;
    QStringListModel *mYaxisModel;


};
#endif // MAINWINDOW_H
