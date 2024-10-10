#include "CppInterface.h"
#include <qdebug.h>

CppInterface::CppInterface(QObject *parent) : QObject(parent)
{

}
void CppInterface::startNewGame()
{
    qDebug() << "started new game";

}
