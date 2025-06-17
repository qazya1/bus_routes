#include "catalog.h"
#include "ui_catalog.h"
#include <QMessageBox>
#include <QDate>
#include "element.h"
#include <QChar>
#include <QDialog>
#include <QFileDialog>
#include <QEvent>
#include <QCloseEvent>
#include <QObject>
#include <dialog.h>

#include <QMetaObject>

catalog::catalog(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::catalog)
{
    ui->setupUi(this);
    //isFileOpen = false; //флаг на открытие файла
    isModified = false; //флаг на изменение базы
    amountOfRecords = 0; //количество записей
    connect(ui->dataView,  SIGNAL(currentItemChanged (QListWidgetItem *, QListWidgetItem *)), this, SLOT(on_item_changed(QListWidgetItem *, QListWidgetItem *)));
    connect(ui->createButton, SIGNAL(clicked()), this, SLOT(slot_create()));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(slot_delete()));
    connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(slot_save()));
    connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(clear()));
    connect(ui->create10Button, SIGNAL(clicked()), this, SLOT(slot_create10()));
    connect(ui->createNButton, SIGNAL(clicked()), this, SLOT(slot_createN()));
    //connect(ui->IntermidiatePoints, &QTextEdit::textChanged, this, &MainWindow::processStopsInput);
    connect(ui->StartPoint, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_change_vis()));
    connect(ui->checkBox,SIGNAL(stateChanged(int)),this,SLOT(District()));
    connect(ui->PriceBox, SIGNAL(valueChanged(int)), this, SLOT(slot_proezd()));

    //connect(ui->action_save_as, SIGNAL(triggered(bool)), this, SLOT(on_menu_save_as()));
    //connect(ui->action_save, SIGNAL(triggered(bool)), this, SLOT(on_menu_save()));
    //connect(ui->action_open, SIGNAL(triggered(bool)), this, SLOT(on_menu_open()));

    QRegExp regex("[0-9]{7}[0-9а-яА-Я]{1}");
    QValidator *validator = new QRegExpValidator(regex, this);

    ui->Number->setValidator(validator);

    QStringList StartPoint = {"Центральный рынок", "Вокзал", "Тракторозаводской рынок", "Кинотеатр Юбилейный", "ВолГУ", "Лавровая"};
    ui->StartPoint->addItems(StartPoint);
    
        //загружаем новую базу
    database.clear();
    database.load();

    //setWindowTitle("База данных маршрутов (" + fileName + ")");

    //очищаем браузер
    ui->dataView->clear();

    //записываем в браузер
    QVector <tableItem> tmp = database.records();
    QVectorIterator <tableItem> iter(tmp);

    while(iter.hasNext())
    {
        setItemInBrowser(iter.next());
        //flag = 1;
    }

    setAfterStartingVisibility();
}
bool isComplete;

//Настройка видимости для случая, когда в базе отстутствуют записи
void catalog::setStartingVisibility(){
    ui->deleteButton->setEnabled(false);
    ui->saveButton->setEnabled(false);
    ui->dataView->setEnabled(false);
    ui->PriceBox->setEnabled(false);
    ui->checkBox->setChecked(false);
}

//Видимость после нажатия кнопки "Создать"
void catalog::setAfterStartingVisibility(){
    ui->checkBox->setChecked(false);
    ui->PriceBox->setEnabled(false);

    ui->Number->setText("");
    ui->StartPoint->setCurrentIndex(0);
    ui->FinishPoint->setCurrentIndex(0);
    ui->IntermidiatePoints->setPlainText("");
    ui->PriceBox->setValue(1);
    ui->FullPrice->setValue(50);
    ui->FullPath->setTime(QTime(0, 0));
    //Если в базе нет записей, кнопка удаления блокируется
    if (database.count() == 0) {
        ui->deleteButton->setEnabled(false);
    } else{
        ui->deleteButton->setEnabled(true);
    }

    ui->saveButton->setEnabled(true);

    ui->dataView->setEnabled(true);
}

void catalog::District()
{

    if (ui->checkBox->isChecked())
    {
        ui->PriceBox->setDisabled(false);

    }
    else
    {
        ui->PriceBox->setDisabled(true);
    }


}

