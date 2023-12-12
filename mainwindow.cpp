#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QStandardPaths>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QStatusBar>

#include "graphe.h"


#define version "MDM_Graphes 0.2"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(version);

    QSettings settings;
    QObject::connect(ui->actionQuitter,&QAction::triggered,this,&MainWindow::close);
    QObject::connect(ui->actionApropos,&QAction::triggered,this,&MainWindow::aPropos);    
    QObject::connect(ui->actionFormat_CSV,&QAction::triggered,this,&MainWindow::clickOnFormatCSV);
    QObject::connect(ui->actionHowTo,&QAction::triggered,this,&MainWindow::clickOnHowTo);
    QObject::connect(ui->tb_Parcourir,&QToolButton::clicked,this,&MainWindow::clickOnParcourir);
    QObject::connect(ui->btn_Load,&QPushButton::clicked,this,&MainWindow::clickOnLoad);
    QObject::connect(ui->btn_AddY,&QPushButton::clicked,this,&MainWindow::clickOnAddY);
    QObject::connect(ui->btn_RmY,&QPushButton::clicked,this,&MainWindow::clickOnRmY);
    QObject::connect(ui->btn_RazY,&QPushButton::clicked,this,&MainWindow::clickOnRazY);
    QObject::connect(ui->btn_Grf,&QPushButton::clicked,this,&MainWindow::clickOnGrf);
    QObject::connect(ui->lv_Yaxis,&QListView::clicked,this,&MainWindow::clickOnYList);

    this->setGeometry(settings.value("geometry",this->geometry()).toRect());

    mFilePath=settings.value("FilePath","").toString();
    if(!mFilePath.isEmpty())
        ui->le_File->setText(mFilePath);

    mEntetesModel=new QStringListModel();
    mYaxisModel=new QStringListModel();

    ui->lV_Entetes->setModel(mEntetesModel);
    ui->lv_Yaxis->setModel(mYaxisModel);
    ui->btn_Grf->setEnabled(false);



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue("geometry",this->geometry());
}



void MainWindow::aPropos()
{
    QString sText=QString("%1\nUtilitaire d'affichage des données CSV extraites du MDM500\n\nSources: https://github.com/MaximeGeay/MDM_Graphes\n"
                          "Développé avec Qt 5.14.1\nMaxime Geay\nDécembre 2023").arg(version);
    QMessageBox::information(this,"Informations",sText);
}

void MainWindow::clickOnFormatCSV()
{
    QString sText=QString("Pour être lisible, le fichier CSV doit avoir les caractéristiques suivantes:\n"
                          "Le séparateur de colonnes doit être la ','\n"
                          "Le séparateur de lignes doit être le 'RC'\n"
                          "La première ligne doit contenir les en-têtes de colonnes.\n"
                          "La première colonne doit être le timestamp au format : aaaa-MM-jj hh:mm:ss").arg(version);
    QMessageBox::information(this,"Informations: Caractéristiques CSV",sText);
}

void MainWindow::clickOnHowTo()
{
    QString sText=QString("- Exporter un fichier CSV depuis MDM_Export\n"
                          "- Sélectionner le fichier à l'aide du bouton '...'\n"
                          "- Charger le fichier sélectionné en cliquant sur le bouton 'Charger'\n"
                          "- Dans la liste de données disponibles, sélectionner la série souhaitée et l'ajouter à la liste des données à afficher"
                          "en cliquant sur le bouton '-->'\n"
                          "On peut ajouter autant de série de données que l'on souhaite mais le traitement sera plus long"
                          "- Si on souhaite retirer une série de la liste à afficher, il suffit de la sélectionner puis de cliquer sur le bouton '<--'\n"
                          "- Pour générer le graphe, il suffit de cliquer sur le bouton 'Graphe'.\n"
                          "Le temps de traitement peut être long suivant la taille du fichier et le nombre de séries sélectionnées.\n"
                          "On peut créer autant de graphes que l'on souhaite").arg(version);
    QMessageBox::information(this,"Informations: Manuel utilisateur",sText);
}

void MainWindow::clickOnParcourir()
{
    QString dirPath;
    if(mFilePath.isEmpty())
        dirPath=QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first();
    else
        dirPath=mFilePath.section("/",0,mFilePath.count("/")-1);

    QString fileName = QFileDialog::getOpenFileName(this, tr("Ouvrir un fichier CSV"),
                                                    dirPath,
                                                    tr("Fichier CSV (*.csv)"));
    if(!fileName.isNull())
        ui->le_File->setText(fileName);

}

