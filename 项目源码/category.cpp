#include "category.h"
#include <QDebug>

#define FOLDER 0
#define DOCUMENT 1

Category::Category() {}

Category::Category(QString rootName, int fileType) {
  root = new FCB(rootName, fileType);
  currentFile = root;
  if (root == nullptr) {
    return;
  }
}

void Category::createFile(QString fileName, int fileType) {
  if (root == nullptr) {
    return;
  } else {
    FCB *temp = new FCB(fileName, fileType);
    if (currentFile->firstChild == nullptr) {
      currentFile->firstChild = temp;
      currentFile->firstChild->parent = currentFile;
    } else {
      currentFile->getLastChild()->nextBrother = temp;
      temp->parent = currentFile;
    }
    temp->parentData[0] = currentFile->filePath;
    qDebug() << "父文件路径 " << temp->parentData[0];
    temp->parentData[1] = currentFile->fileName;
    qDebug() << "父文件名称 " << temp->parentData[1];
    temp->setPath();
  }
}

void Category::deleteFolder(int index) {
  FCB *temp = currentFile->getChild(index);
  if (index == 0) {
    currentFile->firstChild = temp->nextBrother;
  } else {
    currentFile->getChild(index - 1)->nextBrother = temp->nextBrother;
  }
  cleanFile(temp->firstChild);
  delete temp;
  temp = nullptr;
}

void Category::deleteDocument(int index) {
  FCB *temp = currentFile->getChild(index);
  if (index == 0) {
    currentFile->firstChild = temp->nextBrother;
  } else {
    currentFile->getChild(index - 1)->nextBrother = temp->nextBrother;
  }
  delete temp;
}

FCB *Category::searchFile(FCB *current, QString fileName, int fileType,
                          QString filePath) { //就是currentfile
  if ((current != currentFile) && (current->fileName == fileName) &&
      (current->fileType == fileType) &&
      ((current->filePath == filePath) || (filePath == ""))) {
    return current;
  }
  if (current->firstChild == nullptr) {
    return nullptr;
  } else {
    FCB *temp = current->firstChild;
    while (temp != nullptr) {
      FCB *result = searchFile(temp, fileName, fileType, filePath);
      if (result != nullptr) {
        return result;
      } else {
        temp = temp->nextBrother;
      }
    }
  }
  return nullptr;
}

void Category::cleanFile(FCB *fcb) {
  if (fcb == nullptr) {
    return;
  } else {
    cleanFile(fcb->firstChild);
    cleanFile(fcb->nextBrother);
    delete fcb;
    fcb = nullptr;
  }
}

bool Category::judgeSameName(QString fileName, int fileType) {
  if (currentFile->firstChild == nullptr) {
    return true;
  } else {
    FCB *temp = currentFile->firstChild;
    while (temp != nullptr) {
      if ((temp->fileName == fileName) && (temp->fileType == fileType)) {
        return false;
      } else {
        temp = temp->nextBrother;
      }
    }
  }
  return true;
}

void Category::findInsert(FCB *fcb, QString filePath, QString fileName) {
  if (filePath == "root") {
    if (root->firstChild == nullptr) {
      root->firstChild = fcb;
      fcb->parent = root;
      qDebug() << "测试三下";
    } else {
      root->getLastChild()->nextBrother = fcb;
      fcb->parent = root;
    }
  } else {
    FCB *result = searchFile(root, fileName, FOLDER, filePath);
    if (result->firstChild == nullptr) {
      result->firstChild = fcb;
      fcb->parent = result;
    } else {
      result->getLastChild()->nextBrother = fcb;
      fcb->parent = result;
    }
  }
}
