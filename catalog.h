#ifndef CATALOG_H
#define CATALOG_H

#include <QMainWindow>
#include "element.h"
#include "catalogdb.h"
#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class catalog; }
QT_END_NAMESPACE

class catalog : public QMainWindow
{
    Q_OBJECT

public:
    catalog(QWidget *parent = nullptr);
    ~catalog();
private:
    Ui::catalog *ui;
    catalogdb database;//работа с базой данных
    bool isModified;// 1 ~ в базе данных есть не сохранённые изменения

    int amountOfRecords;//кол-во записей
    int activeRow;//активная запись
    void setItemInBrowser(tableItem item, int index = -1);
    void setStartingVisibility();
    void setAfterStartingVisibility();
    int fromEditToDB();
    void fromDBToEdit();
    void addToBrowser(tableItem item, int index = -1);
    void delete_record(int);
    void insert_record(element);
    void updateTable();
    int sortElement(int);
    bool sort();



private slots:
    void receiveDataFromDialog(int data);
    void slot_create();
    void slot_delete();
    void slot_save();
    void on_item_changed( QListWidgetItem * current, QListWidgetItem * previous );

    void slot_create10();
    void slot_createN();

    void District();

    void slot_change_vis();

    /*void on_menu_open();
    void on_menu_save();
    void on_menu_save_as();*/
    void validateStops();
    void slot_proezd();
    void clear();

protected:
    virtual void closeEvent( QCloseEvent * event );
};
#endif // CATALOG_H
