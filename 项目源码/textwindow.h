#ifndef TEXTWINDOW_H
#define TEXTWINDOW_H

#include <QDialog>
#include <QIcon>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStatusBar>
#include <QTextEdit>

namespace Ui {
class TextWindow;
}

class TextWindow : public QDialog {
  Q_OBJECT

public:
  explicit TextWindow(QWidget *parent = nullptr);
  void setData(QString data);
  void initMenu();
  ~TextWindow();

signals:
  void newData(QString);

protected:
  void closeEvent(QCloseEvent *e);

private slots:
  void on_textEdit_textChanged();
  void setTitle(QString name);
  void save();
  void setAbout();
  void clear();
  void closeWindow();
  void underLine();
  void italic();
  void bold();
  void formal();
  void on_textEdit_cursorPositionChanged();

  void on_fontComboBox_currentFontChanged(const QFont &f);

  void on_spinBox_valueChanged(int arg1);

private:
  QString title;
  QLabel *position;
  QLabel *byte;
  bool change = false;
  Ui::TextWindow *ui;
  QStatusBar *statusBar;
  QMenuBar *menuBar;
};

#endif // TEXTWINDOW_H