void catalog::slot_proezd()
{
    int price = ui->PriceBox->value();
    ui->FullPrice->setMinimum(price);
    ui->FullPrice->setMaximum(50);
}

void catalog::validateStops()
{
    QString text = ui->IntermidiatePoints->toPlainText();
    QStringList list = text.split("\n", QString::SkipEmptyParts);

    QString finalText;
    QStringList uniqueList;
    for(const QString& line : list) {
        if(line != ui->StartPoint->currentText() && line != ui->FinishPoint->currentText() && !uniqueList.contains(line)) {
            uniqueList.append(line);
            finalText += line + "\n";
        }
    }

    ui->IntermidiatePoints->setText(finalText);
}

void catalog::slot_change_vis()
{
    ui->FinishPoint->clear();
        for (const QString point : {"Центральный рынок", "Вокзал", "Тракторозаводской рынок", "Кинотеатр Юбилейный", "ВолГУ", "Лавровая"})
            {
                 if (point != ui->StartPoint->currentText()) ui->FinishPoint->addItem(point);
            }
}



//добавление элемента в браузер
void catalog::setItemInBrowser(tableItem item, int index) {
    if(index == -1)
    {
        index=ui->dataView->count();
    }

    ui->dataView ->insertItem(index, item.number + "\t" + item.startPoint + "\t"+ item.finishPoint + "\t" + QString::number(item.fullPrice) + "\n\n");
    ui->dataView->item(index)->setData(Qt::UserRole, item.id);
}

//перемещаем данные из браузера в меню
void catalog::fromDBToEdit(){
    element tmp;
    tmp = database.record(ui->dataView->item(activeRow)->data(Qt::UserRole).toUInt());

    ui->Number->setText(tmp.number);
    ui->StartPoint->setCurrentText(tmp.startPoint);
    ui->FinishPoint->setCurrentText(tmp.finishPoint);
    ui->IntermidiatePoints->setPlainText(tmp.intermidiatePoints);
    if(tmp.checkBox)
    {
        ui->checkBox->setChecked(true);
    }
    else
    {
        ui->checkBox->setChecked(false);
    }
    ui->ID->setValue(tmp.id);
    ui->PriceBox->setValue(tmp.priceBox);
    ui->FullPrice->setValue(tmp.fullPrice);
    ui->FullPath->setTime(tmp.fullPath);
}

//переносим данные из меню в браузер
int catalog::fromEditToDB() {
    element tmp;


    // Сохранение
    tmp.number = ui->Number->text();
    tmp.startPoint = ui->StartPoint->currentText();

    tmp.finishPoint = ui->FinishPoint->currentText();
    tmp.intermidiatePoints = ui->IntermidiatePoints->toPlainText();
    tmp.priceBox = ui->PriceBox->value();
    tmp.fullPath = ui->FullPath->time();

    if (ui->checkBox->isChecked())
    {
        tmp.fullPrice = 1;
    }
    else {
        tmp.fullPrice = ui->FullPrice->value();
    }

    if(database.count() == activeRow)
    {
        return database.append(tmp);
    }
    else
    {
        tmp.id = ui->dataView->item(activeRow)->data(Qt::UserRole).toUInt();
        return database.update(tmp);
    }
}

//создаем пустую запись
void catalog::slot_create()
{
    ui->createButton->setEnabled(false);
    ui->create10Button->setEnabled(false);
    ui->createNButton->setEnabled(false);
    activeRow = database.count();
    setAfterStartingVisibility();
    if (isModified) { //если в браузере есть записи, то блокируем браузер, пока изменения в создаваемой записи не будут сохранены
        ui->dataView->setEnabled(false);
    }
    ui->createButton->setEnabled(true);
    ui->create10Button->setEnabled(true);
    ui->createNButton->setEnabled(true);
}

