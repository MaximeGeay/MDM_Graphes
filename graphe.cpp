#include "graphe.h"
#include "ui_graphe.h"
#include <QtConcurrent>
#include <QStandardPaths>

Graphe::Graphe(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Graphe)
{
    ui->setupUi(this);

    LabelInfo=new QCPItemText(ui->GRF);
    mIsInit=false;
    QSettings settings;
    this->setGeometry(settings.value("grfGeometry",this->geometry()).toRect());
    mImagesPath=settings.value("ImagesPath","").toString();

}

Graphe::~Graphe()
{
    delete ui;
}

void Graphe::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue("grfGeometry",this->geometry());
}


void Graphe::initGraphe(QDateTime dtDebut, QDateTime dtFin, int nMin, int nMax, QStringList listSeries)
{

    mRangeMin=nMin;
    mRangeMax=nMax;
    QList<QColor> ColorList;
    ColorList.append(Qt::blue);
    ColorList.append(Qt::red);
    ColorList.append(Qt::green);
    ColorList.append(Qt::black);
    ColorList.append(Qt::magenta);
    ColorList.append(Qt::yellow);
    ColorList.append(Qt::cyan);

    QStringListIterator it(listSeries);
    int c=0;
    while (it.hasNext())
    {
        stSerie uneSerie;
        uneSerie.label=it.next();
        uneSerie.courbe=new QCPCurve(ui->GRF->xAxis, ui->GRF->yAxis);
        if(c>6)
            c=0;
        uneSerie.couleur=ColorList.at(c);
        QPen pen;
        pen.setWidth(2);
        pen.setColor(uneSerie.couleur);
        uneSerie.courbe->setPen(pen);
        uneSerie.courbe->setName(uneSerie.label);

        mListSerie.append(uneSerie);
        c++;
    }


    this->setWindowTitle(mListSerie.first().label);

    ui->GRF->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom|QCP::iSelectPlottables);
    ui->GRF->setSelectionTolerance(20);

    ui->GRF->axisRect()->setRangeZoom(Qt::Vertical);

    ui->GRF->axisRect()->setRangeZoomAxes(ui->GRF->xAxis,ui->GRF->yAxis);
    ui->GRF->axisRect()->setRangeDragAxes(ui->GRF->xAxis,ui->GRF->yAxis);

    setY(mRangeMin,mRangeMax);

    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeSpec(Qt::TimeSpec::UTC);
    dateTicker->setDateTimeFormat("HH:mm:ss");
    ui->GRF->xAxis->setTicker(dateTicker);

    ui->GRF->xAxis->setLabel("Heure UTC");
    ui->GRF->yAxis->setLabel(mListSerie.first().label);
    ui->GRF->legend->setVisible(true);

    setX(dtDebut,dtFin);

    LabelInfo->setPositionAlignment(Qt::AlignHCenter|Qt::AlignTop);
    LabelInfo->position->setType(QCPItemPosition::ptPlotCoords);
    LabelInfo->setVisible(false);

    QObject::connect(ui->GRF,SIGNAL(plottableDoubleClick(QCPAbstractPlottable*,int,QMouseEvent*)),this,SLOT(AfficheLabelInfo(QCPAbstractPlottable*,int)));

    QObject::connect(ui->btn_RazYAxis,&QPushButton::clicked,this,&Graphe::razEchelle);
    QObject::connect(ui->GRF,&QCustomPlot::mouseWheel,this,&Graphe::gestionBorneZoom);
    QObject::connect(ui->dtDebut,&QDateTimeEdit::dateTimeChanged,this,&Graphe::dateTimeDebutHasChanged);
    QObject::connect(ui->dtFin,&QDateTimeEdit::dateTimeChanged,this,&Graphe::dateTimeFinHasChanged);
    QObject::connect(ui->btn_PrintScreen,&QPushButton::clicked,this,&Graphe::clickOnPrintscreen);
    mIsInit=true;

}



void Graphe::setData(QVector<QDateTime> TabDate, QList<QVector<double> > TabData)
{
    QList<QVector <double>> TabDataList;
    QVector<double> vdTabDate;
    QFuture<QVector<double>> futureDate = QtConcurrent::run(this,&Graphe::convertDateToDouble,TabDate);


    QFuture<QList<QVector <double>>> futureData = QtConcurrent::run(this,&Graphe::convertRowToCol,TabData);


    vdTabDate=futureDate.result();
    TabDataList=futureData.result();
     QVector<double>unVecData;
    if(TabDataList.size()==mListSerie.size())
    {
        int n=0;
        QListIterator<QVector<double>>it(TabDataList);
        while(it.hasNext())
        {           
            unVecData=it.next();
            mListSerie[n].courbe->setData(vdTabDate,unVecData);
            n++;
        }
    }

    mdtDebut=TabDate.first();
    mdtFin=TabDate.last();

    setX(mdtDebut,mdtFin);


}

