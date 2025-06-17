#ifndef CATALOGDB_H
#define CATALOGDB_H
#include "element.h"
#include "tableitem.h"

class catalogdb
{
public:
    catalogdb(void);
    ~catalogdb(void);

    unsigned int append(element record);
    void remove(unsigned int id);
    int update(const element record);
    const element record( unsigned int id );
    unsigned int count();
    const QVector <tableItem> records();
    const tableItem browserRecordIndex(unsigned int index);
    void save();
    void load();
    void clear();

private:
    unsigned int positionNewRecord(const element record);
    QList <element> elementbase;
    unsigned int genID;
};

#endif // CATALOGDB_H
