#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

#define FOLDER 0
#define DOCUMENT 1

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  setWindowFlags(windowFlags() &
                 ~Qt::WindowMaximizeButtonHint); // 禁止最大化按钮
  setFixedSize(this->width(), this->height());
  this->setWindowTitle("FileManager");
  this->setWindowIcon(QIcon(":/image/folder.png"));
  bitMap = new BitMap(1024 * 1024 * 5, 64);
  category = new Category("root", FOLDER);
  under = new QLabel();
  ui->statusbar->addPermanentWidget(under);
  initCategory();
  initBitMap();
  init();
}

void MainWindow::init() {
  connect(&newWindow, SIGNAL(newName(QString)), this,
          SLOT(updateName(QString)));
  connect(&editWindow, SIGNAL(newData(QString)), this,
          SLOT(updateData(QString)));
  connect(this, SIGNAL(fileName(QString)), &editWindow,
          SLOT(setTitle(QString)));
  setMenu();
  setView();
  ui->treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
  ui->treeWidget->header()->setStretchLastSection(false);
  ui->treeWidget->setIconSize(QSize(15, 25));
  ui->pushButton->setIcon(QIcon(":/image/back.png"));
  ui->pushButton->setIconSize(QSize(15, 25));
  ui->pushButton_2->setIcon(QIcon(":/image/front.png"));
  ui->pushButton_2->setIconSize(QSize(15, 25));
  ui->pushButton_3->setIcon(QIcon(":/image/up.png"));
  ui->pushButton_3->setIconSize(QSize(15, 25));
  ui->pushButton_4->setIcon(QIcon(":/image/fresh.png"));
  ui->pushButton_4->setIconSize(QSize(15, 25));
  ui->pushButton_5->setIcon(QIcon(":/image/search.png"));
  ui->pushButton_5->setIconSize(QSize(15, 25));
  ui->pushButton_6->setIcon(QIcon(":/image/clear.png"));
  ui->pushButton_6->setIconSize(QSize(15, 25));
  itemNum = new QLabel();
  ui->statusbar->addWidget(itemNum);
  back.push_back(category->root);
  updateView();
}

void MainWindow::initCategory() {
  QFile file;
  file.setFileName("category.dat");
  qDebug() << "dat文件category大小：" << file.size();
  if (file.size() == 0) {
    return;
  }
  if (!file.open(QIODevice::ReadOnly)) {
    qDebug() << "error!";
    return;
  }
  QDataStream in(&file);
  while (file.atEnd() == false) {
    qDebug() << "测试一下";
    FCB *fcb = new FCB();
    in >> fcb->filePath >> fcb->fileName >> fcb->fileType >> fcb->lastTime >>
        fcb->size >> fcb->byteSize >> fcb->start >> fcb->parentData[0] >>
        fcb->parentData[1];
    qDebug() << "测试两下";
    category->findInsert(fcb, fcb->parentData[0], fcb->parentData[1]);
  }
  file.close();
}

void MainWindow::initBitMap() {
  QFile file;
  file.setFileName("bitMap.dat");
  qDebug() << "dat文件bitmap大小：" << file.size();
  if (file.size() == 0) {
    return;
  }
  if (!file.open(QIODevice::ReadOnly)) {
    qDebug() << "error!";
    return;
  }
  QDataStream in(&file);
  in >> bitMap->remainNum >> bitMap->capacity >> bitMap->blockNum >>
      bitMap->blockSize;
  for (int i = 0; i < bitMap->blockNum; i++) {
    in >> bitMap->bitMap[i];
  }
  for (int i = 0; i < bitMap->blockNum; i++) {
    in >> bitMap->memory[i].maxSize >> bitMap->memory[i].size >>
        bitMap->memory[i].data;
  }
  qDebug() << bitMap->remainNum;
  file.close();
}

