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
#include <QtGui/private/qwasmlocalfileaccess_p.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>



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

    struct LoadFileData {
        QString name;
        QByteArray buffer;
    };

    LoadFileData *fileData = new LoadFileData();

    QWasmLocalFileAccess::openFile(accept.toStdString(),
        [](bool fileSelected) {
            LOG_CDEBUG("client") << "File selected" << fileSelected;
        },
        [fileData](uint64_t size, const std::string name) -> char* {
            fileData->name = QString::fromStdString(name);
            fileData->buffer.resize(size);
            return fileData->buffer.data();
        },
        [fileData, this]() {
            QByteArray content = fileData->buffer;
            //QString name = fileData->name;
            const auto &json = Utils::byteArrayToJsonObject(content);

            if (!json) {
                messageError(tr("Érvénytelen fájl"));
            } else {
                loadFromJson(*json);
            }

            delete fileData;
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

        QWasmLocalFileAccess::saveFile(content.constData(), size_t(content.size()),
                                       m_database->title().append(QStringLiteral(".json")).toStdString());

        m_database->setModified(false);
    } else
        messageError(tr("Sikertelen mentés"));
}




/**
 * @brief OnlineApplication::enableTabCloseConfirmation
 * @param enable
 */

void OnlineApplication::enableTabCloseConfirmation(bool enable)
{
    LOG_CDEBUG("app") << "Enable tab close confirmation" << enable;

    if (enable == m_closeConfirm)
        return;

    using emscripten::val;
    const val window = val::global("window");
    const bool capture = true;
    const val eventHandler = val::module_property("app_beforeUnloadHandler");
    if (enable) {
        window.call<void>("addEventListener", std::string("beforeunload"), eventHandler, capture);
    } else {
        window.call<void>("removeEventListener", std::string("beforeunload"), eventHandler, capture);
    }

    m_closeConfirm = enable;
}


namespace {
void beforeUnloadhandler(emscripten::val event) {
    LOG_CWARNING("client") << "Unload handler";
    // Adding this event handler is sufficent to make the browsers display
    // the confirmation dialog, provided the calls below are also made:
    event.call<void>("preventDefault"); // call preventDefault as required by standard
    event.set("returnValue", std::string(" ")); // set returnValue to something, as required by Chrome
    //return emscripten::val("Sure");
}
}

EMSCRIPTEN_BINDINGS(app) {
    function("app_beforeUnloadHandler", &beforeUnloadhandler);
}
