#ifndef CPPINTERFACE_H
#define CPPINTERFACE_H

#include <QObject>

class CppInterface : public QObject
{
    Q_OBJECT
public:
    explicit CppInterface(QObject *parent = nullptr);

public slots:
    void startNewGame();
};

#endif // CPPINTERFACE_H
