#include "element.h"
#include <QString>
#include <QBuffer>
#include <QTextCodec>

element::element() //базовые данные
{
    this->number = "";
    this->startPoint = "";
    this->finishPoint = "";
    this->intermidiatePoints = "";
    this->checkBox = false;
    this->priceBox = 1;
    this->fullPrice;
    this->fullPath;
}

element::element(const element &other) {
    *this = other;
}

//конструктор по умолчанию
element::element(QString number, QString startPoint, QString finishPoint, QString intermidiatePoints, bool checkBox, int priceBox, int fullPrice, QTime fullPath) {
    this->number = number;
    this->startPoint = startPoint;
    this->finishPoint = finishPoint;
    this->intermidiatePoints = intermidiatePoints;
    this->checkBox = checkBox;
    this->priceBox = priceBox;
    this->fullPrice = fullPrice;
    this->fullPath = fullPath;
}

element::~element() {
}

bool element::operator>= (const element& other) const
{
    if (id >= other.id)  return true;
    return false;
}

bool element::operator== (const element & other) const {
    if (id == other.id)
        return true;
    else
        return false;
}

bool element::operator!= (const element& other) const
{
    return ! operator== (other);
}

int element::saveToFile(HANDLE handleFile,const element & data)
{
    DWORD bytes_number;
    int sumBytes = 0;
    //int size;
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    QDataStream out(&buffer);

    out << data.number.toUtf8();
    out << data.startPoint.toUtf8();
    out << data.finishPoint.toUtf8();
    out << data.intermidiatePoints.toUtf8();
    out << data.checkBox;
    out << data.priceBox;
    out << data.fullPrice;
    out << data.fullPath;
    out << data.id;

    int size = buffer.size();
    WriteFile(handleFile, &size, sizeof(int), &bytes_number,NULL);
    sumBytes += bytes_number;
    WriteFile(handleFile, buffer.buffer(), buffer.size(), &bytes_number,NULL);
    sumBytes += bytes_number;
    buffer.close();
    return sumBytes;
}

int element::readFromFile(HANDLE handleFile, element & data)
{
    DWORD bytes_number;
    int sumBytes = 0;
    //int size;
    QByteArray byteArray;
    int qbufSize;

    ReadFile(handleFile, &qbufSize, sizeof(int), &bytes_number,NULL);
    if (bytes_number == 0) return 0;
    sumBytes += bytes_number;

    byteArray.resize(qbufSize);

    ReadFile(handleFile, byteArray.data(), qbufSize, &bytes_number,NULL);
    sumBytes += bytes_number;

    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::ReadOnly);
    QDataStream in(&buffer);

    QByteArray numberBytes, startPointBytes, finishPointBytes, intermidiatePointsBytes;
    in >> numberBytes;
    in >> startPointBytes;
    in >> finishPointBytes;
    in >> intermidiatePointsBytes;
    in >> data.checkBox;
    in >> data.priceBox;
    in >> data.fullPrice;
    in >> data.fullPath;
    in >> data.id;
    data.number = QTextCodec::codecForMib(106)->toUnicode(numberBytes);
    data.startPoint = QTextCodec::codecForMib(106)->toUnicode(startPointBytes);
    data.finishPoint = QTextCodec::codecForMib(106)->toUnicode(finishPointBytes);
    data.intermidiatePoints = QTextCodec::codecForMib(106)->toUnicode(intermidiatePointsBytes);
    buffer.close();
    return sumBytes;
}
