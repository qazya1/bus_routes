#include "catalogdb.h"
#include "QFile"
#include <windows.h>

catalogdb::catalogdb(void)
{
    genID = 0; //глобальный идентификатор
}

catalogdb::~catalogdb(void) {
}

//получаем позицию
unsigned int catalogdb::positionNewRecord(const element record) {
    unsigned int position = 0;

    QListIterator<element> iter(elementbase);
    while (iter.hasNext() && record >= iter.next())
        position++;

    return position;
}

//добавляем в бд
unsigned int catalogdb::append(element record) {
    int pos;
    record.id = genID;
    genID++;
    pos = positionNewRecord(record);
    elementbase.insert(pos, record);  //вставляем

    return pos;
}


//удаляем из бд
void catalogdb::remove(unsigned int id) {
    QMutableListIterator <element> iter(elementbase);
    while (iter.hasNext())
        if (iter.next().id == id)
        {
            iter.remove();
            return;
        }

}


//изменение записи
int catalogdb::update(const element record) {
    remove(record.id);//удаляем запись из бд
    int position = positionNewRecord(record);
    elementbase.insert(position, record);

    return position; //получаем новую позицию
}

//получаем запись по идентификатору
const element catalogdb::record(unsigned int id){
    QListIterator <element> iter(elementbase);
    while (iter.hasNext())
        if (iter.next().id == id)
        {
            return iter.previous();
        }

    element tmp;
    return tmp;

}

//получение записей для браузера
const QVector <tableItem> catalogdb::records(){
        QVector <tableItem> elementData;
        tableItem tmpItem;
        element tmpRecord;

        QListIterator <element> iter(elementbase);

        while (iter.hasNext())
        {
            tmpRecord = iter.next();

            tmpItem.number = tmpRecord.number;
            tmpItem.startPoint = tmpRecord.startPoint;
            tmpItem.finishPoint = tmpRecord.finishPoint;
            tmpItem.fullPrice = tmpRecord.fullPrice;
            tmpItem.id = tmpRecord.id;

            elementData << tmpItem;
        }

        return elementData;
}

//подсчёт элементов в бд
unsigned int catalogdb::count()
{
    return elementbase.count();
}

//получение структуры из полей браузера
const tableItem catalogdb::browserRecordIndex(unsigned int index){
    tableItem tmp;

    tmp.number = elementbase[index].number;
    tmp.startPoint = elementbase[index].startPoint;
    tmp.finishPoint = elementbase[index].finishPoint;
    tmp.fullPrice = elementbase[index].fullPrice;
    tmp.id = elementbase[index].id;

    return tmp;
}

void catalogdb::save() {
    HANDLE handleFile = CreateFile(L"source.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    QListIterator <element> iter(elementbase);
    element el;
    while (iter.hasNext())
    {
        el = iter.next();
        el.saveToFile(handleFile, el);
    }
    CloseHandle(handleFile);
}

//загрузка данных
void catalogdb::load(){
    HANDLE handleFile = CreateFile(L"source.txt", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (handleFile == INVALID_HANDLE_VALUE) return;
    int bytesRead;
    while (true)
    {
        element tmp;
        bytesRead = tmp.readFromFile(handleFile, tmp);
        if (bytesRead == 0) break;
        elementbase.insert(tmp.id, tmp);
    }
    CloseHandle(handleFile);
    QListIterator <element> iter(elementbase);
    element tmpRecord;
    while (iter.hasNext())
    {
        unsigned int tmpId = iter.next().id;
        if (genID < tmpId) genID = tmpId;
    }
    genID++;
}

//очистка бд
void catalogdb::clear()
{
    elementbase.clear();
    genID = 0;
}
