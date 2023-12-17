/*
 * ---- Call of Suli ----
 *
 * utils.h
 *
 * Created on: 2022. 12. 11.
 *     Author: Valaczka János Pál <valaczka.janos@piarista.hu>
 *
 * Utils
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

#ifndef UTILS__H
#define UTILS__H

#include "qmetaobject.h"
#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QColor>
#include <optional>
#include <qslistmodel.h>

/**
 * @brief The Utils class
 */

class Utils : public QObject
{
    Q_OBJECT

public:
    explicit Utils(QObject *parent = nullptr);
    virtual ~Utils();

    static std::optional<QByteArray> fileContent(const QString &filename);
    Q_INVOKABLE static QByteArray fileContentRead(const QString &filename) {
        return fileContent(filename).value_or(QByteArray{});
    }

    Q_INVOKABLE static QString fileBaseName(const QString &filename);
    Q_INVOKABLE static bool fileExists(const QUrl &file);

    Q_INVOKABLE static bool jsonDocumentToFile(const QJsonDocument &doc, const QString &filename,
                                               const QJsonDocument::JsonFormat &format = QJsonDocument::Indented);
    Q_INVOKABLE static bool jsonObjectToFile(const QJsonObject &object, const QString &filename,
                                             const QJsonDocument::JsonFormat &format = QJsonDocument::Indented);
    Q_INVOKABLE static bool jsonArrayToFile(const QJsonArray &array, const QString &filename,
                                            const QJsonDocument::JsonFormat &format = QJsonDocument::Indented);

    static std::optional<QJsonDocument> byteArrayToJsonDocument(const QByteArray &data);
    static std::optional<QJsonObject> byteArrayToJsonObject(const QByteArray &data);
    static std::optional<QJsonArray> byteArrayToJsonArray(const QByteArray &data);

    static std::optional<QJsonDocument> fileToJsonDocument(const QString &filename);
    static std::optional<QJsonObject> fileToJsonObject(const QString &filename);
    static std::optional<QJsonArray> fileToJsonArray(const QString &filename);

    Q_INVOKABLE static QColor colorSetAlpha(QColor color, const qreal &alpha);

    Q_INVOKABLE static QString formatMSecs(const int &msec, const int &decimals = 0, const bool &withMinute = true);

    Q_INVOKABLE static void openUrl(const QUrl &url);

    Q_INVOKABLE static QString standardPath(const QString &path = "");
    Q_INVOKABLE static QString genericDataPath(const QString &path = "");

    Q_INVOKABLE static QByteArray generateRandomString(quint8 length);
    Q_INVOKABLE static QByteArray generateRandomString(quint8 length, const char *characters);

    Q_INVOKABLE static QVariant settingsGet(const QString &key, const QVariant &defaultValue = QVariant());
    Q_INVOKABLE static void settingsSet(const QString &key, const QVariant &value);
    Q_INVOKABLE static void settingsClear(const QString &key);

    static const QModelIndex& noParent();

    template<typename QEnum>
    static std::string enumToString(const QEnum value)
    {
        return std::string(QMetaEnum::fromType<QEnum>().valueToKey(value));
    }

    template<typename QEnum>
    static QString enumToQString(const QEnum value)
    {
        return QString(QMetaEnum::fromType<QEnum>().valueToKey(value));
    }

    static QStringList getRolesFromObject(const QMetaObject *object);
    static void patchSListModel(QSListModel *model, const QVariantList &data, const QString &keyField);

    Q_INVOKABLE static void setClipboardText(const QString &text);
    Q_INVOKABLE static QString clipboardText();

#ifdef CLIENT_UTILS
    Q_INVOKABLE void checkStoragePermissions();
    Q_INVOKABLE void checkMediaPermissions();
#endif

    Q_INVOKABLE static quint32 versionCode(const int &major, const int &minor);

signals:
    void storagePermissionsGranted();
    void storagePermissionsDenied();
    void mediaPermissionsGranted();
    void mediaPermissionsDenied();
};


#endif // UTILS__H
