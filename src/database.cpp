/*
 * ---- Call of Suli ----
 *
 * database.cpp
 *
 * Created on: 2023. 12. 16.
 *     Author: Valaczka János Pál <valaczka.janos@piarista.hu>
 *
 * Database
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

#include <QSqlDatabase>
#include <Logger.h>
#include <querybuilder.hpp>
#include "database.h"
#include "application.h"
#include "utils_.h"



Database::Database(QObject *parent)
    : QObject{parent}
    , m_model(new QSListModel)
{
    m_model->setRoleNames(QStringList{
        QStringLiteral("id"),
        QStringLiteral("start"),
        QStringLiteral("end"),
        QStringLiteral("name"),
        QStringLiteral("master"),
        QStringLiteral("type"),
        QStringLiteral("hour"),
        QStringLiteral("value"),
        QStringLiteral("overlap"),
        QStringLiteral("durationYears"),
        QStringLiteral("durationDays"),
        QStringLiteral("jobMode"),
        QStringLiteral("jobYears"),
        QStringLiteral("jobDays"),
        QStringLiteral("practiceMode"),
        QStringLiteral("practiceYears"),
        QStringLiteral("practiceDays"),
        QStringLiteral("prestigeMode"),
        QStringLiteral("prestigeYears"),
        QStringLiteral("prestigeDays"),
    });
}


/**
 * @brief Database::~Database
 */

Database::~Database()
{
    LOG_CTRACE("app") << "Database closed" << qPrintable(m_databaseName);

    if (QSqlDatabase::contains(m_databaseName))
        QSqlDatabase::removeDatabase(m_databaseName);
}



/**
 * @brief Database::prepare
 * @param databaseName
 * @return
 */

bool Database::prepare(const QString &databaseName)
{
    if (QSqlDatabase::contains(databaseName)) {
        LOG_CWARNING("app") << "Database already exists:" << qPrintable(databaseName);
        return false;
    }

    LOG_CDEBUG("app") << "Prepare database:" << qPrintable(databaseName);

    auto db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), databaseName);
    db.setDatabaseName(QStringLiteral(":memory:"));

    if (!db.open()) {
        LOG_CERROR("app") << "Can't open database:" << qPrintable(databaseName);
        db.close();
        QSqlDatabase::removeDatabase(databaseName);
        return false;
    }


    static const char* const sqlList[] =
        {
            "CREATE TABLE job("
            "id INTEGER NOT NULL PRIMARY KEY, "
            "start TEXT NOT NULL, "
            "end TEXT, "
            "name TEXT, "
            "master TEXT, "
            "type TEXT, "
            "hour INTEGER, "
            "value INTEGER"
            ")",

            "CREATE TABLE calc("
            "id INTEGER NOT NULL PRIMARY KEY, "
            "jobid INTEGER NOT NULL REFERENCES job(id) ON UPDATE CASCADE ON DELETE CASCADE, "
            "type INTEGER NOT NULL, "
            "mode INTEGER NOT NULL DEFAULT 1, "
            "years INTEGER, "
            "days INTEGER, "
            "UNIQUE(jobid, type)"
            ")",

            "CREATE VIEW overlap AS "
            "WITH r AS (SELECT id, start, CASE WHEN start > date('now') THEN start ELSE COALESCE(end, date('now')) END AS end FROM job) "
            "SELECT r1.id AS jobid1, r2.id as jobid2 FROM r AS r1 INNER JOIN r AS r2 "
            "ON (r1.id <> r2.id AND r1.start <= r2.end AND r2.start <= r1.end)"
        };


    db.transaction();

    for (const auto &sql : sqlList) {
        if (!QueryBuilder::q(db).addQuery(sql).exec()) {
            LOG_CERROR("app") << "SQL error:" << qPrintable(databaseName) << sql;
            db.close();
            QSqlDatabase::removeDatabase(databaseName);
            return false;
        }
    }

    db.commit();

    LOG_CDEBUG("app") << "Database prepared";

    return true;
}



/**
 * @brief Database::toJson
 * @return
 */