void MainWindow::clickOnLoad()
{
    QString fileName=ui->le_File->text();

        if(QFile::exists(fileName))
        {
            QFile fichier(fileName);
            if(fichier.open(QIODevice::ReadOnly))
            {
                QTextStream flux(&fichier);
                QString sFirst=flux.readLine();
                int nChamps=sFirst.count(",");

                mEntetes.clear();
                mYaxisList.clear();
                ui->btn_Grf->setEnabled(false);

                for(int i=0;i<=nChamps;i++)
                    mEntetes.append(sFirst.section(",",i,i));


                mEntetesModel->setStringList(mEntetes);
                mYaxisModel->setStringList(mYaxisList);


                mFilePath=fileName;
                QSettings settings;
                settings.setValue("FilePath",mFilePath);
                fichier.close();

            }

        }


}

void MainWindow::clickOnAddY()
{
    QString selData=mEntetesModel->data(ui->lV_Entetes->currentIndex()).toString();
    if(!selData.isEmpty())
    {
        if(mYaxisList.contains(selData)==false)
            mYaxisList.append(selData);

        mYaxisModel->setStringList(mYaxisList);
        ui->btn_Grf->setEnabled(true);
    }
}

void MainWindow::clickOnRmY()
{
    QString selData=mYaxisModel->data(ui->lv_Yaxis->currentIndex()).toString();
    if(!selData.isEmpty())
    {
        mYaxisList.removeAll(selData);
        mYaxisModel->setStringList(mYaxisList);
        if(mYaxisList.isEmpty())
            ui->btn_Grf->setEnabled(false);

    }

}

void MainWindow::clickOnRazY()
{
    mYaxisList.clear();
    mYaxisModel->setStringList(mYaxisList);
    ui->btn_Grf->setEnabled(false);
}

void MainWindow::clickOnYList()
{
    QStringListIterator it(mYaxisList);
    int nCol=0;
    while(it.hasNext())
    {
        QString sCurrentCol=it.next();
        nCol=mEntetes.indexOf(sCurrentCol);
    }
}

void MainWindow::clickOnGrf()
{

    this->setCursor(Qt::WaitCursor);
    QList<QVector<double>> TabData;
    QVector<QDateTime> TabDate;
    QStringList listSeries;

    QString fileName=ui->le_File->text();
    QString sUneLigne;
    QDateTime dateheure;
    double dMin=0;
    double dMax=0;
    bool bValid=false;

        if(QFile::exists(fileName))
        {
            QFile fichier(fileName);
            if(fichier.open(QIODevice::ReadOnly))
            {

                QTextStream flux(&fichier);

                int n=0;
                 bool bFirst=true;
                while(!flux.atEnd())                   
                {
                    sUneLigne=flux.readLine();
                    if(n>0)
                    {
                    //2023-11-16 05:04:01
                    TabDate.append(QDateTime::fromString(sUneLigne.section(",",0,0),"yyyy-MM-dd hh:mm:ss"));

                    QVector<double>listValues;
                    QStringListIterator it(mYaxisList);
                    int nCol=0;

                    listSeries.clear();
                    while(it.hasNext())
                    {
                        QString sCurrentCol=it.next();
                        listSeries.append(sCurrentCol);
                        nCol=mEntetes.indexOf(sCurrentCol);
                        double dValue=sUneLigne.section(",",nCol,nCol).toDouble();

                        if(bFirst)
                        {
                            dMin=dValue;
                            dMax=dValue;
                            bFirst=false;
                        }
                        else
                        {
                            if(dValue>dMax)
                                dMax=dValue;
                            if(dValue<dMin)
                                dMin=dValue;

                        }

                        listValues.append(dValue);
                    }
                    bValid=true;
                    TabData.append(listValues);

                    }
                    n++;
                }


                fichier.close();
            }


        }
        if(bValid)
        {

            Graphe * grf=new Graphe;

            grf->initGraphe(TabDate.first(),TabDate.last(),qRound(dMin)-1,qRound(dMax)+1,listSeries);
            grf->setData(TabDate,TabData);

            grf->show();
            grf->replot();



        }



        this->setCursor(Qt::ArrowCursor);
}