void MainWindow::setView() {
  FileModel->setColumnCount(4);
  FileModel->setRowCount(0);
  FileModel->setHeaderData(0, Qt::Horizontal, QObject::tr("文件名"));
  FileModel->setHeaderData(1, Qt::Horizontal,
                           QObject::tr("修改日期")); //指定标题行
  FileModel->setHeaderData(2, Qt::Horizontal, QObject::tr("类型"));
  FileModel->setHeaderData(3, Qt::Horizontal, QObject::tr("大小"));
  //设置表格内容不可编辑
  /*ui->tableView_3->horizontalHeader()->setSectionResizeMode(
       QHeaderView::ResizeToContents);*/
  ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->tableView->verticalHeader()->hide();
  ui->tableView->setShowGrid(false);
  ui->tableView->setModel(FileModel);
  ui->tableView->setColumnWidth(0, 280); //设置列的宽度
  ui->tableView->setColumnWidth(1, 350);
  ui->tableView->setColumnWidth(2, 150);
  ui->tableView->setColumnWidth(3, 100);
  ui->tableView->verticalHeader()->setDefaultSectionSize(15); //设置列宽
  ui->tableView->setCurrentIndex(QModelIndex());
  ui->tableView->setIconSize(QSize(20, 30));
  // ui->listWidget->setViewMode(QListView::IconMode);
}

void MainWindow::setMenu() {
  QAction *folderMenu = new QAction("新建文件夹", ui->tableView);
  QAction *documentMenu = new QAction("新建文件", ui->tableView);
  QAction *deleteMenu = new QAction("删除", ui->tableView);
  QAction *openMenu = new QAction("打开", ui->tableView);
  QAction *renameMenu = new QAction("重命名", ui->tableView);
  QList<QAction *> newMenu;
  newMenu.append(folderMenu);
  newMenu.append(documentMenu);
  ui->tableView->addActions(newMenu);
  ui->tableView->addAction(deleteMenu);
  ui->tableView->addAction(openMenu);
  ui->tableView->addAction(renameMenu);
  connect(folderMenu, SIGNAL(triggered(bool)), this, SLOT(newFolder()));
  connect(documentMenu, SIGNAL(triggered(bool)), this, SLOT(newDocument()));
  connect(openMenu, SIGNAL(triggered(bool)), this, SLOT(open()));
  connect(renameMenu, SIGNAL(triggered(bool)), this, SLOT(rename()));
  connect(deleteMenu, SIGNAL(triggered(bool)), this, SLOT(deleteFile()));
  ui->tableView->setContextMenuPolicy(Qt::ActionsContextMenu);
}

void MainWindow::updateView() {
  updateButton();
  updateListWidget();
  updateTreeWidget();
  updatePath();
}

void MainWindow::updateTreeWidget() {
  ui->treeWidget->clear();                        //清空
  ui->treeWidget->setColumnCount(1);              //设置列数
  ui->treeWidget->setHeaderLabel(tr("文件列表")); //设置头的标题
  QTreeWidgetItem *item = new QTreeWidgetItem(
      ui->treeWidget, QStringList(category->root->fileName));
  item->setWhatsThis(0, "文件夹");
  item->setIcon(0, QIcon(":/image/folder.png"));
  FCB *temp = category->root->firstChild;
  while (temp != nullptr) {
    QTreeWidgetItem *item1 = update(item, temp); //子节点1
    item->addChild(item1);
    temp = temp->nextBrother;
  }
  ui->treeWidget->expandAll(); //结点全部展开
}

QTreeWidgetItem *MainWindow::update(QTreeWidgetItem *item, FCB *node) {
  QString name = node->fileName;
  //+ ((node->fileType == FOLDER) ? "" : ".txt");
  QTreeWidgetItem *item1 = new QTreeWidgetItem(item, QStringList(name));
  if (node->fileType == FOLDER) {
    item1->setIcon(0, QIcon(":/image/folder.png"));
    item1->setWhatsThis(0, "文件夹");
  } else {
    item1->setIcon(0, QIcon(":/image/document.png"));
    item1->setWhatsThis(0, "文件");
  }
  FCB *temp = node->firstChild;
  while (temp != nullptr) {
    QTreeWidgetItem *item2 = update(item1, temp); //子节点1
    item1->addChild(item2);
    temp = temp->nextBrother;
  }
  return item1;
}