std::optional<QJsonObject> Database::toJson() const
{
    if (!QSqlDatabase::contains(m_databaseName)) {
        LOG_CWARNING("app") << "Database doesn't exists:" << qPrintable(m_databaseName);
        return std::nullopt;
    }

    auto db = QSqlDatabase::database(m_databaseName);

    if (!db.isOpen()) {
        LOG_CWARNING("app") << "Database doesn't opened:" << qPrintable(m_databaseName);
        return std::nullopt;
    }

    const QMap<QString, FieldConvertFunc> converter = {
        { QStringLiteral("start"), [](const QVariant &v) -> QJsonValue {
                return v.toDate().toString(QStringLiteral("yyyy-MM-dd"));
            }
        },
        { QStringLiteral("end"), [](const QVariant &v) -> QJsonValue {
                if (v.isNull())
                    return QJsonValue::Null;
                else
                    return v.toDate().toString(QStringLiteral("yyyy-MM-dd"));
            }
        },
        { QStringLiteral("name"), [](const QVariant &v) -> QJsonValue {
                if (v.isNull())
                    return QJsonValue::Null;
                else
                    return v.toString();
            }
        },
        { QStringLiteral("master"), [](const QVariant &v) -> QJsonValue {
                if (v.isNull())
                    return QJsonValue::Null;
                else
                    return v.toString();
            }
        },
        { QStringLiteral("type"), [](const QVariant &v) -> QJsonValue {
                if (v.isNull())
                    return QJsonValue::Null;
                else
                    return v.toString();
            }
        },
        { QStringLiteral("hour"), [](const QVariant &v) -> QJsonValue {
                if (v.isNull())
                    return QJsonValue::Null;
                else
                    return v.toInt();
            }
        },
        { QStringLiteral("value"), [](const QVariant &v) -> QJsonValue {
                if (v.isNull())
                    return QJsonValue::Null;
                else
                    return v.toInt();
            }
        },
    };

    const auto &jobList = QueryBuilder::q(db)
                              .addQuery("SELECT * FROM job")
                              .execToJsonArray(converter);

    const auto &calcList = QueryBuilder::q(db)
                               .addQuery("SELECT * FROM calc")
                               .execToJsonArray();

    if (!jobList || !calcList) {
        LOG_CWARNING("app") << "Sql error:" << qPrintable(m_databaseName);
        return std::nullopt;
    }

    return QJsonObject{
                       { QStringLiteral("_type"), QStringLiteral("TimeCalculator") },
                       { QStringLiteral("_version"), 0 },
                       { QStringLiteral("title"), m_title },
                       { QStringLiteral("jobs"), *jobList },
                       { QStringLiteral("calculations"), *calcList },
                       };
}






/**
 * @brief Database::fromJson
 * @param json
 * @return
 */

Database *Database::fromJson(const QString &databaseName, const QJsonObject &json)
{
    if (json.value(QStringLiteral("_type")).toString() != QStringLiteral("TimeCalculator")) {
        LOG_CWARNING("app") << "Invalid JSON";
        return nullptr;
    }

    const int &version = json.value(QStringLiteral("_version")).toInt();

    if (version > 0) {
        LOG_CWARNING("app") << "Invalid JSON version";
        return nullptr;
    }

    std::unique_ptr<Database> ptr(new Database);

    if (!ptr->prepare(databaseName.isEmpty() ? ptr->databaseName() : databaseName))
        return nullptr;

    if (!databaseName.isEmpty())
        ptr->setDatabaseName(databaseName);

    const auto &list = json.value(QStringLiteral("jobs")).toArray();
    for (auto v : list) {
        const QJsonObject &obj = v.toObject();

        if (ptr->jobAdd(obj) == -1) {
            LOG_CWARNING("app") << "SQL error" << obj;
            return nullptr;
        }
    }

    const auto &cList = json.value(QStringLiteral("calculations")).toArray();
    for (auto v : cList) {
        const QJsonObject &obj = v.toObject();

        if (!ptr->calculationAddFromJson(obj)) {
            LOG_CWARNING("app") << "SQL error" << obj;
            return nullptr;
        }

    }

    ptr->setTitle(json.value(QStringLiteral("title")).toString());
    ptr->setModified(false);

    return ptr.release();
}


/**
 * @brief Database::jobAdd
 * @param data
 */

