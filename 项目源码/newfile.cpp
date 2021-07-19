#include "newfile.h"
#include "ui_newfile.h"

NewFile::NewFile(QWidget *parent) : QDialog(parent), ui(new Ui::NewFile) {
  ui->setupUi(this);
  setFixedSize(this->width(), this->height());
}

NewFile::~NewFile() { delete ui; }

void NewFile::on_pushButton_clicked() {
  emit newName(ui->lineEdit->text());
  this->close();
}

void NewFile::cleanEdit() { ui->lineEdit->clear(); }
