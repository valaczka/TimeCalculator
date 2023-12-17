/*
 * ---- Call of Suli ----
 *
 * desktopapplication.cpp
 *
 * Created on: 2023. 12. 11.
 *     Author: Valaczka János Pál <valaczka.janos@piarista.hu>
 *
 * DesktopApplication
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

#include "desktopapplication.h"
#include "ColorConsoleAppender.h"
#include "Logger.h"

/**
 * @brief DesktopApplication::DesktopApplication
 * @param app
 */
DesktopApplication::DesktopApplication(QGuiApplication *app)
    : Application(app)
{
    auto appender = new ColorConsoleAppender;

    appender->setDetailsLevel(Logger::Trace);

    cuteLogger->registerAppender(appender);

    cuteLogger->logToGlobalInstance(QStringLiteral("app"), true);
    cuteLogger->logToGlobalInstance(QStringLiteral("utils"), true);
    cuteLogger->logToGlobalInstance(QStringLiteral("qml"), true);
    cuteLogger->logToGlobalInstance(QStringLiteral("db"), true);
    cuteLogger->logToGlobalInstance(QStringLiteral("logger"), true);
    cuteLogger->logToGlobalInstance(QStringLiteral("qaterial.utils"), true);
}