int Database::jobAdd(const QJsonObject &data)
{
    auto db = QSqlDatabase::database(m_databaseName);
    if (!db.isOpen()) {
        LOG_CERROR("app") << "Database isn't opened";
        return -1;
    }

    QueryBuilder q(db);

    q.addQuery("INSERT INTO job(")
        .setFieldPlaceholder()
        .addQuery(") VALUES (")
        .setValuePlaceholder()
        .addQuery(")");

    for (const QString &s : data.keys()) {
        if (s == QStringLiteral("start") || s == QStringLiteral("end")) {
            QDate d = QDate::fromString(data.value(s).toString(), QStringLiteral("yyyy-MM-dd"));
            if (d.isNull()) {
                LOG_CWARNING("app") << "Invalid date:" << data.value(s).toString();
            } else {
                q.addField(s.toUtf8(), d);
            }
        } else {
            q.addField(s.toUtf8(), data.value(s).toVariant());
        }
    }

    const auto &id = q.execInsertAsInt();

    setModified(true);

    sync();

    if (!id)
        return -1;
    else
        return *id;

}


/**
 * @brief Database::jobEdit
 * @param id
 * @param data
 * @return
 */

bool Database::jobEdit(const int &id, const QJsonObject &data)
{
    auto db = QSqlDatabase::database(m_databaseName);
    if (!db.isOpen()) {
        LOG_CERROR("app") << "Database isn't opened";
        return -1;
    }

    QueryBuilder q(db);

    q.addQuery("UPDATE job SET ")
        .setCombinedPlaceholder();

    for (const QString &s : data.keys()) {
        if (s != QStringLiteral("id"))
            q.addField(s.toUtf8(), data.value(s).toVariant());
    }

    if (!q.fieldCount()) {
        LOG_CERROR("app") << "Missing fields";
        return false;
    }

    q.addQuery(" WHERE id=").addValue(id);

    if (!q.exec()) {
        LOG_CERROR("app") << "SQL error";
        return false;
    }

    setModified(true);

    sync();

    return true;
}



/**
 * @brief Database::calculationGet
 * @param id
 * @returnjob
 */

QVariantMap Database::calculationGet(const int &id) const
{
    auto db = QSqlDatabase::database(m_databaseName);
    if (!db.isOpen()) {
        LOG_CERROR("app") << "Database isn't opened";
        return {};
    }

    QVariantMap map;

    map.insert(QStringLiteral("jobMode"), 0);
    map.insert(QStringLiteral("jobYears"), 0);
    map.insert(QStringLiteral("jobDays"), 0);
    map.insert(QStringLiteral("practiceMode"), 0);
    map.insert(QStringLiteral("practiceYears"), 0);
    map.insert(QStringLiteral("practiceDays"), 0);
    map.insert(QStringLiteral("prestigeMode"), 0);
    map.insert(QStringLiteral("prestigeYears"), 0);
    map.insert(QStringLiteral("prestigeDays"), 0);


    QueryBuilder q(db);
    q.addQuery("SELECT type, mode, years, days FROM calc WHERE jobid=").addValue(id);

    if (q.exec()) {
        while (q.sqlQuery().next()) {
            const int &type = q.value("type").toInt(0);
            const int &mode = q.value("mode").toInt(0);
            const int &years = q.value("years").toInt(0);
            const int &days = q.value("days").toInt(0);

            QString prefix;

            if (type == 1) {
                prefix = QStringLiteral("job");
            } else if (type == 2) {
                prefix = QStringLiteral("practice");
            } else if (type == 3) {
                prefix = QStringLiteral("prestige");
            }

            map[prefix+QStringLiteral("Mode")] = mode;
            map[prefix+QStringLiteral("Years")] = years;
            map[prefix+QStringLiteral("Days")] = days;
        }
    }

    return map;
}



/**
 * @brief Database::calculationEdit
 * @param id
 * @param data
 * @return
 */

bool Database::calculationEdit(const int &id, const QJsonObject &data)
{
    auto db = QSqlDatabase::database(m_databaseName);
    if (!db.isOpen()) {
        LOG_CERROR("app") << "Database isn't opened";
        return -1;
    }

    const QStringList &keys = data.keys();

    bool ret = false;

    for (int type=1; type<=3; ++type) {
        if (type == 1 && !keys.contains(QStringLiteral("jobMode")))
            continue;

        if (type == 2 && !keys.contains(QStringLiteral("practiceMode")))
            continue;

        if (type == 3 && !keys.contains(QStringLiteral("prestigeMode")))
            continue;

        QueryBuilder q(db);
        q.addQuery("INSERT OR REPLACE INTO calc(")
            .setFieldPlaceholder()
            .addQuery(") VALUES(")
            .setValuePlaceholder()
            .addQuery(")");

        q.addField("jobid", id);
        q.addField("type", type);

        QString prefix;
        static const QHash<QByteArray, QString> hash = {
                                                        { QByteArrayLiteral("mode"), QStringLiteral("Mode") },
                                                        { QByteArrayLiteral("years"), QStringLiteral("Years") },
                                                        { QByteArrayLiteral("days"), QStringLiteral("Days") },
                                                        };

        if (type == 1) {
            prefix = QStringLiteral("job");
        } else if (type == 2) {
            prefix = QStringLiteral("practice");
        } else if (type == 3) {
            prefix = QStringLiteral("prestige");
        }

        for (auto it = hash.constBegin(); it != hash.constEnd(); ++it) {
            const QString &key = prefix+it.value();
            if (data.contains(key)) {
                q.addField(it.key(), data.value(key).toVariant());
            }
        }

        if (!q.exec())
            return false;
        else
            ret = true;
    }

    setModified(true);

    sync();

    return ret;
}