//удаление
void catalog::slot_delete(){
    delete_record(activeRow); //удаление из памяти
    disconnect(ui->dataView,  SIGNAL(currentItemChanged (QListWidgetItem *, QListWidgetItem *)), this, SLOT(on_item_changed(QListWidgetItem *, QListWidgetItem *)));
    delete ui->dataView->takeItem(activeRow); //удаляем из браузера
    connect(ui->dataView,  SIGNAL(currentItemChanged (QListWidgetItem *, QListWidgetItem *)), this, SLOT(on_item_changed(QListWidgetItem *, QListWidgetItem *)));
    //если бд пустая, то переводим программу в начальное состояние
    if(!database.count())
    {
        activeRow = -1;
        setStartingVisibility();
    }
    else //проверяем остальные случаи для фокуса
        if(activeRow == 1 && activeRow == amountOfRecords-1)
        {
            emit on_item_changed(ui->dataView->item(activeRow),ui->dataView->item(activeRow+1));
            isModified = true;
            ui->dataView->setCurrentRow (activeRow);
        }
        else
            if(activeRow >=0 && activeRow < amountOfRecords - 1)
            {
                emit on_item_changed(ui->dataView->item(activeRow),ui->dataView->item(activeRow+1));
                activeRow++;
                isModified = true;
                ui->dataView->setCurrentRow (activeRow);
            }
            else
            {
                emit on_item_changed(ui->dataView->item(activeRow),ui->dataView->item(activeRow+1));
                isModified = true;
                ui->dataView->setCurrentRow (activeRow);
            }
}

//удаление из бд
void catalog::delete_record(int index)
{
    database.remove(ui->dataView->item(index)->data(Qt::UserRole).toUInt());
}

//добавление в бд
void catalog::insert_record(element newRecord)
{
    database.append(newRecord);
}

//обновление данных в браузере
void catalog::updateTable()
{
    //очистка таблицы
    disconnect(ui->dataView,  SIGNAL(currentItemChanged (QListWidgetItem *, QListWidgetItem *)), this, SLOT(on_item_changed(QListWidgetItem *, QListWidgetItem *)));
    ui->dataView->clear();
    connect(ui->dataView,  SIGNAL(currentItemChanged (QListWidgetItem *, QListWidgetItem *)), this, SLOT(on_item_changed(QListWidgetItem *, QListWidgetItem *)));

    //заполняем браузер
    QVector <tableItem> tmp = database.records();
    QVectorIterator <tableItem> iter(tmp);

    while(iter.hasNext())
        setItemInBrowser(iter.next());

    //установка фокуса
    ui->dataView->setCurrentRow(0);
    activeRow = ui->dataView->currentRow();
    ui->dataView->setCurrentRow(activeRow);
}

//сохранение запииси
void catalog::slot_save()
{
    validateStops();


    int position;
    //если запись новая
    if (activeRow == database.count())
    {
        position = fromEditToDB(); //записываем в бд
        setItemInBrowser(database.browserRecordIndex(position), position); //добавляем в браузер
        isModified = true; //устанавливаем флаг
    }
    else
    {
        element oldRecord = database.record(ui->dataView->item(activeRow)->data(Qt::UserRole).toUInt()); //сохраняем старые значения
        position = fromEditToDB();
        //проверяем, есть ли изменения, и меняем флаг
        if(oldRecord != database.record(ui->dataView->item(activeRow)->data(Qt::UserRole).toUInt()))
            isModified = true;

        //обновляем в браузере
        delete ui->dataView->takeItem(activeRow);
        setItemInBrowser(database.browserRecordIndex(position), position);
    }

    ui->dataView->setEnabled(true);
    ui->deleteButton->setEnabled(true);
    ui->dataView->setCurrentRow(position);
}


//выбор записи в браузере
void catalog::on_item_changed(QListWidgetItem * current, QListWidgetItem * previous)
{
    activeRow=ui->dataView->currentRow();
    if(activeRow < database.count() && activeRow>=0)
    {
        fromDBToEdit();
    }
}

//запуск сортировки
int catalog::sortElement(int index){
    return database.update(database.record(ui->dataView->item(index)->data(Qt::UserRole).toUInt()));
}



