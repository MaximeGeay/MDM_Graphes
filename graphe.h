#ifndef GRAPHE_H
#define GRAPHE_H

#include <QWidget>
#include <QDateTime>

#include "qcustomplot.h"

namespace Ui {
class Graphe;


}

class Graphe : public QWidget
{
    Q_OBJECT

public:
    explicit Graphe(QWidget *parent = nullptr);
    ~Graphe();

    struct stSerie
    {
        QColor couleur;
        QString label;
        QCPCurve *courbe;

    };

protected:
    void closeEvent(QCloseEvent *event) override;



public slots:

    void setData(QVector<QDateTime> TabDate,QList<QVector <double>> TabData);
    void initGraphe(QDateTime dtDebut,QDateTime dtFin,int nMin,int nMax,QStringList listSeries);
    void setY(int nMin,int nMax);
    void setX(QDateTime dtDebut,QDateTime dtFin);
    void replot();


private slots:
    void AfficheLabelInfo(QCPAbstractPlottable*curve, int i);
    void gestionBorneZoom (QWheelEvent* event);
    void dateTimeDebutHasChanged(QDateTime uneDT);
    void dateTimeFinHasChanged(QDateTime uneDT);
    void dateTimeHasChanged();
    void razEchelle();

private:
    Ui::Graphe *ui;

    QList<stSerie> mListSerie;
    QVector<double> convertDateToDouble(QVector<QDateTime> TabDate);
    QList<QVector <double>>convertRowToCol(QList<QVector <double>>TabIn);
    QDateTime mdtDebut;
    QDateTime mdtFin;
    int mRangeMin;
    int mRangeMax;
    QCPItemText *LabelInfo;
    bool mIsInit;
};

#endif // GRAPHE_H

