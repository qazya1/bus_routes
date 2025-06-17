#ifndef ELEMENT_H
#define ELEMENT_H
#include <QString>
#include <QDate>
#include <QDataStream>
#include <windows.h>


//QDate currentDate1 = QDate::currentDate();

class element
{
public:
    QString number;
    QString startPoint;
    QString finishPoint;
    QString intermidiatePoints;
    bool checkBox;
    int priceBox;
    int fullPrice;
    unsigned int id;
    QTime fullPath;
    element();
    ~element();
    element(QString number, QString startPoint, QString finishPoint, QString intermidiatePoints, bool checkBox, int priceBox, int fullPrice, QTime fullPath);
    element(const element& other);
    int saveToFile(HANDLE handleFile ,const element & data);
    int readFromFile(HANDLE handleFile, element & data);
    bool operator>= (const element& other) const;
    bool operator== (const element& other) const;
    bool operator!= (const element& other) const;
};

inline QDataStream &operator<<(QDataStream & out,const element & data){
    out << data.number << data.startPoint << data.finishPoint << data.intermidiatePoints << data.checkBox << data.priceBox << data.fullPrice << data.fullPath << data.id;
    return out;
}

//Получение данных из потока
inline QDataStream &operator>>(QDataStream & out, element & data){
    out >> data.number >> data.startPoint >> data.finishPoint >> data.intermidiatePoints >> data.checkBox >> data.priceBox >> data.fullPrice >> data.fullPath >> data.id;
    return out;
}

#endif // ELEMENT_H

\