// Создаем 10 записей
void catalog::slot_create10(){
    // Настройки видимости
    setAfterStartingVisibility();
    ui->createButton->setEnabled(false);
    ui->create10Button->setEnabled(false);
    ui->createNButton->setEnabled(false);

    // Данные для 10 записей
    QString value_number[10] = { {"1111111А"},{"2222222Б"},{"3333333В"},{"4444444Г"},{"5555555Д"},{"6666666Е"},{"7777777Ж"},{"8888888З"},{"9999999И"},{"1000000Й"} };
    QString value_startPoint[10] = {"Центральный рынок", "Тракторозаводской рынок", "Вокзал", "Кинотеатр Юбилейный", "ВолГУ", "Лавровая", "Центральный рынок", "Тракторозаводской рынок", "Вокзал", "Кинотеатр Юбилейный"};
    QString value_finishPoint[10] = {"Лавровая", "Центральный рынок", "Тракторозаводской рынок", "Тракторозаводской рынок", "Кинотеатр Юбилейный", "Лавровая", "Лавровая", "Центральный рынок", "Тракторозаводской рынок", "Тракторозаводской рынок"};
    int value_priceBox[10] = { 10,15,19,14,20,18,17,16,13,11 };
    int value_fullprice[10] = { 50,50,50,50,50,50,50,50,50,50 };
    QString value_intermidiatePoints[10] = {{"Вокзал"}, {"ВолГУ"}, {"Кинотеатр Юбилейный"}, {"Вокзал"}, {"Тракторозаводской рынок"}, {"Вокзал"}, {"Вокзал"}, {"ВолГУ"}, {"Кинотеатр Юбилейный"}, {"Вокзал"}};

    // Запись данных
    for(int i = 0; i < 10; i++) {
        activeRow = database.count();
        ui->Number->setText(value_number[i]);
        ui->StartPoint->setCurrentText(value_startPoint[i]);
        ui->FinishPoint->setCurrentText(value_finishPoint[i]);
        ui->IntermidiatePoints->setPlainText(value_intermidiatePoints[i]);
        ui->checkBox->setChecked(true);
        ui->PriceBox->setValue(value_priceBox[i]);
        ui->FullPrice->setValue(value_fullprice[i]);
        ui->FullPath->setTime(QTime::currentTime());
        slot_save();
    }
    ui->createButton->setEnabled(true);
    ui->create10Button->setEnabled(true);
    ui->createNButton->setEnabled(true);
}


//диалоговое окно
void catalog::slot_createN(){

    ui->createButton->setEnabled(false);
    ui->create10Button->setEnabled(false);
    ui->createNButton->setEnabled(false);
    Dialog dialog(this);
    connect(&dialog, &Dialog::sendData, this, &catalog::receiveDataFromDialog);
    dialog.exec();
    if (isComplete) {
        dialog.accept();
    }
    ui->createButton->setEnabled(true);
    ui->create10Button->setEnabled(true);
    ui->createNButton->setEnabled(true);

}

//работа с данными из диалога
void catalog::receiveDataFromDialog(int data){

    QString value_number = "1111111А";
    QString value_startPoint = "Вокзал";
    QString value_finishPoint = "Кинотеатр Юбилейный";
    QString value_intermidiatePoints = "ВолГУ";
    int value_priceBox = 20;
    int value_fullprice = 50;
    for (int i = 0; i < data; i++) {
        activeRow = database.count();
        ui->Number->setText(value_number);
        ui->StartPoint->setCurrentText(value_startPoint);
        ui->FinishPoint->setCurrentText(value_finishPoint);
        ui->IntermidiatePoints->setPlainText(value_intermidiatePoints);
        ui->checkBox->setChecked(true);
        ui->PriceBox->setValue(value_priceBox);
        ui->FullPrice->setValue(value_fullprice);
        ui->FullPath->setTime(QTime::currentTime());
        slot_save();
    }
    setAfterStartingVisibility();


    QMessageBox::information(this, "Отчет о добавлении записей", "Добавлено записей: "+QString::number(data), QMessageBox::Ok, QMessageBox::Ok);
    isModified = true;
    isComplete = 1;

}

//обработка крестика
void catalog::closeEvent(QCloseEvent * event)
{
    database.save();
}

void catalog::clear()
{
    database.clear();
    setAfterStartingVisibility();
    ui->dataView->clear();
}

catalog::~catalog()
{
    delete ui;
}
