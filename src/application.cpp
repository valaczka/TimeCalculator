/*
 * ---- Call of Suli ----
 *
 * application.cpp
 *
 * Created on: 2023. 12. 15.
 *     Author: Valaczka János Pál <valaczka.janos@piarista.hu>
 *
 * Application
 *
 *  This file is part of Call of Suli.
 *
 *  Call of Suli is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "application.h"
#include "Logger.h"
#include "querybuilder.hpp"
#include "utils_.h"


/**
 * @brief Application::Application
 * @param app
 */

Application::Application(QGuiApplication *app)
    : AbstractApplication(app)
{

}


/**
 * @brief Application::~Application
 */

Application::~Application()
{
testSql(true);
}



/**
 * @brief Application::testSql
 * @param close
 */

void Application::testSql(const bool &close)
{
    static const QString &dbName = QStringLiteral("testDb");

    if (close) {
        if (QSqlDatabase::contains(dbName)) {
            QSqlDatabase::removeDatabase(dbName);
        }

        return;
    }

    if (!QSqlDatabase::contains(dbName)) {
        snack("Add database");
        auto db = QSqlDatabase::addDatabase("QSQLITE", dbName);
        db.setDatabaseName(":memory:");
        DB_LOG_DEBUG() << "Open DB" << db.open();
        if (!QueryBuilder::q(db)
                 .addQuery("CREATE TABLE test(id INTEGER PRIMARY KEY, timestamp TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP)")
                 .exec()) {
            messageError("Create error");
        }
        return;
    }

    auto db = QSqlDatabase::database(dbName);

    if (!QueryBuilder::q(db)
             .addQuery("INSERT INTO test(timestamp) VALUES(CURRENT_TIMESTAMP)")
             .exec()) {
        messageError("Insert error");
        return;
    }

    snack("added");

    auto list = QueryBuilder::q(db).addQuery("SELECT * from test").execToJsonArray();

    if (!list) {
        messageError("Select error");
    } else {
        QJsonDocument doc(*list);
        emit addedToDb(doc.toJson());
    }
}



/**
 * @brief Application::onApplicationStarted
 */

void Application::onApplicationStarted()
{
    LOG_CINFO("app") << "Az alkalmazás sikeresen elindult.";

        stackPushPage(QStringLiteral("PageStart.qml"));
}




bool Application::loadResources()
{
    QStringList searchList;

    loadFonts({
        QStringLiteral(":/NotoSans-Italic-VariableFont_wdth,wght.ttf"),
        QStringLiteral(":/NotoSans-VariableFont_wdth,wght.ttf"),
    });

    return true;
}



/**
 * @brief Application::registerQmlTypes
 */

void Application::registerQmlTypes()
{
    LOG_CTRACE("app") << "Register QML types";

    qmlRegisterUncreatableType<Application>("TimeCalculator", 1, 0, "Application", "Application is uncreatable");
    qmlRegisterUncreatableType<Utils>("TimeCalculator", 1, 0, "Utils", "Utils is uncreatable");

    //qmlRegisterType<QSJsonListModel>("QSyncable", 1, 0, "QSJsonListModel");
    //qmlRegisterType<QSListModel>("QSyncable", 1, 0, "QSListModel");


    //qmlRegisterType<BaseMap>("CallOfSuli", 1, 0, "BaseMap");
}


/**
 * @brief Application::setAppContextProperty
 */

void Application::setAppContextProperty()
{
    m_engine->rootContext()->setContextProperty("App", this);
}
