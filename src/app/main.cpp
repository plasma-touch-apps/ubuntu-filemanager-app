/*
 * Copyright: 2013 - 2014 Canonical, Ltd
 *
 * This file is part of ubuntu-filemanager-app
 *
 * ubuntu-filemanager-app is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ubuntu-filemanager-app is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Michael Zanetti <michael.zanetti@canonical.com>
 *          Riccardo Padovani <rpadovani@ubuntu.com>
 *          David Planella <david.planella@ubuntu.com>
 */

#include <QtGui/QGuiApplication>
#include <QtQuick/QQuickView>
#include <QtQml/QtQml>
#include <QLibrary>
#include <QDir>

#include <QDebug>

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);
    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);

    // Set up import paths
    QStringList importPathList = view.engine()->importPathList();
    // Prepend the location of the plugin in the build dir,
    // so that Qt Creator finds it there, thus overriding the one installed
    // in the sistem if there is one
    importPathList.prepend(QCoreApplication::applicationDirPath() + "/../plugin/");

    QStringList args = a.arguments();
    if (args.contains("-h") || args.contains("--help")) {
        qDebug() << "usage: " + args.at(0) + " [-p|--phone] [-t|--tablet] [-h|--help] [-I <path>]";
        qDebug() << "    -p|--phone    If running on Desktop, start in a phone sized window.";
        qDebug() << "    -t|--tablet   If running on Desktop, start in a tablet sized window.";
        qDebug() << "    --forceAuth <true|false> Force authentication on or off.";
        qDebug() << "    -h|--help     Print this help.";
        qDebug() << "    -I <path>     Give a path for an additional QML import directory. May be used multiple times.";
        qDebug() << "    -q <qmlfile>  Give an alternative location for the main qml file.";
        return 0;
    }

    QString qmlfile;
    // Desktop doesn't have yet Unity8 and so no unity greeter either. Consequently it doesn't
    // also have any "PIN code" or "Password" extra authentication. Don't require any extra
    // authentication there by default
    if (qgetenv("QT_QPA_PLATFORM") != "ubuntumirclient") {
        qDebug() << Q_FUNC_INFO << "Running on non-MIR desktop, not requiring authentication by default";
        view.engine()->rootContext()->setContextProperty("noAuthentication", QVariant(true));
    } else {
        view.engine()->rootContext()->setContextProperty("noAuthentication", QVariant(false));
    }
    for (int i = 0; i < args.count(); i++) {
        if (args.at(i) == "-I" && args.count() > i + 1) {
            QString addedPath = args.at(i+1);
            if (addedPath.startsWith('.')) {
                addedPath = addedPath.right(addedPath.length() - 1);
                addedPath.prepend(QDir::currentPath());
            }
            importPathList.append(addedPath);
        } else if (args.at(i) == "-q" && args.count() > i + 1) {
            qmlfile = args.at(i+1);
        } else if (args.at(i) == "--forceAuth" && args.count() > i + 1) {
            QString value = args.at(i+1);
            if (value == "true") {
                qDebug() << Q_FUNC_INFO << "Forcing authentication on";
                view.engine()->rootContext()->setContextProperty("noAuthentication", QVariant(false));
            } else if (value == "false") {
                qDebug() << Q_FUNC_INFO << "Forcing authentication off";
                view.engine()->rootContext()->setContextProperty("noAuthentication", QVariant(true));
            } else {
                qWarning() << Q_FUNC_INFO << "Invalid forceAuth option '" << value << "', keeping default";
            }
        }
    }

    if (args.contains(QLatin1String("-testability")) || getenv("QT_LOAD_TESTABILITY")) {
        QLibrary testLib(QLatin1String("qttestability"));
        if (testLib.load()) {
            typedef void (*TasInitialize)(void);
            TasInitialize initFunction = (TasInitialize)testLib.resolve("qt_testability_init");
            if (initFunction) {
                initFunction();
            } else {
                qCritical("Library qttestability resolve failed!");
            }
        } else {
            qCritical("Library qttestability load failed!");
        }
    }

    view.engine()->rootContext()->setContextProperty("tablet", QVariant(false));
    view.engine()->rootContext()->setContextProperty("phone", QVariant(false));

    if (args.contains("-t") || args.contains("--tablet")) {
        qDebug() << "running in tablet mode";
        view.engine()->rootContext()->setContextProperty("tablet", QVariant(true));
    } else if (args.contains("-p") || args.contains("--phone")){
        qDebug() << "running in phone mode";
        view.engine()->rootContext()->setContextProperty("phone", QVariant(true));
    } else if (qgetenv("QT_QPA_PLATFORM") != "ubuntumirclient") {
        // Default to tablet size on X11
        view.engine()->rootContext()->setContextProperty("tablet", QVariant(true));
    }

    view.engine()->setImportPathList(importPathList);

    // load the qml file
    if (qmlfile.isEmpty()) {
        QStringList paths = QStandardPaths::standardLocations(QStandardPaths::DataLocation);
        paths.prepend(QDir::currentPath());
        paths.prepend(QCoreApplication::applicationDirPath());

        foreach (const QString &path, paths) {
            QFileInfo fi(path + "/qml/filemanager.qml");
            qDebug() << "Trying to load QML from:" << path + "/qml/filemanager.qml";
            if (fi.exists()) {
                qmlfile = path +  "/qml/filemanager.qml";
                break;
            }
        }
    }
    qDebug() << "using main qml file from:" << qmlfile;
    view.setSource(QUrl::fromLocalFile(qmlfile));
    view.show();

    return a.exec();
}