void MainWindow::updateListWidget() {
  FileModel->removeRows(0, FileModel->rowCount());
  if (category->currentFile->firstChild == nullptr) {
    itemNum->setText(" " + QString::number(FileModel->rowCount()) +
                     " 个项目  ");
    return;
  }
  FCB *temp = category->currentFile->firstChild;
  while (temp != nullptr) {
    QString name = temp->fileName + ((temp->fileType == FOLDER) ? "" : ".txt");
    QString size = (temp->fileType == DOCUMENT)
                       ? (QString::number(temp->byteSize) + "B")
                       : "";
    QString type = (temp->fileType == FOLDER) ? "文件夹" : "文件";
    QString time = temp->lastTime.toString("yyyy/MM/dd hh:mm:ss");
    QStandardItem *first = new QStandardItem(name);
    QStandardItem *second = new QStandardItem(time);
    QStandardItem *third = new QStandardItem(type);
    QStandardItem *fourth = new QStandardItem(size);
    QList<QStandardItem *> data;
    data.append(first);
    data.append(second);
    data.append(third);
    data.append(fourth);
    FileModel->appendRow(data);
    if (temp->fileType == FOLDER) {
      data[0]->setIcon(QIcon(":/image/folder.png"));
    } else {
      data[0]->setIcon(QIcon(":/image/document.png"));
    }
    temp = temp->nextBrother;
  }
  itemNum->setText(" " + QString::number(FileModel->rowCount()) + " 个项目  ");
}

void MainWindow::updatePath() {
  ui->lineEdit->setText(category->currentFile->filePath);
  under->setText("系统总空间：" + QString::number(bitMap->capacity / 1024) +
                 "KB  系统总块数：" + QString::number(bitMap->blockNum) +
                 "  系统剩余块数：" + QString::number(bitMap->remainNum));
}

void MainWindow::updateButton() {
  if (category->currentFile->parent != nullptr) {
    ui->pushButton_3->setEnabled(true);
  } else {
    ui->pushButton_3->setEnabled(false);
  }
  if (back.size() > 1) {
    ui->pushButton->setEnabled(true);
  } else {
    ui->pushButton->setEnabled(false);
  }
  if (!front.isEmpty()) {
    ui->pushButton_2->setEnabled(true);
  } else {
    ui->pushButton_2->setEnabled(false);
  }
}

QString MainWindow::ensureName(int fileType) {
  QString init = "";
  if (fileType == FOLDER) {
    init = "新建文件夹";
  } else if (fileType == DOCUMENT) {
    init = "新建文本文件";
  }
  QString name = init;
  int index = 1;
  while (!category->judgeSameName(name, fileType)) {
    name = init + "(" + QString::number(index) + ")";
    index++;
  }
  return name;
}

void MainWindow::newFolder() {
  category->createFile(ensureName(FOLDER), FOLDER);
  updateView();
}

void MainWindow::newDocument() {
  category->createFile(ensureName(DOCUMENT), DOCUMENT);
  updateView();
}

void MainWindow::open() {
  QItemSelectionModel *selectionModel = ui->tableView->selectionModel();
  QModelIndexList indexsSelected = selectionModel->selectedIndexes();
  if (indexsSelected.isEmpty()) {
    QMessageBox::warning(this, "ERROR", "请先选择一个文件或文件夹");
  } else {
    qDebug() << "选中的行：" << indexsSelected.at(0).row();
    FCB *child = category->currentFile->getChild(indexsSelected.at(0).row());
    if (child->fileType == FOLDER) {
      category->currentFile = child;
      if (back.last()->filePath != category->currentFile->filePath) {
        back.push_back(category->currentFile);
        front.clear();
      }
      updateView();
    } else if (child->fileType == DOCUMENT) {
      editWindow.setData(bitMap->getFileData(child));
      editFile = child;
      emit fileName(child->fileName);
      editWindow.exec();
    }
    qDebug() << child->parent;
  }
  ui->tableView->setCurrentIndex(QModelIndex());
}

void MainWindow::rename() {
  QItemSelectionModel *selectionModel = ui->tableView->selectionModel();
  QModelIndexList indexsSelected = selectionModel->selectedIndexes();
  if (indexsSelected.isEmpty()) {
    QMessageBox::warning(this, "ERROR", "请先选择一个文件或文件夹");
  } else {
    reNameFCB = indexsSelected.at(0).row();
    newWindow.cleanEdit();
    newWindow.exec();
  }
}

