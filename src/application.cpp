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

}



/**
 * @brief Application::dbOpen
 */

void Application::dbOpen(const QString &)
{
    if (m_database)
        return messageError(tr("Már meg van nyitva egy adatbázis!"));

    if (QFile::exists("/tmp/_test.json")) {
        const auto &json = Utils::fileToJsonObject("/tmp/_test.json");
        if (!json) {
            messageError(tr("Érvénytelen fájl"));
        } else {
            loadFromJson(*json);
        }
    }
}



/**
 * @brief Application::dbSave
 */

void Application::dbSave()
{
    if (!m_database)
        return messageError(tr("Nincs megnyitva adatbázis!"));

    const auto &json = m_database->toJson();

    if (json) {
        if (Utils::jsonObjectToFile(*json, "/tmp/_test.json")) {
            snack(tr("Mentés sikerült"));
            m_database->setModified(false);
        } else
            messageError(tr("Sikertelen mentés"));
    }
}



/**
 * @brief Application::dbCreate
 * @param title
 */

bool Application::dbCreate(const QString &title)
{
    if (m_database) {
        messageError(tr("Már meg van nyitva egy adatbázis!"));
        return false;
    }

    std::unique_ptr<Database> ptr(new Database);

    if (ptr->prepare(ptr->databaseName())) {
        ptr->setTitle(title);
        ptr->sync();
        ptr->setModified(true);
        setDatabase(ptr);
        stackPushPage(QStringLiteral("PageDatabase.qml"));
        return true;
    }

    return false;
}



/**
 * @brief Application::dbClose
 */

void Application::dbClose()
{
    if (m_database) {
        std::unique_ptr<Database> db;
        setDatabase(db);
    }
}



/**
 * @brief Application::yearsBetween
 * @param date1
 * @param date2
 * @return
 */

int Application::yearsBetween(const QDate &date1, const QDate &date2)
{
    QDate d2 = date2.addDays(1);
    QDate d(d2.year(), date1.month(), date1.day());

    if (d2 < d)
        return d2.year()-1-date1.year();
    else
        return d2.year()-date1.year();
}


/**
 * @brief Application::daysBetween
 * @param date1
 * @param date2
 * @return
 */

int Application::daysBetween(const QDate &date1, const QDate &date2)
{
    QDate d2 = date2.addDays(1);
    QDate d(d2.year(), date1.month(), date1.day());

    if (d2 < d) {
        QDate from(d2.year()-1, date1.month(), date1.day());
        return from.daysTo(d2);
    } else {
        return d.daysTo(d2);
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



/**
 * @brief Application::loadFromJson
 * @param data
 * @return
 */

bool Application::loadFromJson(const QJsonObject &data)
{
    std::unique_ptr<Database> db = nullptr;

    db.reset(Database::fromJson(data));
    if (!db) {
        messageError(tr("Érvénytelen adat"));
        return false;
    }

    setDatabase(db);
    stackPushPage(QStringLiteral("PageDatabase.qml"));

    return true;
}




/**
 * @brief Application::database
 * @return
 */

Database*Application::database() const
{
    return m_database.get();
}


/**
 * @brief Application::setDatabase
 * @param newDatabase
 */

void Application::setDatabase(Database *newDatabase)
{
    std::unique_ptr<Database> ptr(newDatabase);
    setDatabase(ptr);
}


/**
 * @brief Application::setDatabase
 * @param newDatabase
 */

void Application::setDatabase(std::unique_ptr<Database> &newDatabase)
{
    if (m_database == newDatabase)
        return;
    m_database = std::move(newDatabase);
    emit databaseChanged();
}
