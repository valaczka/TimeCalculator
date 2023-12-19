/*
 * ---- Call of Suli ----
 *
 * onlineapplication.cpp
 *
 * Created on: 2023. 12. 11.
 *     Author: Valaczka János Pál <valaczka.janos@piarista.hu>
 *
 * OnlineApplication
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

#include "onlineapplication.h"
#include "Logger.h"
#include "utils_.h"
#include "emscripten_browser_file.h"


OnlineApplication::OnlineApplication(QGuiApplication *app)
	: Application(app)
{
	connect(this, &Application::databaseChanged, this, [this]() {
		if (m_database)
			enableTabCloseConfirmation(true);
		else
			enableTabCloseConfirmation(false);
	});
}


/**
 * @brief OnlineApplication::dbOpen
 * @param accept
 */

void OnlineApplication::dbOpen(const QString &accept)
{
	if (m_database)
		return messageError(tr("Már meg van nyitva egy adatbázis!"));

	emscripten_browser_file::upload(accept.toStdString(), [](std::string const &/*filename*/,
									std::string const &/*mime_type*/,
									std::string_view buffer, void *ptr){
		if (!ptr) {
			LOG_CERROR("app") << "Invalid argument";
			return;
		}

		OnlineApplication *app = reinterpret_cast<OnlineApplication*>(ptr);

		if (!app) {
			LOG_CERROR("app") << "Invalid argument";
			return;
		}

		QByteArray content(buffer.data(), buffer.length());
		const auto &json = Utils::byteArrayToJsonObject(content);

		if (!json) {
			app->messageError(tr("Érvénytelen fájl"));
		} else {
			app->loadFromJson(*json);
		}

	}, this);

}



/**
 * @brief OnlineApplication::dbSave
 */

void OnlineApplication::dbSave()
{
	if (!m_database)
		return messageError(tr("Nincs megnyitva adatbázis!"));

	const auto &json = m_database->toJson();

	if (json) {
		QJsonDocument doc(*json);
		const QByteArray &content = doc.toJson();

		wasmSave(content, m_database->title().append(QStringLiteral(".json")), QStringLiteral("application/json"));

		m_database->setModified(false);
	} else
		messageError(tr("Sikertelen mentés"));
}


/**
 * @brief OnlineApplication::dbPrint
 */

void OnlineApplication::dbPrint()
{
	if (!m_database)
		return messageError(tr("Nincs megnyitva adatbázis!"));

	const QByteArray &content = toTextDocument();

	wasmSave(content, m_database->title().append(QStringLiteral(".pdf")), QStringLiteral("application/pdf"));
}


/**
 * @brief OnlineApplication::importTemplateDownload
 */

void OnlineApplication::importTemplateDownload() const
{
	const QByteArray &content = importTemplate();
	wasmSave(content, QStringLiteral("import_sablon.xlsx"),
			 QStringLiteral("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"));
}


/**
 * @brief OnlineApplication::import
 */

void OnlineApplication::import()
{
	if (!m_database)
		return messageError(tr("Nincs megnyitva adatbázis!"));

	emscripten_browser_file::upload(std::string{".xlsx"}, [](std::string const &/*filename*/,
									std::string const &/*mime_type*/,
									std::string_view buffer, void *ptr){
		if (!ptr) {
			LOG_CERROR("app") << "Invalid argument";
			return;
		}

		OnlineApplication *app = reinterpret_cast<OnlineApplication*>(ptr);

		if (!app) {
			LOG_CERROR("app") << "Invalid argument";
			return;
		}

		QByteArray content(buffer.data(), buffer.length());

		if (!app->importData(content)) {
			app->messageError(tr("Hibás fájl"));
		} else {
			app->messageInfo(tr("Az importálás sikerült."));
		}
	}, this);


	/*wasmOpen(QStringLiteral("*.xlsx"), [this](const QByteArray &content, const QString &){
		if (!importData(content)) {
			messageError(tr("Hibás fájl"));
		} else {
			messageInfo(tr("Az importálás sikerült."));
		}
	});*/
}