void MainWindow::updateChildPath(FCB *fcb) {
  if (fcb == nullptr) {
    return;
  }
  fcb->parentData[0] = fcb->parent->filePath;
  fcb->parentData[1] = fcb->parent->fileName;
  fcb->setPath();
  updateChildPath(fcb->firstChild);
  updateChildPath(fcb->nextBrother);
}

void MainWindow::updateName(QString newName) {
  FCB *child = category->currentFile->getChild(reNameFCB);
  if (category->judgeSameName(newName, child->fileType)) {
    child->fileName = newName;
    child->setPath();
    if (child->firstChild != nullptr) {
      updateChildPath(child->firstChild);
    }
    updateView();
  } else {
    QMessageBox::warning(this, "ERROR", "已经有相同名称的同类型文件！");
  }
}

void MainWindow::checkBack(FCB *fcb) {
  for (int i = 0; i < back.size(); i++) {
    FCB *temp = back[i];
    while (temp != nullptr) {
      if ((temp->fileName == fcb->fileName) &&
          (temp->fileType == fcb->fileType) &&
          (temp->filePath == fcb->filePath)) {
        back.remove(i);
        i--;
        break;
      } else {
        temp = temp->parent;
      }
    }
  }
  for (int i = 0; i < front.size(); i++) {
    FCB *temp = front[i];
    while (temp != nullptr) {
      if ((temp->fileName == fcb->fileName) &&
          (temp->fileType == fcb->fileType) &&
          (temp->filePath == fcb->filePath)) {
        front.remove(i);
        i--;
        break;
      } else {
        temp = temp->parent;
      }
    }
  }
}

void MainWindow::deleteFile() {
  QItemSelectionModel *selectionModel = ui->tableView->selectionModel();
  QModelIndexList indexsSelected = selectionModel->selectedIndexes();
  if (indexsSelected.isEmpty()) {
    QMessageBox::warning(this, "ERROR", "请先选择一个文件或文件夹");
  } else {
    FCB *child = category->currentFile->getChild(indexsSelected.at(0).row());
    if (child->fileType == FOLDER) {
      checkBack(child);
      bitMap->cleanFileData(child);
      category->deleteFolder(indexsSelected.at(0).row());
    } else if (child->fileType == DOCUMENT) {
      bitMap->deleteFileData(child);
      category->deleteDocument(indexsSelected.at(0).row());
    }
    updateView();
  }
}

void MainWindow::updateData(QString newData) {
  bitMap->updateFileData(editFile, newData);
  editFile->lastTime = QDateTime::currentDateTime();
  updateView();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index) {
  open();
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column) {
  if (column == 0) {
    QString type = item->whatsThis(0);
    FCB *result = category->root;
    QList<QString> path;
    while (item->parent() != nullptr) {
      path.append(item->text(0));
      item = item->parent();
    }
    if (path.length() == 0) {
      category->currentFile = result;
      if (back.last()->filePath != category->currentFile->filePath) {
        back.push_back(category->currentFile);
        front.clear();
      }
      updateView();
      return;
    }
    for (int i = path.length() - 1; i > 0; i--) {
      result = result->findChild(path[i], FOLDER);
    }
    qDebug() << "树形文件类型" << type << "  " << path.size();
    if (type == "文件夹") {
      result = result->findChild(path[0], FOLDER);
      category->currentFile = result;
      if (back.last()->filePath != category->currentFile->filePath) {
        back.push_back(category->currentFile);
        front.clear();
      }
      updateView();
    } else if (type == "文件") {
      result = result->findChild(path[0], DOCUMENT);
      editWindow.setData(bitMap->getFileData(result));
      editFile = result;
      emit fileName(result->fileName);
      editWindow.exec();
    }
  }
}

void MainWindow::saveTree(FCB *fcb, QDataStream &outCategory) {
  if (fcb == nullptr) {
    return;
  }
  outCategory << fcb->filePath << fcb->fileName << fcb->fileType
              << fcb->lastTime << fcb->size << fcb->byteSize << fcb->start
              << fcb->parentData[0] << fcb->parentData[1];
  saveTree(fcb->firstChild, outCategory);
  saveTree(fcb->nextBrother, outCategory);
}

