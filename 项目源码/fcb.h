#ifndef FCB_H
#define FCB_H

#include <QDateTime>
#include <QString>

class FCB {
public:
  FCB();
  FCB(FCB &file);
  FCB(QString fileName, int fileType);
  void setPath();
  FCB *getLastChild();
  FCB *findChild(QString fileName, int fileType);
  FCB *getChild(int index);
  QString filePath = "root";
  QString fileName;   //文件名称
  int fileType;       //文件类型
  QDateTime lastTime; //最近修改时间
  int size = 0;       // 文件所占块数
  int byteSize = 0;   //文件大小
  int start = -1;     //起始位置
  QString parentData[2] = {"", ""};
  FCB *firstChild = nullptr;
  FCB *nextBrother = nullptr;
  FCB *parent = nullptr;
};

#endif // FCB_H
