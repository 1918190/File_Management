#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "bitmap.h"
#include "category.h"
#include "newfile.h"
#include "textwindow.h"
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStack>
#include <QStandardItemModel>
#include <QTreeWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  void init();
  void initCategory();
  void initBitMap();
  void updateView();
  void setView();
  void setMenu();
  void saveTree(FCB *fcb, QDataStream &outCategory);
  void updateChildPath(FCB *fcb);
  void checkBack(FCB *fcb);
  ~MainWindow();

private slots:
  void newFolder();
  void newDocument();
  void open();
  void rename();
  void updateName(QString newName);
  void deleteFile();
  void updateData(QString newData);

  void on_tableView_doubleClicked(const QModelIndex &index);

  void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

  void on_pushButton_4_clicked();

  void on_pushButton_5_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_6_clicked();

  void on_lineEdit_textEdited(const QString &arg1);

signals:
  void fileName(QString);

protected:
  void closeEvent(QCloseEvent *e);

private:
  FCB *editFile = nullptr;
  int reNameFCB = 0;
  QString ensureName(int fileType);
  void updateTreeWidget();
  QTreeWidgetItem *update(QTreeWidgetItem *item, FCB *node);
  void updateListWidget();
  void updatePath();
  void updateButton();
  Ui::MainWindow *ui;
  NewFile newWindow;
  TextWindow editWindow;
  BitMap *bitMap;
  Category *category;
  QStandardItemModel *FileModel = new QStandardItemModel(0, 4);
  QLabel *itemNum;
  QLabel *under;
  QVector<FCB *> back;
  QVector<FCB *> front;
};
#endif // MAINWINDOW_H
