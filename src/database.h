/*
 * ---- Call of Suli ----
 *
 * database.h
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

#ifndef DATABASE_H
#define DATABASE_H

#include "qslistmodel.h"
#include <QObject>

class Database : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString databaseName READ databaseName WRITE setDatabaseName NOTIFY databaseNameChanged FINAL)
	Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged FINAL)
	Q_PROPERTY(QSListModel* model READ model CONSTANT FINAL)
	Q_PROPERTY(QVariantMap calculation READ calculation WRITE setCalculation NOTIFY calculationChanged FINAL)
	Q_PROPERTY(int prestigeCalculationTime READ prestigeCalculationTime WRITE setPrestigeCalculationTime NOTIFY prestigeCalculationTimeChanged FINAL)
	Q_PROPERTY(bool modified READ modified WRITE setModified NOTIFY modifiedChanged FINAL)

public:
	explicit Database(QObject *parent = nullptr);
	virtual ~Database();

	static bool prepare(const QString &databaseName);
	std::optional<QJsonObject> toJson() const;
	static Database *fromJson(const QString &databaseName, const QJsonObject &json);
	static Database *fromJson(const QJsonObject &json) { return fromJson(QStringLiteral(""), json); }

	Q_INVOKABLE int jobAdd(const QJsonObject &data);
	bool jobAddBatch(const QVector<QVariantMap> &data);
	Q_INVOKABLE bool jobEdit(const int &id, const QJsonObject &data);
	Q_INVOKABLE bool jobDelete(const int &id);

	Q_INVOKABLE QVariantMap calculationGet(const int &id) const;
	Q_INVOKABLE bool calculationEdit(const int &id, const QJsonObject &data);
	Q_INVOKABLE QVariantList overlapGet(const int &id) const;

	Q_INVOKABLE void sync();


	Q_INVOKABLE QString toMarkdown() const;

	QString databaseName() const;
	void setDatabaseName(const QString &newDatabaseName);

	QString title() const;
	void setTitle(const QString &newTitle);

	QSListModel* model() const;

	QVariantMap calculation() const;
	void setCalculation(const QVariantMap &newCalculation);

	bool modified() const;
	void setModified(bool newModified);

	int prestigeCalculationTime() const;
	void setPrestigeCalculationTime(int newPrestigeCalculationTime);

signals:
	void databaseNameChanged();
	void titleChanged();
	void calculationChanged();
	void modifiedChanged();

	void prestigeCalculationTimeChanged();

private:
	bool calculationAddFromJson(const QJsonObject &data);
	QVariantList sqlMainView(QVariantMap *dest) const;

	QString m_databaseName = QStringLiteral("mainDb");
	QString m_title;
	int m_prestigeCalculationTime = -1;
	bool m_modified = false;

	std::unique_ptr<QSListModel> m_model;
	QVariantMap m_calculation;
};

#endif // DATABASE_H
