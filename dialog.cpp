#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)

{
    ui->setupUi(this);
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(on_dialog_create()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

//обработка кнопки создания
int N;
void Dialog::on_dialog_create()
{
    N = ui->valEdit->value();
    emit sendData(N); //посылаем данные
    accept();
}


Dialog::~Dialog()
{
    delete ui;
}

