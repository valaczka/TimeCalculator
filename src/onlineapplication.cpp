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

	wasmOpen(accept, [this](const QByteArray &content, const QString &){
		const auto &json = Utils::byteArrayToJsonObject(content);

		if (!json) {
			messageError(tr("Érvénytelen fájl"));
		} else {
			loadFromJson(*json);
		}
	});

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

		wasmSave(content, m_database->title().append(QStringLiteral(".json")));

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

	wasmSave(content, m_database->title().append(QStringLiteral(".pdf")));
}


