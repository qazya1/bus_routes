#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QWidget>
#include "element.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();
    element * elements;

private:
    Ui::Dialog *ui;

signals:
    void sendData(int data);

private slots:
        void on_dialog_create();
};

#endif // DIALOG_H
