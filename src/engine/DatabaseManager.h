#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QDebug>

class DatabaseManager : public QObject {
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    Q_INVOKABLE bool openDatabase(const QString &path);
    Q_INVOKABLE void closeDatabase();
    Q_INVOKABLE QVariantList executeQuery(const QString &queryStr);

private:
    QSqlDatabase db;
};

#endif // DATABASEMANAGER_H

