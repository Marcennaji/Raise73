#include "DatabaseManager.h"

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent) {
    db = QSqlDatabase::addDatabase("QSQLITE");
}

DatabaseManager::~DatabaseManager() {
    if (db.isOpen()) {
        db.close();
    }
}

bool DatabaseManager::openDatabase(const QString &path) {
    db.setDatabaseName(path);
    if (!db.open()) {
        qDebug() << "Error: connection with database failed";
        return false;
    } else {
        qDebug() << "Database: connection ok";
        return true;
    }
}

void DatabaseManager::closeDatabase() {
    if (db.isOpen()) {
        db.close();
        qDebug() << "Database: connection closed";
    }
}

QVariantList DatabaseManager::executeQuery(const QString &queryStr) {
    QVariantList results;
    QSqlQuery query;
    if (!query.exec(queryStr)) {
        qDebug() << "Error: failed to execute query";
        qDebug() << query.lastError();
        return results;
    }

    while (query.next()) {
        QVariantMap row;
        for (int i = 0; i < query.record().count(); ++i) {
            row[query.record().fieldName(i)] = query.value(i);
        }
        results.append(row);
    }

    return results;
}
