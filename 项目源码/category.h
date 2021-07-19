#ifndef CATEGORY_H
#define CATEGORY_H

#include "fcb.h"

class Category {
public:
  Category();
  Category(QString rootName, int fileType);
  void createFile(QString fileName, int fileType);
  void deleteFolder(int index);
  void deleteDocument(int index);
  FCB *searchFile(FCB *current, QString fileName, int fileType,
                  QString filePath);
  void cleanFile(FCB *fcb);
  bool judgeSameName(QString fileName, int fileType);
  void findInsert(FCB *fcb, QString filePath, QString fileName);
  FCB *currentFile = nullptr;
  FCB *root = nullptr;
};

#endif // CATEGORY_H
