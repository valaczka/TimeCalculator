/*
 * ---- Call of Suli ----
 *
 * utils.cpp
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

#include "utils_.h"
#include "Logger.h"
#include "qclipboard.h"
#include "qdesktopservices.h"
#include "qdir.h"
#include "qfileinfo.h"
#include "qjsondocument.h"
#include "qmath.h"
#include "qsdiffrunner.h"
#include "qsettings.h"
#include <QStandardPaths>
#include <QGuiApplication>
#include <random>


/**
 * @brief Utils::Utils
 * @param client
 */

Utils::Utils(QObject *parent)
	: QObject{parent}
{

}


/**
 * @brief Utils::~Utils
 */

Utils::~Utils()
{

}


/**
 * @brief Utils::fileContent
 * @param filename
 * @param error
 * @return
 */

std::optional<QByteArray> Utils::fileContent(const QString &filename)
{
	QFile f(filename);

	if (!f.exists()) {
		LOG_CWARNING("utils") << "Can't read file:" << filename;
		return std::nullopt;
	}

	if (!f.open(QIODevice::ReadOnly)) {
		LOG_CWARNING("utils") << "Can't open file:" << filename;
		return std::nullopt;
	}

	QByteArray data = f.readAll();

	f.close();

	return data;
}


/**
 * @brief Utils::fileBaseName
 * @param filename
 * @return
 */

QString Utils::fileBaseName(const QString &filename)
{
	return QFileInfo(filename).baseName();
}


/**
 * @brief Utils::fileExists
 * @param filename
 * @return
 */

bool Utils::fileExists(const QUrl &file)
{
	return QFile::exists(file.toLocalFile());
}


/**
 * @brief Utils::jsonDocumentToFile
 * @param doc
 * @param filename
 * @return
 */

bool Utils::jsonDocumentToFile(const QJsonDocument &doc, const QString &filename, const QJsonDocument::JsonFormat &format)
{
	const QByteArray &b = doc.toJson(format);

	QFile f(filename);

	if (!f.open(QIODevice::WriteOnly)) {
		LOG_CWARNING("utils") << "Can't write file:" << filename;

		return false;
	}

	f.write(b);

	f.close();

	return true;
}


/**
 * @brief Utils::jsonObjectToFile
 * @param doc
 * @param filename
 * @param format
 * @return
 */

bool Utils::jsonObjectToFile(const QJsonObject &object, const QString &filename, const QJsonDocument::JsonFormat &format)
{
	return jsonDocumentToFile(QJsonDocument(object), filename, format);
}


/**
 * @brief Utils::jsonArrayToFile
 * @param array
 * @param filename
 * @param format
 * @return
 */

bool Utils::jsonArrayToFile(const QJsonArray &array, const QString &filename, const QJsonDocument::JsonFormat &format)
{
	return jsonDocumentToFile(QJsonDocument(array), filename, format);
}




/**
 * @brief Utils::byteArrayToJsonDocument
 * @param data
 * @return
 */

std::optional<QJsonDocument> Utils::byteArrayToJsonDocument(const QByteArray &data)
{
	if (data.isEmpty())
		return QJsonDocument();

	QJsonParseError error;
	QJsonDocument doc = QJsonDocument::fromJson(data, &error);

	if (error.error != QJsonParseError::NoError) {
		LOG_CWARNING("utils") << "JSON parse error:" << error.errorString() << error.error;
		return std::nullopt;
	}

	return doc;
}


/**
 * @brief Utils::byteArrayToJsonObject
 * @param data
 * @return
 */

std::optional<QJsonObject> Utils::byteArrayToJsonObject(const QByteArray &data)
{
	if (data.isEmpty())
		return QJsonObject();

	const std::optional<QJsonDocument> &doc = byteArrayToJsonDocument(data);

	if (!doc || doc->isNull())
		return std::nullopt;

	return doc->object();
}


/**
 * @brief Utils::byteArrayToJsonArray
 * @param data
 * @return
 */

std::optional<QJsonArray> Utils::byteArrayToJsonArray(const QByteArray &data)
{
	if (data.isEmpty())
		return QJsonArray();

	const std::optional<QJsonDocument> &doc = byteArrayToJsonDocument(data);

	if (!doc || doc->isNull())
		return std::nullopt;

	return doc->array();
}


/**
 * @brief Utils::fileToJsonDocument
 * @param filename
 * @return
 */

std::optional<QJsonDocument> Utils::fileToJsonDocument(const QString &filename)
{
	QFile f(filename);

	if (!f.exists()) {
		LOG_CWARNING("utils") << "Can't read file:" << filename;
		return std::nullopt;
	}

	if (!f.open(QIODevice::ReadOnly)) {
		LOG_CWARNING("utils") << "Can't open file:" << filename;
		return std::nullopt;
	}

	QByteArray data = f.readAll();

	f.close();

	return byteArrayToJsonDocument(data);
}