/**
 * @brief Database::databaseName
 * @return
 */

QString Database::databaseName() const
{
    return m_databaseName;
}

void Database::setDatabaseName(const QString &newDatabaseName)
{
    if (m_databaseName == newDatabaseName)
        return;
    m_databaseName = newDatabaseName;
    emit databaseNameChanged();
}


/**
 * @brief Database::title
 * @return
 */

QString Database::title() const
{
    return m_title;
}

void Database::setTitle(const QString &newTitle)
{
    if (m_title == newTitle)
        return;
    m_title = newTitle;
    emit titleChanged();
    setModified(true);
}


QSListModel*Database::model() const
{
    return m_model.get();
}


/**
 * @brief Database::sqlMainView
 * @return
 */

QVariantList Database::sqlMainView(QVariantMap *dest) const
{
    if (!QSqlDatabase::contains(m_databaseName)) {
        LOG_CWARNING("app") << "Database doesn't exists:" << qPrintable(m_databaseName);
        return {};
    }

    auto db = QSqlDatabase::database(m_databaseName);

    if (!db.isOpen()) {
        LOG_CWARNING("app") << "Database doesn't opened:" << qPrintable(m_databaseName);
        return {};
    }

    const QMap<QString, FieldConvertVariantFunc> converter = {
                                                              { QStringLiteral("id"), [](const QVariant &v) -> QVariant { return v.toLongLong(); } },
                                                              { QStringLiteral("start"), [](const QVariant &v) -> QVariant { return v.toDate(); } },
                                                              { QStringLiteral("end"), [](const QVariant &v) -> QVariant {
                                                                   if (v.isNull())
                                                                       return QVariant(QMetaType::fromType<QDate>());
                                                                   else
                                                                       return v.toDate();
                                                               } },
                                                              { QStringLiteral("name"), [](const QVariant &v) -> QVariant { return v.toString(); } },
                                                              { QStringLiteral("hour"), [](const QVariant &v) -> QVariant { return v.toInt(); } },
                                                              { QStringLiteral("value"), [](const QVariant &v) -> QVariant { return v.toInt(); } },
                                                              { QStringLiteral("overlap"), [](const QVariant &v) -> QVariant { return v.toBool(); } },
                                                              };

    const auto &jobList = QueryBuilder::q(db)
                              .addQuery("SELECT id, start, end, name, master, type, hour, value, "
                                        "EXISTS(SELECT * FROM overlap WHERE jobid1=job.id) AS overlap "
                                        "FROM job "
                                        "ORDER BY id")
                              .execToVariantList(converter);

    if (!jobList) {
        LOG_CWARNING("app") << "Sql error:" << qPrintable(m_databaseName);
        return {};
    }

    QVariantList list;

    struct Calc {
        int jobYears = 0;
        int jobDays = 0;
        int practiceYears = 0;
        int practiceDays = 0;
        int prestigeYears = 0;
        int prestigeDays = 0;

        QVariantMap toMap() const {
            QVariantMap m;
            m[QStringLiteral("jobYears")] = jobYears;
            m[QStringLiteral("jobDays")] = jobDays;
            m[QStringLiteral("practiceYears")] = practiceYears;
            m[QStringLiteral("practiceDays")] = practiceDays;
            m[QStringLiteral("prestigeYears")] = prestigeYears;
            m[QStringLiteral("prestigeDays")] = prestigeDays;
            return m;
        }

        void normalize() {
            int jy = qFloor((float)jobDays/365.);
            jobYears += jy;
            jobDays -= 365*jy;

            int pay = qFloor((float)practiceDays/365.);
            practiceYears += pay;
            practiceDays -= 365*pay;

            int pey = qFloor((float)prestigeDays/365.);
            prestigeYears += pey;
            prestigeDays -= 365*pey;
        }
    };

    Calc calc;

    for (const auto &v : *jobList) {
        QVariantMap map = v.toMap();

        const int &id = map.value(QStringLiteral("id"), 0).toInt();
        const QDate &date1 = map.value(QStringLiteral("start")).toDate();
        QDate date2 = map.value(QStringLiteral("end")).toDate();

        if (date2.isNull()) {
            map.remove(QStringLiteral("end"));
            date2 = QDate::currentDate();
        }

        const int &defYears = Application::yearsBetween(date1, date2);
        const int &defDays = Application::daysBetween(date1, date2);

        map.insert(QStringLiteral("durationYears"), defYears);
        map.insert(QStringLiteral("durationDays"), defDays);


        map.insert(QStringLiteral("jobMode"), -1);
        map.insert(QStringLiteral("jobYears"), 0);
        map.insert(QStringLiteral("jobDays"), 0);
        map.insert(QStringLiteral("practiceMode"), -1);
        map.insert(QStringLiteral("practiceYears"), 0);
        map.insert(QStringLiteral("practiceDays"), 0);
        map.insert(QStringLiteral("prestigeMode"), -1);
        map.insert(QStringLiteral("prestigeYears"), 0);
        map.insert(QStringLiteral("prestigeDays"), 0);


        QueryBuilder q(db);
        q.addQuery("SELECT type, mode, years, days FROM calc WHERE jobid=").addValue(id);


        if (q.exec()) {
            while (q.sqlQuery().next()) {
                const int &type = q.value("type").toInt(0);
                const int &mode = q.value("mode").toInt(0);
                int years = q.value("years").toInt(0);
                int days = q.value("days").toInt(0);

                if (mode == 0) {
                    years = 0;
                    days = 0;
                } else if (mode == 1) {
                    years = defYears;
                    days = defDays;
                }

                QString prefix;

                if (type == 1) {
                    prefix = QStringLiteral("job");
                    calc.jobDays += days;
                    calc.jobYears += years;
                } else if (type == 2) {
                    prefix = QStringLiteral("practice");
                    calc.practiceDays += days;
                    calc.practiceYears += years;
                } else if (type == 3) {
                    prefix = QStringLiteral("prestige");
                    calc.prestigeDays += days;
                    calc.prestigeYears += years;
                }

                map[prefix+QStringLiteral("Mode")] = mode;
                map[prefix+QStringLiteral("Years")] = years;
                map[prefix+QStringLiteral("Days")] = days;
            }
        }

        list.append(map);
    }

    calc.normalize();

    if (dest)
        *dest = calc.toMap();

    return list;
}