void MainWindow::closeEvent(QCloseEvent *e) {
  QFile bitMapFile("bitMap.dat");
  bitMapFile.open(QIODevice::WriteOnly | QIODevice::Truncate); //全部清除
  QDataStream outBitMap(&bitMapFile);
  outBitMap << bitMap->remainNum << bitMap->capacity << bitMap->blockNum
            << bitMap->blockSize;
  for (int i = 0; i < bitMap->blockNum; i++) {
    outBitMap << bitMap->bitMap[i];
  }
  for (int i = 0; i < bitMap->blockNum; i++) {
    outBitMap << bitMap->memory[i].maxSize << bitMap->memory[i].size
              << bitMap->memory[i].data;
  }
  bitMapFile.close();
  QFile categoryFile("category.dat");
  categoryFile.open(QIODevice::WriteOnly | QIODevice::Truncate); //全部清除
  QDataStream outCategory(&categoryFile);
  if (category->root->firstChild != nullptr) {
    saveTree(category->root->firstChild, outCategory);
  }
  categoryFile.close();
  e->accept();
}

void MainWindow::on_pushButton_4_clicked() { updateView(); }

void MainWindow::on_pushButton_5_clicked() {
  if (ui->lineEdit_2->text().isEmpty()) {
    QMessageBox::warning(this, "ERROR", "请先输入要搜索的文件名");
  } else {
    qDebug() << ui->comboBox->currentIndex();
    FCB *item =
        category->searchFile(category->currentFile, ui->lineEdit_2->text(),
                             ui->comboBox->currentIndex(), "");
    if (item == nullptr) {
      QMessageBox::warning(this, "ERROR", "要搜索的文件不存在");
      return;
    }
    qDebug() << item->fileType;
    int type = item->fileType;
    FCB *result = category->root;
    QList<QString> path;
    while (item->parent != nullptr) {
      path.append(item->fileName);
      item = item->parent;
    }
    qDebug() << path.size();
    if (path.length() == 0) {
      category->currentFile = result;
      updateView();
      return;
    }
    for (int i = path.length() - 1; i > 0; i--) {
      result = result->findChild(path[i], FOLDER);
    }
    qDebug() << result->fileName;
    qDebug() << item->fileType;
    if (type == FOLDER) {
      result = result->findChild(path[0], FOLDER);
      category->currentFile = result;
      updateView();
    } else if (type == DOCUMENT) {
      qDebug() << result->fileName;
      result = result->findChild(path[0], DOCUMENT);
      qDebug() << result->fileName;
      editWindow.setData(bitMap->getFileData(result));
      editFile = result;
      emit fileName(result->fileName);
      editWindow.exec();
    }
  }
}

void MainWindow::on_pushButton_3_clicked() {
  if (category->currentFile->parent != nullptr) {
    category->currentFile = category->currentFile->parent;
    if (back.last()->filePath != category->currentFile->filePath) {
      back.push_back(category->currentFile);
      front.clear();
    }
    updateView();
  }
}

void MainWindow::on_pushButton_clicked() {
  qDebug() << back.last();
  qDebug() << "back大小：" << back.size();
  front.push_back(back.last());
  back.pop_back();
  category->currentFile = back.last();
  updateView();
}

void MainWindow::on_pushButton_2_clicked() {
  qDebug() << front.last();
  qDebug() << "front大小：" << front.size();
  category->currentFile = front.last();
  back.push_back(front.last());
  front.pop_back();
  updateView();
}

void MainWindow::on_pushButton_6_clicked() {
  int choose =
      QMessageBox::question(this, tr("格式化"), tr("确定要进行格式化吗？"),
                            QMessageBox::Yes, QMessageBox::No);
  if (choose == QMessageBox::Yes) {
    bitMap->cleanFileData(category->root);
    category->cleanFile(category->root->firstChild);
    category->root->firstChild = nullptr;
    back.clear();
    front.clear();
    back.push_back(category->root);
    category->currentFile = category->root;
    updateView();
    qDebug() << "恢复内存：" << bitMap->remainNum << " " << bitMap->blockNum;
  }
}

void MainWindow::on_lineEdit_textEdited(const QString &arg1) {
  ui->lineEdit->setText(category->currentFile->filePath);
}
