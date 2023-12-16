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

class Application : public AbstractApplication
{
    Q_OBJECT

public:
    Application(QGuiApplication *app);
    virtual ~Application();

     Q_INVOKABLE void testSql(const bool &close = false);

public slots:
    virtual void onApplicationStarted();

signals:
    void addedToDb(QString txt);

protected:
    virtual bool loadResources();
    virtual void registerQmlTypes();
    virtual void setAppContextProperty();
};

#endif // APPLICATION_H