bool Database::modified() const
{
    return m_modified;
}

void Database::setModified(bool newModified)
{
    if (m_modified == newModified)
        return;
    m_modified = newModified;
    emit modifiedChanged();
}



/**
 * @brief Database::sync
 */

void Database::sync()
{
    QVariantMap map;
    Utils::patchSListModel(m_model.get(), sqlMainView(&map), QStringLiteral("id"));
    setCalculation(map);
}



/**
 * @brief Database::calculation
 * @return
 */

QVariantMap Database::calculation() const
{
    return m_calculation;
}

void Database::setCalculation(const QVariantMap &newCalculation)
{
    if (m_calculation == newCalculation)
        return;
    m_calculation = newCalculation;
    emit calculationChanged();
}



/**
 * @brief Database::calculationAddFromJson
 * @param data
 */

bool Database::calculationAddFromJson(const QJsonObject &data)
{
    auto db = QSqlDatabase::database(m_databaseName);
    if (!db.isOpen()) {
        LOG_CERROR("app") << "Database isn't opened";
        return false;
    }

    QueryBuilder q(db);
    q.addQuery("INSERT OR REPLACE INTO calc(")
        .setFieldPlaceholder()
        .addQuery(") VALUES(")
        .setValuePlaceholder()
        .addQuery(")");

    for (const QString &s : data.keys()) {
        q.addField(s.toUtf8(), data.value(s).toVariant());
    }

    const auto &id = q.execInsert();

    if (!id)
        return false;
    else {
        sync();
        return true;
    }
}