void Graphe::AfficheLabelInfo(QCPAbstractPlottable *curve, int i)
{


    double dData=0;
    double dHeure=0;
    double dPosY=ui->GRF->yAxis->range().upper;
    QString sSource;

    dData=curve->interface1D()->dataMainValue(i);
    dHeure=curve->interface1D()->dataMainKey(i);

    QDateTime dtHeure=QDateTime::fromTime_t(dHeure);
    dtHeure.setTimeZone(QTimeZone::utc());
    LabelInfo->position->setCoords(dHeure,dPosY);
    LabelInfo->setText(sSource+"\r\n"+QString::number(dData,'f',1).rightJustified(7,' ')+"\r\n"+dtHeure.toString("HH:mm:ss")+"\r\n"+QDateTime::fromTime_t(dHeure).toString("dd/MM/yyyy"));
    LabelInfo->setVisible(true);

}

void Graphe::gestionBorneZoom(QWheelEvent *event)
{
    QCPRange range=ui->GRF->yAxis->range();
    if(range.upper>mRangeMax)
        ui->GRF->yAxis->setRangeUpper(mRangeMax);
    if(range.lower<mRangeMin)
        ui->GRF->yAxis->setRangeLower(mRangeMin);

}

void Graphe::dateTimeDebutHasChanged(QDateTime uneDT)
{
    if(uneDT.isValid())
    {
        if(mdtDebut.secsTo(uneDT)<0)
            ui->dtDebut->setDateTime(mdtDebut);

        dateTimeHasChanged();
    }
}

void Graphe::dateTimeFinHasChanged(QDateTime uneDT)
{
    if(uneDT.isValid())
    {
        if(uneDT.secsTo(mdtFin)<0)
            ui->dtFin->setDateTime(mdtFin);

        dateTimeHasChanged();
    }
}

void Graphe::dateTimeHasChanged()
{
        setX(ui->dtDebut->dateTime(),ui->dtFin->dateTime());
        this->replot();
}

void Graphe::razEchelle()
{
    if(mIsInit)
    {
        setY(mRangeMin,mRangeMax);
        setX(mdtDebut,mdtFin);

        this->replot();
    }
}

void Graphe::clickOnPrintscreen()
{
    QString dirPath;
    bool bSuccess=false;
    if(mImagesPath.isEmpty())
        dirPath=QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).first();
    else
        dirPath=mImagesPath;

    QString fileName=QString("%1/%2.png").arg(dirPath).arg(this->windowTitle());
    qDebug()<<dirPath;
    qDebug()<<mImagesPath;
    fileName = QFileDialog::getSaveFileName(this,tr("Enregistrer le Snapshot"),fileName, tr("Images (*.png *.jpg *bmp *pdf);;Images(*.png);;Images(*.jpg);;Images(*.bmp);;Document(*.pdf)"));
    if(!fileName.isNull())
    {
       if(fileName.section("/",-1).endsWith(".png",Qt::CaseInsensitive))
           bSuccess=ui->GRF->savePng(fileName);
       if(fileName.section("/",-1).endsWith(".jpg",Qt::CaseInsensitive))
           bSuccess=ui->GRF->saveJpg(fileName);
       if(fileName.section("/",-1).endsWith(".bmp",Qt::CaseInsensitive))
           bSuccess=ui->GRF->saveBmp(fileName);
       if(fileName.section("/",-1).endsWith(".pdf",Qt::CaseInsensitive))
           bSuccess=ui->GRF->savePdf(fileName);

       if(bSuccess)
       {
           mImagesPath=fileName.section("/",0,fileName.count("/")-1);
           QSettings settings;
           settings.setValue("ImagesPath",mImagesPath);

           QDesktopServices::openUrl(mImagesPath);
       }

    }

}


QVector<double> Graphe::convertDateToDouble(QVector<QDateTime> TabDate)
{
    QVector<double>TabNum;
    int n=0;
    QVectorIterator<QDateTime> it(TabDate);
    while(it.hasNext())
    {
        TabNum.append(it.next().toTime_t());
        n++;
    }

    return TabNum;
}

QList<QVector<double> > Graphe::convertRowToCol(QList<QVector<double> > TabIn)
{
    QList<QVector<double>> TabOut;
    QVector<double>uneLigneIn;
    QVector<double>uneLigneOut;
    int nbCol=TabIn.first().count();
    int nbLigne=TabIn.size();
    double dInit=0;
    for(int i=0;i<nbLigne;i++)
    {
        uneLigneOut.append(dInit);
    }
    for(int i=0;i<nbCol;i++)
    {
        TabOut.append(uneLigneOut);
    }

    QListIterator<QVector<double>>it(TabIn);
    for(int i=0;i<nbCol;i++)
    {
        it.toFront();
        int n=0;
        while(it.hasNext())
        {

            uneLigneIn=it.next();
            uneLigneOut[n]=uneLigneIn[i];
            TabOut[i]=uneLigneOut;
            n++;

        }
    }


    return TabOut;

}


void Graphe::setY(int nMin, int nMax)
{
    ui->GRF->yAxis->setRange(nMin,nMax);


}

void Graphe::setX(QDateTime dtDebut, QDateTime dtFin)
{
    dtDebut.setTimeZone(QTimeZone::utc());
    dtFin.setTimeZone(QTimeZone::utc());
    double dDebut=dtDebut.toTime_t();

    double dFin=dtFin.toTime_t();
    ui->dtDebut->setDateTime(dtDebut);
    ui->dtFin->setDateTime(dtFin);

    ui->GRF->xAxis->setRange(dDebut,dFin);

}


void Graphe::replot()
{
    ui->GRF->replot();
}

