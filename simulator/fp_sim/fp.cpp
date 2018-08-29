#include "fp.h"
#include "ui_fp.h"

fp::fp(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::fp)
{
    ui->setupUi(this);
}

fp::~fp()
{
    delete ui;
}
