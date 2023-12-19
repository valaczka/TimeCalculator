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
#include "qtextdocument.h"
#include <QPdfWriter>
#include "utils_.h"
#include "xlsxdocument.h"

const QHash<Application::Field, QString> Application::m_fieldMap = {
	{ StartDate, QStringLiteral("Jogviszony kezdete") },
	{ EndDate, QStringLiteral("Jogviszony vége") },
	{ Name, QStringLiteral("Munkakör") },
	{ Master, QStringLiteral("Munkáltató") },
	{ StartDate, QStringLiteral("Jogviszony kezdete") },
	{ Type, QStringLiteral("Jogviszony típusa") },
	{ Hour, QStringLiteral("Munkaidő") },
	{ Value, QStringLiteral("Heti óraszám") },
};



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
 * @brief Application::dbPrint
 */

void Application::dbPrint()
{
	if (!m_database)
		return messageError(tr("Nincs megnyitva adatbázis!"));

	const QByteArray &content = toTextDocument();

	QFile f("/tmp/out.pdf");
	f.open(QIODevice::WriteOnly);
	f.write(content);
	f.close();

	snack(tr("PDF elkészült"));
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
 * @brief Application::importTemplateDownload
 */

void Application::importTemplateDownload() const
{
	const QByteArray &data = importTemplate();

	QFile f("/tmp/_import.xlsx");
	f.open(QIODevice::WriteOnly);
	f.write(data);
	f.close();

	snack(tr("Sablon elkészült"));
}


/**
 * @brief Application::import
 */

void Application::import()
{
	if (!m_database)
		return messageError(tr("Nincs megnyitva adatbázis!"));

	if (QFile::exists("/tmp/_import.xlsx")) {
		const auto &content = Utils::fileContent("/tmp/_import.xlsx");
		if (!content) {
			messageError(tr("Érvénytelen fájl"));
		} else {
			if (!importData(*content)) {
				messageError(tr("Hibás fájl"));
			} else {
				messageInfo(tr("Az importálás sikerült."));
			}
		}
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

	db->sync();
	setDatabase(db);
	stackPushPage(QStringLiteral("PageDatabase.qml"));

	return true;
}




/**
 * @brief Application::toTextDocument
 * @param markdown
 */

QByteArray Application::toTextDocument() const
{
	QTextDocument document;

	QFont font(QStringLiteral("Noto Sans"), 9);

	document.setDefaultFont(font);
	document.setHtml(m_database->toMarkdown());

	QImage img = QImage::fromData(Utils::fileContent(":/piar.png").value_or(QByteArray{}));
	document.addResource(QTextDocument::ImageResource, QUrl("imgdata://piar.png"), QVariant(img));


	QByteArray content;
	QBuffer buffer(&content);
	buffer.open(QIODevice::WriteOnly);

	QPdfWriter pdf(&buffer);
	pdf.setCreator("Valaczka János Pál");
	QPageLayout layout = pdf.pageLayout();
	layout.setPageSize(QPageSize::A4);
	layout.setMargins(QMarginsF(5, 5, 5, 5));
	layout.setMinimumMargins(QMarginsF(5, 5, 5, 5));
	layout.setMode(QPageLayout::FullPageMode);
	pdf.setPageLayout(layout);

	pdf.setTitle(QStringLiteral("Gyakorlati idő kalkulátor – ").append(m_database->title()));
	pdf.setCreator(QStringLiteral("TimeCalculator"));

	document.print(&pdf);

	buffer.close();

	return content;
}



/**
 * @brief Application::importTemplate
 * @return
 */

QByteArray Application::importTemplate() const
{
	QXlsx::Document doc;

	QXlsx::Format format;
	format.setBottomBorderStyle(QXlsx::Format::BorderMedium);
	format.setFontBold(true);


	static const QVector<QMap<Field, QVariant>> records = {
		{
			{ StartDate, QDate::currentDate().addYears(-1) },
			{ EndDate, QDate::currentDate() },
			{ Name, QStringLiteral("pedagógus") },
			{ Master, QStringLiteral("Petőfi Sándor Általános Iskola\nBudapest") },
			{ Type, QStringLiteral("munkaszerződés") },
			{ Hour, 40 },
		},

	};

	int cell = 1;

	const QMetaEnum enumId = QMetaEnum::fromType<Field>();

	// Header

	for (int i=0; i<enumId.keyCount(); ++i) {
		const Field &field = QVariant(enumId.value(i)).value<Field>();
		const QString &name = m_fieldMap.value(field);
		if (name.isEmpty())
			continue;

		int row = 1;
		doc.write(row, cell, name, format);

		// Example records

		for (auto it = records.constBegin(); it != records.constEnd(); ++it) {
			const QVariant &txt = it->value(field);
			++row;

			if (txt.isNull())
				continue;

			doc.write(row, cell, txt);
		}

		++cell;
	}

	//const double w = doc.columnWidth(1) * 6.;
	//doc.setColumnWidth(1, cell, w);


	QBuffer buf;
	doc.saveAs(&buf);
	return buf.data();
}



/**
 * @brief Application::importData
 * @param data
 * @return
 */

bool Application::importData(const QByteArray &data)
{
	if (!m_database)
		return false;

	QBuffer buf;
	buf.setData(data);
	buf.open(QIODevice::ReadOnly);

	QXlsx::Document doc(&buf);

	buf.close();

	QHash<int, Field> headers;

	const QXlsx::CellRange &range = doc.dimension();

	for (int i=range.firstColumn(); i<=range.lastColumn(); ++i) {
		const QString &txt = doc.read(range.firstRow(), i).toString();

		const Field &field = m_fieldMap.key(txt, Invalid);

		if (field != Invalid)
			headers.insert(i, field);
	}

	if (headers.isEmpty()) {
		return false;
	}

	QVector<QVariantMap> list;

	for (int row=range.firstRow()+1; row<=range.lastRow(); ++row) {
		QVariantMap map;

		for (auto it = headers.constBegin(); it != headers.constEnd(); ++it) {
			const QVariant &cell = doc.read(row, it.key());

			if (cell.isNull())
				continue;

			LOG_CINFO("app") << cell.toString() << cell.toDate() << cell.canConvert<QDate>();

			if (it.value() == StartDate || it.value() == EndDate) {
				QDate destDate;

				if (cell.canConvert<QDate>())
					destDate = cell.toDate();
				else if (const QDate &d = QDate::fromString(cell.toString(), QStringLiteral("yyyy-MM-dd")); !d.isNull()) {
					destDate = d;
				} else {
					static const QDate refDate(1899, 12, 31);
					const int cNum = cell.toInt();

					if (cNum > refDate.daysTo(QDate(1970, 1, 1)))
						destDate = refDate.addDays(cNum-1);				// Excel BUG: Excel dates after 28th February 1900 are actually one day out. Excel behaves as though the date 29th February 1900 existed, which it didn't.
				}

				if (it.value() == StartDate && !destDate.isNull())
					map[QStringLiteral("start")] = destDate;
				else if (it.value() == EndDate && !destDate.isNull())
					map[QStringLiteral("end")] = destDate;
			} else if (it.value() == Name)
				map[QStringLiteral("name")] = cell.toString();
			else if (it.value() == Master)
				map[QStringLiteral("master")] = cell.toString();
			else if (it.value() == Type)
				map[QStringLiteral("type")] = cell.toString();
			else if (it.value() == Hour)
				map[QStringLiteral("hour")] = cell.toInt();
			else if (it.value() == Value)
				map[QStringLiteral("value")] = cell.toInt();
		}

		if (map.isEmpty())
			continue;

		list.append(map);
	}

	return m_database->jobAddBatch(list);
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
