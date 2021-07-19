#ifndef BLOCK_H
#define BLOCK_H

#include <QString>

class Block {
public:
  Block();
  void setData(QString data);
  QString getData();
  void setSize(int maxSize);

  int maxSize = 0;
  int size = 0;
  QString data = "";
};

#endif // BLOCK_H
