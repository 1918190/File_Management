#ifndef NEWFILE_H
#define NEWFILE_H

#include <QDialog>
#include <QString>

namespace Ui {
class NewFile;
}

class NewFile : public QDialog {
  Q_OBJECT

public:
  explicit NewFile(QWidget *parent = nullptr);
  void cleanEdit();
  ~NewFile();

private slots:
  void on_pushButton_clicked();

signals:
  void newName(QString);

private:
  Ui::NewFile *ui;
};

#endif // NEWFILE_H
