/*
 * ---- Call of Suli ----
 *
 * onlineapplication.h
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

#ifndef ONLINEAPPLICATION_H
#define ONLINEAPPLICATION_H

#include "application.h"

class OnlineApplication : public Application
{
public:
	OnlineApplication(QGuiApplication *app);
	virtual ~OnlineApplication() {}

	Q_INVOKABLE virtual void dbOpen(const QString &accept = QStringLiteral(".json")) override;
	Q_INVOKABLE virtual void dbSave() override;
	Q_INVOKABLE virtual void dbPrint() override;

	Q_INVOKABLE virtual void importTemplateDownload() const override;
	Q_INVOKABLE virtual void import() override;
};

#endif // ONLINEAPPLICATION_H
