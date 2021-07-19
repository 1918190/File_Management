#ifndef BITMAP_H
#define BITMAP_H

#include "fcb.h"
#include "math.h"
#include <block.h>

class BitMap {
public:
  BitMap();
  BitMap(int capacity, int blockSize);
  QString getData(int i);
  bool setSpace(FCB *fcb, QString data);
  QString getFileData(FCB *fcb);
  void deleteFileData(FCB *fcb);
  void cleanFileData(FCB *fcb);
  void updateFileData(FCB *fcb, QString newData);

  int remainNum;
  int capacity;
  int blockNum;
  int blockSize;
  int *bitMap = nullptr;
  Block *memory = nullptr;
};

#endif // BITMAP_H
