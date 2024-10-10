// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "DatabaseManager.h"

#include "app_environment.h"
#include "import_qml_components_plugins.h"
#include "import_qml_plugins.h"

DatabaseManager *dbManager = nullptr;

int main(int argc, char *argv[])
{
    set_qt_environment();

 
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    dbManager = new DatabaseManager();
    engine.rootContext()->setContextProperty("dbManager", dbManager); // Expose dbManager to QML

    const QUrl url(u"qrc:Main/main.qml"_qs);
    QObject::connect(
                &engine, &QQmlApplicationEngine::objectCreated, &app,
                [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    },
    Qt::QueuedConnection);

    engine.addImportPath(QCoreApplication::applicationDirPath() + "/qml");
    engine.addImportPath(":/");

    engine.load(url);

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    int result = app.exec(); // Start the application event loop

    delete dbManager; // Clean up
    return result;
}