/**
 * @brief Utils::fileToJsonObject
 * @param filename
 * @param error
 * @return
 */

std::optional<QJsonObject> Utils::fileToJsonObject(const QString &filename)
{
	const std::optional<QJsonDocument> &doc = fileToJsonDocument(filename);

	if (!doc || doc->isNull())
		return std::nullopt;

	return doc->object();
}



/**
 * @brief Utils::fileToJsonArray
 * @param filename
 * @param error
 * @return
 */

std::optional<QJsonArray> Utils::fileToJsonArray(const QString &filename)
{
	const std::optional<QJsonDocument> &doc = fileToJsonDocument(filename);

	if (!doc || doc->isNull())
		return std::nullopt;

	return doc->array();
}


/**
 * @brief Utils::colorSetAlpha
 * @param color
 * @param alpha
 * @return
 */

QColor Utils::colorSetAlpha(QColor color, const qreal &alpha)
{
	color.setAlphaF(alpha);
	return color;
}



/**
 * @brief Utils::formatMsecs
 * @param msec
 * @param decimals
 * @return
 */

QString Utils::formatMSecs(const int &msec, const int &decimals, const bool &withMinute)
{
	int s = qFloor((qreal)msec / 1000.0);
	int ms = msec - 1000*s;

	QString r;

	if (withMinute) {
		int h = qFloor((qreal)msec / (60*60*1000.0));
		int m = qFloor((qreal)msec / (60*1000.0)) - h*60;
		s -= m*60 + h*60*60;

		if (h > 0)
			r += QStringLiteral("%1:").arg(h, 2, 10, QChar('0'));

		r += QStringLiteral("%1:%2").arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0'));

		if (decimals > 0) {
			QString n = QStringLiteral("%1").arg(ms, 3, 10, QChar('0'));
			r += QStringLiteral(".")+n.left(decimals);
		}
	} else {
		r = QString::number(s);
		if (decimals > 0) {
			QString n = QStringLiteral("%1").arg(ms, 3, 10, QChar('0'));
			r += QStringLiteral(".")+n.left(decimals);
		}
	}

	return r;
}




/**
 * @brief Utils::openUrl
 * @param url
 */
void Utils::openUrl(const QUrl &url)
{
	LOG_CDEBUG("utils") << "Open URL:" << url;
	QDesktopServices::openUrl(url);
}


/**
 * @brief Utils::standardPath
 * @param path
 * @return
 */

QString Utils::standardPath(const QString &path)
{
	if (!path.isEmpty())
		return QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).at(0)+QStringLiteral("/")+path;
	else
		return QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).at(0);
}




/**
 * @brief Utils::genericDataPath
 * @param path
 * @return
 */

QString Utils::genericDataPath(const QString &path)
{
#ifdef Q_OS_ANDROID
#if QT_VERSION >= 0x060000
	QJniObject mediaDir = QJniObject::callStaticObjectMethod("android/os/Environment", "getExternalStorageDirectory", "()Ljava/io/File;");
	QJniObject mediaPath = mediaDir.callObjectMethod( "getAbsolutePath", "()Ljava/lang/String;" );
#else
	QAndroidJniObject mediaDir = QAndroidJniObject::callStaticObjectMethod("android/os/Environment", "getExternalStorageDirectory", "()Ljava/io/File;");
	QAndroidJniObject mediaPath = mediaDir.callObjectMethod( "getAbsolutePath", "()Ljava/lang/String;" );
#endif
	if (!path.isEmpty())
		return QStringLiteral("file://")+mediaPath.toString()+QStringLiteral("/")+path;
	else
		return QStringLiteral("file://")+mediaPath.toString();
#endif

	if (!path.isEmpty())
		return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)+QStringLiteral("/")+path;
	else
		return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
}





/**
 * @brief Utils::generateRandomString
 * @param length
 * @return
 */

QByteArray Utils::generateRandomString(quint8 length)
{
	const char characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
	return generateRandomString(length, characters);
}


/**
 * @brief Utils::generateRandomString
 * @param length
 * @param characters
 * @return
 */

QByteArray Utils::generateRandomString(quint8 length, const char *characters)
{
	Q_ASSERT(characters && strlen(characters));

	static std::mt19937 randomEngine(QDateTime::currentDateTime().toMSecsSinceEpoch());
	std::uniform_int_distribution<int> distribution(0, strlen(characters)-1);
	QByteArray data;
	data.reserve(length);
	for (quint8 i = 0; i < length; ++i)
		data.append(characters[distribution(randomEngine)]);
	return data;
}


/**
 * @brief Utils::settingsGet
 * @param key
 * @param defaultValue
 * @return
 */

QVariant Utils::settingsGet(const QString &key, const QVariant &defaultValue)
{
	QSettings s;
	return s.value(key, defaultValue);
}


/**
 * @brief Utils::settingsSet
 * @param key
 * @param value
 */

void Utils::settingsSet(const QString &key, const QVariant &value)
{
	QSettings s;
	s.setValue(key, value);
}


