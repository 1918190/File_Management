#include "textwindow.h"
#include "ui_textwindow.h"
#include <QDebug>

TextWindow::TextWindow(QWidget *parent)
    : QDialog(parent), ui(new Ui::TextWindow) {
  ui->setupUi(this);
  setWindowFlags(windowFlags() &
                 ~Qt::WindowMaximizeButtonHint); // 禁止最大化按钮
  setFixedSize(this->width(), this->height());
  ui->textEdit->setLineWrapMode(QTextEdit::NoWrap);
  statusBar = new QStatusBar(this);
  position = new QLabel();
  byte = new QLabel();
  statusBar->addPermanentWidget(position);
  statusBar->addWidget(byte);
  statusBar->setGeometry(0, 430, 560, 25);
  menuBar = new QMenuBar(this);
  menuBar->setGeometry(0, 0, 150, 25);
  initMenu();
  ui->fontComboBox->setEditable(false);
  this->setWindowIcon(QIcon(":/image/document.png"));
}

void TextWindow::initMenu() {
  QMenu *fileMenu = menuBar->addMenu("文件");
  QMenu *edit = menuBar->addMenu("编辑");
  QMenu *help = menuBar->addMenu("帮助");
  QAction *save = fileMenu->addAction("保存");
  fileMenu->addAction(save);
  fileMenu->addSeparator();
  QAction *exit = fileMenu->addAction("退出");
  fileMenu->addAction(exit);
  QAction *clear = edit->addAction("清空");
  edit->addAction(clear);
  edit->addSeparator();
  QMenu *type = edit->addMenu("格式");
  edit->addMenu(type);
  QAction *about = help->addAction("关于");
  help->addAction(about);
  QMenu *item = type->addMenu("风格");
  type->addMenu(item);
  QAction *init = item->addAction("默认");
  item->addAction(init);
  QAction *under = item->addAction("下划线");
  item->addAction(under);
  QAction *bold = item->addAction("粗体");
  item->addAction(bold);
  QAction *italic = item->addAction("斜体");
  item->addAction(italic);

  menuBar->addMenu(fileMenu);
  menuBar->addMenu(edit);
  menuBar->addMenu(help);
  connect(save, SIGNAL(triggered(bool)), this, SLOT(save()));
  connect(exit, SIGNAL(triggered(bool)), this, SLOT(closeWindow()));
  connect(clear, SIGNAL(triggered(bool)), this, SLOT(clear()));
  connect(about, SIGNAL(triggered(bool)), this, SLOT(setAbout()));
  connect(init, SIGNAL(triggered(bool)), this, SLOT(formal()));
  connect(under, SIGNAL(triggered(bool)), this, SLOT(underLine()));
  connect(bold, SIGNAL(triggered(bool)), this, SLOT(bold()));
  connect(italic, SIGNAL(triggered(bool)), this, SLOT(italic()));
}

void TextWindow::setData(QString data) {
  ui->textEdit->setText(data);
  byte->setText(QString::number(data.toLocal8Bit().length()) + " 字节 ");
}

void TextWindow::save() {
  change = false;
  emit newData(ui->textEdit->toPlainText());
  this->setWindowTitle(title);
}

void TextWindow::setAbout() {
  QMessageBox::information(this, tr("作者"), tr("1952216翟晨昊"),
                           QMessageBox::Yes);
}

void TextWindow::clear() {
  int choose =
      QMessageBox::question(this, tr("清空"), tr("你想清空所有内容吗？"),
                            QMessageBox::Yes, QMessageBox::No);
  if (choose == QMessageBox::Yes) {
    ui->textEdit->clear();
    change = true;
  }
}

void TextWindow::closeWindow() { this->close(); }

void TextWindow::underLine() {
  QFont font = ui->textEdit->font();
  font.setUnderline(true);
  ui->textEdit->setFont(font);
}

void TextWindow::italic() {
  QFont font = ui->textEdit->font();
  font.setItalic(true);
  ui->textEdit->setFont(font);
}

void TextWindow::bold() {
  QFont font = ui->textEdit->font();
  font.setBold(true);
  ui->textEdit->setFont(font);
}

void TextWindow::formal() {
  QFont font = ui->textEdit->font();
  font.setUnderline(false);
  font.setBold(false);
  font.setItalic(false);
  ui->textEdit->setFont(font);
}

void TextWindow::setTitle(QString name) {
  title = name;
  change = false;
  this->setWindowTitle(name);
  ui->spinBox->setValue(9);
  QFont font;
  font.setFamily("SimSun");
  ui->fontComboBox->setCurrentFont(font);
}

void TextWindow::closeEvent(QCloseEvent *e) {
  qDebug() << change;
  if (change) {
    int choose = QMessageBox::question(
        this, tr("退出"), tr("你想将更改保存入文档吗？"), QMessageBox::Yes,
        QMessageBox::No, QMessageBox::Cancel);
    if (choose == QMessageBox::Yes) {
      e->accept(); //不会将事件传递给组件的父组件
      change = false;
      title = "";
      emit newData(ui->textEdit->toPlainText());
      qDebug() << ui->textEdit->toPlainText();
      ui->textEdit->clear();
    } else if (choose == QMessageBox::No) {
      e->accept();
    } else {
      e->ignore();
    }
  } else {
    e->accept();
  }
}

TextWindow::~TextWindow() { delete ui; }

void TextWindow::on_textEdit_textChanged() {
  this->setWindowTitle("*" + title);
  byte->setText(
      QString::number(ui->textEdit->toPlainText().toLocal8Bit().length()) +
      " 字节");
  change = true;
}

void TextWindow::on_textEdit_cursorPositionChanged() {
  int row = ui->textEdit->textCursor().blockNumber() + 1;
  int col = ui->textEdit->textCursor().columnNumber() + 1;
  position->setText("第 " + QString::number(row) + " 行 " + " 第 " +
                    QString::number(col) + " 列");
}

void TextWindow::on_fontComboBox_currentFontChanged(const QFont &f) {
  ui->textEdit->setFont(f.family());
}

void TextWindow::on_spinBox_valueChanged(int arg1) {
  QFont font = ui->textEdit->font();
  font.setPointSize(arg1);
  ui->textEdit->setFont(font);
}
