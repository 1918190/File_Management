#include "block.h"
#include <QDebug>

Block::Block() {}

void Block::setData(QString data) {
  size = (data.length() > this->maxSize) ? this->maxSize : data.length();
  qDebug() << "该块大小：" << size;
  this->data.resize(size);
  for (int i = 0; i < size; i++) {
    this->data[i] = data[i];
  }
}

QString Block::getData() { return this->data; }

void Block::setSize(int maxSize) { this->maxSize = maxSize; }