/**
 * @brief Utils::settingsClear
 * @param key
 */

void Utils::settingsClear(const QString &key)
{
	QSettings s;
	s.remove(key);
}






/**
 * @brief Utils::getRolesFromObject
 * @param object
 * @return
 */

QStringList Utils::getRolesFromObject(const QMetaObject *object)
{
	QStringList roles;

	for (int i = 0 ; i < object->propertyCount(); i++) {
		const QMetaProperty &property = object->property(i);

		if (!property.isValid() || !property.isReadable() || !property.isStored())
			continue;

		const QString &p = property.name();

		if (p == QStringLiteral("objectName"))
			continue;

		roles.append(p);
	}

	return roles;
}


/**
 * @brief Utils::patchSListModel
 * @param model
 * @param keyField
 */

void Utils::patchSListModel(QSListModel *model, const QVariantList &data, const QString &keyField)
{
	Q_ASSERT(model);

	QSDiffRunner runner;

	runner.setKeyField(keyField);

	const QList<QSPatch> &patches = runner.compare(model->storage(), data);

	runner.patch(model, patches);
}


/**
 * @brief Utils::setCliboardText
 * @param text
 */

void Utils::setClipboardText(const QString &text)
{
	QClipboard *clipboard = QGuiApplication::clipboard();

	if (!clipboard) {
		LOG_CERROR("utils") << "Cliboard unavailable";
		return;
	}

	clipboard->setText(text);
}


/**
 * @brief Utils::clipboardText
 * @return
 */

QString Utils::clipboardText()
{
	QClipboard *clipboard = QGuiApplication::clipboard();

	if (!clipboard) {
		LOG_CERROR("utils") << "Cliboard unavailable";
		return QStringLiteral("");
	}

	return clipboard->text();
}


#ifdef CLIENT_UTILS

/**
 * @brief Utils::checkStoragePermissions
 */


void Utils::checkStoragePermissions()
{
#ifdef Q_OS_ANDROID

#if QT_VERSION < 0x060000
	QtAndroid::PermissionResult result1 = QtAndroid::checkPermission("android.permission.READ_EXTERNAL_STORAGE");
	QtAndroid::PermissionResult result2 = QtAndroid::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE");

	QStringList permissions;

	if (result1 == QtAndroid::PermissionResult::Denied)
		permissions.append("android.permission.READ_EXTERNAL_STORAGE");

	if (result2 == QtAndroid::PermissionResult::Denied)
		permissions.append("android.permission.WRITE_EXTERNAL_STORAGE");

	if (!permissions.isEmpty()) {
		QtAndroid::PermissionResultMap resultHash = QtAndroid::requestPermissionsSync(permissions, 30000);

		QList<QtAndroid::PermissionResult> results = resultHash.values();
		if (results.isEmpty() || results.contains(QtAndroid::PermissionResult::Denied)) {
			emit storagePermissionsDenied();
			return;
		}
	}
#else
	emit storagePermissionsDenied();
	return;
#endif
#else

#endif

	emit storagePermissionsGranted();
}





/**
 * @brief Utils::checkMediaPermissions
 */

void Utils::checkMediaPermissions()
{
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS) || defined (Q_OS_MAC)

#if QT_VERSION < 0x060000 && defined(Q_OS_ANDROID)
	QtAndroid::PermissionResult result0 = QtAndroid::checkPermission("android.permission.CAMERA");

	QStringList permissions;

	if (result0 == QtAndroid::PermissionResult::Denied)
		permissions.append("android.permission.CAMERA");

	if (!permissions.isEmpty()) {
		QtAndroid::PermissionResultMap resultHash = QtAndroid::requestPermissionsSync(permissions, 30000);

		QList<QtAndroid::PermissionResult> results = resultHash.values();
		if (results.isEmpty() || results.contains(QtAndroid::PermissionResult::Denied)) {
			emit mediaPermissionsDenied();
			return;
		}
	}
#elif QT_VERSION >= 0x060000
	switch (qApp->checkPermission(QCameraPermission{}))
	{
		case Qt::PermissionStatus::Undetermined:
			qApp->requestPermission(QCameraPermission{}, this, [this](const QPermission &permission) {
				if (permission.status() == Qt::PermissionStatus::Granted)
					emit mediaPermissionsGranted();
				else if (permission.status() == Qt::PermissionStatus::Denied)
					emit mediaPermissionsDenied();
			});
			break;

		case Qt::PermissionStatus::Granted:
			emit mediaPermissionsGranted();
			break;

		case Qt::PermissionStatus::Denied:
			emit mediaPermissionsDenied();
			break;
	}

	return;


#endif

#endif

	emit mediaPermissionsGranted();
}


#endif

/**
 * @brief Utils::versionCode
 * @param major
 * @param minor
 * @return
 */

quint32 Utils::versionCode(const int &major, const int &minor)
{
	return (1000*major)+minor;
}





