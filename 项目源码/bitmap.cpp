#include "bitmap.h"
#include <QDebug>

#define EMPTY -1
#define END -2
#define FOLDER 0
#define DOCUMENT 1

BitMap::BitMap() {}

BitMap::BitMap(int capacity, int blockSize) {
  this->capacity = capacity;
  this->blockSize = blockSize;
  this->blockNum = capacity / blockSize;
  this->remainNum = this->blockNum;
  qDebug() << this->remainNum;
  bitMap = new int[blockNum];
  memory = new Block[blockNum];
  for (int i = 0; i < blockNum; i++) {
    bitMap[i] = EMPTY;
    memory[i].setSize(blockSize);
  }
}

QString BitMap::getData(int i) { return memory[i].getData(); }

bool BitMap::setSpace(FCB *fcb, QString data) {
  if (fcb->size > remainNum) {
    return false;
  } else {
    int prev = 0;
    int count = 0;
    fcb->byteSize = data.toLocal8Bit().length();
    for (int i = 0; i < blockNum; i++) {
      if (bitMap[i] == EMPTY) {
        remainNum--;
        count++;
        fcb->start = i;
        prev = i;
        memory[i].setData(data.mid(0, blockSize));
        bitMap[i] = END;
        break;
      }
    }
    for (int i = prev + 1; i < blockNum; i++) {
      if (count < fcb->size) {
        if (bitMap[i] == EMPTY) {
          remainNum--;
          bitMap[prev] = i;
          prev = i;
          qDebug() << "count" << count;
          memory[i].setData(data.mid(count * blockSize, blockSize));
          count++;
        }
      } else {
        break;
      }
    }
    qDebug() << "count" << count;
    bitMap[prev] = END;
    return true;
  }
}

QString BitMap::getFileData(FCB *fcb) {
  if (fcb->start == EMPTY) {
    return "";
  } else {
    QString data = "";
    int current = fcb->start;
    while (current != END) {
      data += memory[current].getData();
      current = bitMap[current];
    }
    return data;
  }
}

void BitMap::deleteFileData(FCB *fcb) {
  if (fcb->size <= 0) {
    return;
  } else {
    int next = fcb->start;
    int prev = fcb->start;
    while (next != END) {
      memory[next].setData("");
      next = bitMap[prev];
      bitMap[prev] = EMPTY;
      prev = next;
      remainNum++;
    }
  }
}

void BitMap::cleanFileData(FCB *fcb) {
  if (fcb == nullptr) {
    return;
  } else {
    cleanFileData(fcb->firstChild);
    cleanFileData(fcb->nextBrother);
    if (fcb->fileType == DOCUMENT) {
      deleteFileData(fcb);
    }
  }
}

void BitMap::updateFileData(FCB *fcb, QString newData) {
  deleteFileData(fcb);
  fcb->size = ceil(newData.length() / (double)blockSize);
  fcb->start = -1;
  qDebug() << "fcb大小：" << fcb->size;
  setSpace(fcb, newData);
}
