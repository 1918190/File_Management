#include "fcb.h"

FCB::FCB() {}

FCB::FCB(FCB &file) {
  this->fileName = file.fileName;
  this->lastTime = file.lastTime;
  this->fileType = file.fileType;
  this->size = file.size;
  this->byteSize = file.byteSize;
}

FCB::FCB(QString fileName, int fileType) {
  this->fileName = fileName;
  this->fileType = fileType;
  this->lastTime = QDateTime::currentDateTime();
}

void FCB::setPath() {
  if ((fileType == 0) && (this->parent != nullptr)) {
    this->filePath = this->parent->filePath + " > " + fileName;
  }
}

FCB *FCB::getLastChild() {
  if (this->firstChild == nullptr) {
    return nullptr;
  }
  FCB *temp = this->firstChild;
  while (temp->nextBrother != nullptr) {
    temp = temp->nextBrother;
  }
  return temp;
}

FCB *FCB::getChild(int index) {
  if (this->firstChild == nullptr) {
    return nullptr;
  }
  FCB *temp = this->firstChild;
  for (int i = 0; i < index; i++) {
    temp = temp->nextBrother;
  }
  return temp;
}

FCB *FCB::findChild(QString fileName, int fileType) {
  FCB *temp = this->firstChild;
  while (temp != nullptr) {
    if ((temp->fileName == fileName) && (temp->fileType == fileType)) {
      return temp;
    } else {
      temp = temp->nextBrother;
    }
  }
  return nullptr;
}
