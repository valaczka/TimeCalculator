/*
 * ---- Call of Suli ----
 *
 * application.h
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

#ifndef APPLICATION_H
#define APPLICATION_H

#include "abstractapplication.h"
#include "database.h"

class Application : public AbstractApplication
{
	Q_OBJECT

	Q_PROPERTY(Database* database READ database NOTIFY databaseChanged FINAL)

public:
	Application(QGuiApplication *app);
	virtual ~Application();

	Q_INVOKABLE virtual void dbOpen(const QString &accept = QStringLiteral("*"));
	Q_INVOKABLE virtual void dbSave();
	Q_INVOKABLE virtual void dbPrint();
	Q_INVOKABLE bool dbCreate(const QString &title);
	Q_INVOKABLE void dbClose();

	Q_INVOKABLE static int yearsBetween(const QDate &date1, const QDate &date2);
	Q_INVOKABLE static int daysBetween(const QDate &date1, const QDate &date2);

	Database* database() const;
	void setDatabase(Database *newDatabase);
	void setDatabase(std::unique_ptr<Database> &newDatabase);

public slots:
	virtual void onApplicationStarted();

signals:
	void databaseChanged();

protected:
	virtual bool loadResources();
	virtual void registerQmlTypes();
	virtual void setAppContextProperty();

	bool loadFromJson(const QJsonObject &data);
	QByteArray toTextDocument() const;

	std::unique_ptr<Database> m_database;

};

#endif // APPLICATION_H
