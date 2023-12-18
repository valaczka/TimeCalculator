/*
 * ---- Call of Suli ----
 *
 * baseapplication.cpp
 *
 * Created on: 2022. 12. 09.
 *     Author: Valaczka János Pál <valaczka.janos@piarista.hu>
 *
 * BaseApplication
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

#include "Logger.h"
#include <QFontDatabase>
#include <QDebug>
#include <Qaterial/Qaterial.hpp>
#include <QMetaObject>
#include <QQuickWindow>

#include "abstractapplication.h"
#include "../version/version.h"
#include "utils_.h"

#ifdef Q_OS_WASM
#include <QtGui/private/qwasmlocalfileaccess_p.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>
#endif


#define WINDOW_ICON     QStringLiteral(":/piar.png")
#define MAIN_FONT       QStringLiteral("Noto Sans")




/// Static variables

const int AbstractApplication::m_versionMajor = VERSION_MAJOR;
const int AbstractApplication::m_versionMinor = VERSION_MINOR;
const int AbstractApplication::m_versionBuild = VERSION_BUILD;
const char *AbstractApplication::m_version = VERSION_FULL;
AbstractApplication *AbstractApplication::m_instance = nullptr;

#ifdef QT_NO_DEBUG
const bool Application::m_debug = false;
#else
const bool AbstractApplication::m_debug = true;
#endif



/// App initialization

/**
 * @brief AbstractApplication::initialize
 */

void AbstractApplication::initialize()
{
	QCoreApplication::setApplicationName(QStringLiteral("TimeCalculator"));
	QCoreApplication::setOrganizationDomain(QStringLiteral("TimeCalculator"));
	QCoreApplication::setApplicationVersion(m_version);
	QGuiApplication::setApplicationDisplayName(QStringLiteral("Gyakorlati idő kalkulátor"));

	QLocale::setDefault(QLocale(QLocale::Hungarian, QLocale::Hungary));
}






/**
 * @brief AbstractApplication::loadMainQml
 * @return
 */

bool AbstractApplication::loadMainQml()
{
	const QUrl url(QStringLiteral("qrc:/main.qml"));
	QObject::connect(m_engine.get(), &QQmlApplicationEngine::objectCreated,
					 m_application, [url](QObject *obj, const QUrl &objUrl) {
		if (!obj && url == objUrl)
			QCoreApplication::exit(-1);

#if defined(Q_OS_ANDROID)
#if QT_VERSION < 0x060000
		QtAndroid::hideSplashScreen();
#else
		QNativeInterface::QAndroidApplication::hideSplashScreen();
#endif
#endif
	}, Qt::QueuedConnection);

	m_engine->load(url);

	return true;
}






/**
 * @brief AbstractApplication::loadFonts
 */

void AbstractApplication::loadFonts(const QStringList &fontList)
{
	LOG_CTRACE("app") << "Load fonts";

	for (const QString &fontPath : fontList) {
		if (QFontDatabase::addApplicationFont(fontPath) == -1) {
			LOG_CWARNING("app") << "Failed to load font:" << qPrintable(fontPath);
		} else {
			LOG_CINFO("app") << "Font loaded:" << qPrintable(fontPath);
		}
	}
}


/**
 * @brief AbstractApplication::loadQaterial
 */

void AbstractApplication::loadQaterial()
{
	m_engine->addImportPath(QStringLiteral("qrc:/"));

	qaterial::setDefaultFontFamily(MAIN_FONT);

	qaterial::loadQmlResources();
	qaterial::registerQmlTypes();
}














/**
 * @brief AbstractApplication::AbstractApplication
 * @param argc
 * @param argv
 */

AbstractApplication::AbstractApplication(QGuiApplication *app)
	: QObject()
	, m_application(app)
	, m_utils(new Utils(this))
{
	Q_ASSERT(!m_instance);
	Q_ASSERT(m_application);

	m_instance = this;

	QObject::connect(m_application, &QCoreApplication::aboutToQuit, [this](){
		m_engine.reset();
	});

	m_engine = std::make_unique<QQmlApplicationEngine>();
}


/**
 * @brief BaseApplication::~BaseApplication
 */

AbstractApplication::~AbstractApplication()
{
	if (m_translator) {
		LOG_CTRACE("app") << "Remove translator";
		m_application->removeTranslator(m_translator.get());
	}

	LOG_CTRACE("app") << "Destroy Application" << this;
}


/**
 * @brief BaseApplication::run
 * @return
 */

int AbstractApplication::run()
{
	registerQmlTypes();
	loadQaterial();

	setAppContextProperty();

	if (!loadResources()) {
		LOG_CERROR("app") << "Failed to load resources";
		return -1;
	}

	if (!loadMainQml()) {
		LOG_CERROR("app") << "Failed to load main qml";
		return -1;
	}

	if (m_engine->rootObjects().isEmpty())
	{
		LOG_CERROR("app") << "Missing root object";
		return -1;
	}

	retranslate(Utils::settingsGet(QStringLiteral("window/language"), QStringLiteral("hu")).toString());

	LOG_CINFO("app") << "Run Application";

	const int r = m_application->exec();

	LOG_CINFO("app") << "Application finished with code" << r;

	return r;
}


/**
 * @brief AbstractApplication::versionMajor
 * @return
 */

int AbstractApplication::versionMajor()
{
	return m_versionMajor;
}

int AbstractApplication::versionMinor()
{
	return m_versionMinor;
}

int AbstractApplication::versionBuild()
{
	return m_versionBuild;
}

const char *AbstractApplication::version()
{
	return m_version;
}


/**
 * @brief AbstractApplication::application
 * @return
 */

QGuiApplication *AbstractApplication::application() const
{
	return m_application;
}


/**
 * @brief AbstractApplication::engine
 * @return
 */

QQmlApplicationEngine *AbstractApplication::engine() const
{
	return m_engine.get();
}




/**
 * @brief AbstractApplication::_message
 * @param text
 * @param title
 * @param type
 */

void AbstractApplication::_message(const QString &text, const QString &title, const QString &type) const
{
	if (m_mainWindow) {
		QMetaObject::invokeMethod(m_mainWindow, "messageDialog", Qt::DirectConnection,
								  Q_ARG(QString, text),
								  Q_ARG(QString, title),
								  Q_ARG(QString, type)
								  );
	}
}




/**
 * @brief AbstractApplication::safeMarginBottom
 * @return
 */

qreal AbstractApplication::safeMarginBottom() const
{
	return m_safeMarginBottom;
}

void AbstractApplication::setSafeMarginBottom(qreal newSafeMarginBottom)
{
	if (qFuzzyCompare(m_safeMarginBottom, newSafeMarginBottom))
		return;
	m_safeMarginBottom = newSafeMarginBottom;
	emit safeMarginBottomChanged();
}


qreal AbstractApplication::safeMarginTop() const
{
	return m_safeMarginTop;
}

void AbstractApplication::setSafeMarginTop(qreal newSafeMarginTop)
{
	if (qFuzzyCompare(m_safeMarginTop, newSafeMarginTop))
		return;
	m_safeMarginTop = newSafeMarginTop;
	emit safeMarginTopChanged();
}

qreal AbstractApplication::safeMarginRight() const
{
	return m_safeMarginRight;
}

void AbstractApplication::setSafeMarginRight(qreal newSafeMarginRight)
{
	if (qFuzzyCompare(m_safeMarginRight, newSafeMarginRight))
		return;
	m_safeMarginRight = newSafeMarginRight;
	emit safeMarginRightChanged();
}

qreal AbstractApplication::safeMarginLeft() const
{
	return m_safeMarginLeft;
}

void AbstractApplication::setSafeMarginLeft(qreal newSafeMarginLeft)
{
	if (qFuzzyCompare(m_safeMarginLeft, newSafeMarginLeft))
		return;
	m_safeMarginLeft = newSafeMarginLeft;
	emit safeMarginLeftChanged();
}



/**
 * @brief AbstractApplication::mainWindow
 * @return
 */

QQuickWindow *AbstractApplication::mainWindow() const
{
	return m_mainWindow;
}

/**
 * @brief AbstractApplication::setMainWindow
 * @param newMainWindow
 */

void AbstractApplication::setMainWindow(QQuickWindow *newMainWindow)
{
	if (m_mainWindow == newMainWindow)
		return;

#ifndef Q_OS_WASM
	if (m_mainWindow)
		disconnect(m_mainWindow, &QQuickWindow::visibilityChanged, this, &AbstractApplication::fullScreenHelperChanged);
#endif

	m_mainWindow = newMainWindow;
	emit mainWindowChanged();

	if (!m_mainWindow)
		return;

#ifndef Q_OS_WASM
	connect(m_mainWindow, &QQuickWindow::visibilityChanged, this, &AbstractApplication::fullScreenHelperChanged);
#endif

	m_mainWindow->setIcon(QIcon(WINDOW_ICON));

	safeMarginsGet();
}




/**
 * @brief AbstractApplication::mainStack
 * @return
 */

QQuickItem *AbstractApplication::mainStack() const
{
	return m_mainStack;
}

void AbstractApplication::setMainStack(QQuickItem *newMainStack)
{
	if (m_mainStack == newMainStack)
		return;
	m_mainStack = newMainStack;
	emit mainStackChanged();
}




/**
 * @brief AbstractApplication::debug
 * @return
 */

bool AbstractApplication::debug() const
{
	return m_debug;
}



/**
 * @brief AbstractApplication::instance
 * @return
 */

AbstractApplication *AbstractApplication::instance()
{
	return m_instance;
}


/**
 * @brief AbstractApplication::utils
 * @return
 */

Utils *AbstractApplication::utils() const
{
	return m_utils.get();
}



/**
 * @brief AbstractApplication::messageInfo
 * @param text
 * @param title
 */

void AbstractApplication::messageInfo(const QString &text, const QString &title) const
{
	QString _title = title;
	if (title.isEmpty())
		_title = m_application->applicationDisplayName();

	LOG_CINFO("app") << qPrintable(text) << _title;
	_message(text, _title, QStringLiteral("info"));
}


/**
 * @brief AbstractApplication::messageWarning
 * @param text
 * @param title
 */

void AbstractApplication::messageWarning(const QString &text, const QString &title) const
{
	QString _title = title;
	if (title.isEmpty())
		_title = m_application->applicationDisplayName();

	LOG_CWARNING("app") << qPrintable(text) << _title;
	_message(text, _title, QStringLiteral("warning"));
}


/**
 * @brief AbstractApplication::messageError
 * @param text
 * @param title
 */

void AbstractApplication::messageError(const QString &text, const QString &title) const
{
	QString _title = title;
	if (title.isEmpty())
		_title = m_application->applicationDisplayName();

	LOG_CERROR("app") << qPrintable(text) << _title;
	_message(text, _title, QStringLiteral("error"));
}



/**
 * @brief AbstractApplication::snack
 * @param text
 */

void AbstractApplication::snack(const QString &text) const
{
	if (m_mainWindow) {
		QMetaObject::invokeMethod(m_mainWindow, "snack", Qt::DirectConnection,
								  Q_ARG(QString, text)
								  );
	}
}



/**
 * @brief AbstractApplication::stackPushPage
 * @param qml
 * @param parameters
 * @return
 */

QQuickItem *AbstractApplication::stackPushPage(QString qml, QVariantMap parameters) const
{
	if (!m_mainStack) {
		LOG_CERROR("app") << "Missing MainStack!";
		return nullptr;
	}

	if (qml.isEmpty()) {
		LOG_CWARNING("app") << "Empty qml";
		return nullptr;
	}

	QQuickItem *o = nullptr;
	QMetaObject::invokeMethod(m_mainStack, "createPage", Qt::DirectConnection,
							  Q_RETURN_ARG(QQuickItem*, o),
							  Q_ARG(QString, qml),
							  Q_ARG(QVariant, parameters)
							  );

	if (!o) {
		messageError(tr("Nem lehet a lapot betölteni!"), qml);
		return nullptr;
	}

	LOG_CDEBUG("app") << "Lap betöltve:" << qPrintable(qml) << o;

	return o;
}




/**
 * @brief AbstractApplication::stackPop
 * @param index
 * @param forced
 * @return
 */

bool AbstractApplication::stackPop(int index, const bool &forced) const
{
	if (!m_mainStack) {
		LOG_CERROR("app") << "Missing MainStack!";
		return false;
	}

	QQuickItem *currentItem = qvariant_cast<QQuickItem*>(m_mainStack->property("currentItem"));

	if (!currentItem) {
		LOG_CERROR("app") << "MainStack currentItem unavailable";
		return false;
	}

	const int &depth = m_mainStack->property("depth").toInt();

	if (index == -1) {
		index = depth-2;
	}


	LOG_CTRACE("app") << "Stack pop" << index << depth;

	if (index >= depth-1) {
		LOG_CWARNING("app") << "Nem lehet a lapra visszalépni:" << index << "mélység:" << depth;
		return false;
	}

	bool canPop = true;

	QMetaObject::invokeMethod(m_mainStack, "callStackPop", Qt::DirectConnection,
							  Q_RETURN_ARG(bool, canPop)
							  );

	if (!canPop && !forced)
		return false;



	if (depth <= 2) {
#if !defined(Q_OS_IOS) && !defined(Q_OS_ANDROID)
		// LOG_CDEBUG("app") << "Nem lehet visszalépni, mélység:" << depth;
		//Application::instance()->messageInfo("Nem lehet visszalépni!");
		return false;
#endif

		m_mainWindow->close();
		return true;
	}




	QString closeDisabled = currentItem->property("closeDisabled").toString();
	QString question = currentItem->property("closeQuestion").toString();

	if (!closeDisabled.isEmpty() && !forced) {
		messageWarning(closeDisabled);
		return false;
	}

	if (forced || question.isEmpty()) {
		QMetaObject::invokeMethod(m_mainStack, "popPage", Qt::DirectConnection,
								  Q_ARG(int, index)
								  );

		return true;
	}

	LOG_CDEBUG("app") << "Kérdés a visszalépés előtt" << currentItem;

	QMetaObject::invokeMethod(m_mainWindow, "closeQuestion", Qt::DirectConnection,
							  Q_ARG(QString, question),
							  Q_ARG(bool, true),						// _pop
							  Q_ARG(int, index)
							  );

	return false;
}



/**
 * @brief AbstractApplication::stackPop
 * @param page
 * @return
 */

bool AbstractApplication::stackPop(QQuickItem *page) const
{
	if (!page)
		return false;

	QQmlProperty property(page, "StackView.index", qmlContext(page));
	return stackPop(property.read().toInt()-1, true);
}



/**
 * @brief AbstractApplication::stackPopToPage
 * @param page
 * @return
 */

bool AbstractApplication::stackPopToPage(QQuickItem *page) const
{
	if (!page)
		return false;

	QQmlProperty property(page, "StackView.index", qmlContext(page));
	return stackPop(property.read().toInt(), true);
}




/**
 * @brief AbstractApplication::closeWindow
 * @param forced
 * @return
 */

bool AbstractApplication::closeWindow(const bool &forced)
{
	if (m_mainWindowClosable)
		return true;

	QQuickItem *currentItem = qvariant_cast<QQuickItem*>(m_mainStack->property("currentItem"));

	if (!currentItem) {
		LOG_CERROR("app") << "MainStack currentItem unavailable";
		return false;
	}

	QString closeDisabled = currentItem->property("closeDisabled").toString();
	QString question = currentItem->property("closeQuestion").toString();

	if (!closeDisabled.isEmpty()) {
		messageWarning(closeDisabled);
		return false;
	}


	if (forced || question.isEmpty()) {
		LOG_CDEBUG("app") << "Ablak bezárása";
		m_mainWindowClosable = true;
		m_mainWindow->close();
		return true;
	}

	LOG_CDEBUG("app") << "Kérdés a bezárás előtt" << currentItem;

	QMetaObject::invokeMethod(m_mainWindow, "closeQuestion", Qt::DirectConnection,
							  Q_ARG(QString, question),
							  Q_ARG(bool, false),					// _pop
							  Q_ARG(int, -1)						// _index
							  );

	return false;
}



/**
 * @brief AbstractApplication::notifyWindow
 */

void AbstractApplication::notifyWindow()
{
	LOG_CDEBUG("app") << "Notify window";

	if (!m_mainWindow)
		return;

#if (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)) || defined(Q_OS_WIN) || defined(Q_OS_MACOS)
	if (m_mainWindow->visibility() == QWindow::Minimized)
		m_mainWindow->show();
#endif

	m_mainWindow->raise();
	m_mainWindow->alert(0);
	m_mainWindow->requestActivate();
}




/**
 * @brief AbstractApplication::safeMarginsGet
 */

void AbstractApplication::safeMarginsGet()
{
	QMarginsF margins;

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	margins = MobileUtils::getSafeMargins();
#else
	const QString &str = QString::fromUtf8(qgetenv("SAFE_MARGINS"));

	if (!str.isEmpty()) {
		margins.setTop(str.section(',', 0, 0).toDouble());
		margins.setLeft(str.section(',', 1, 1).toDouble());
		margins.setBottom(str.section(',', 2, 2).toDouble());
		margins.setRight(str.section(',', 3, 3).toDouble());

		setSafeMargins(margins);
		return;
	}


	QPlatformWindow *platformWindow = m_mainWindow->handle();
	if(!platformWindow) {
		LOG_CERROR("app") << "Invalid QPlatformWindow";
		return;
	}
#endif

	LOG_CDEBUG("app") << "New safe margins:" << margins;

	setSafeMargins(margins);
}



/**
 * @brief AbstractApplication::setSafeMargins
 * @param margins
 */

void AbstractApplication::setSafeMargins(const QMarginsF &margins)
{
	setSafeMarginLeft(margins.left());
	setSafeMarginRight(margins.right());
	setSafeMarginTop(margins.top());
	setSafeMarginBottom(margins.bottom());
}



/**
 * @brief AbstractApplication::getDevicePixelSizeCorrection
 * @return
 */

qreal AbstractApplication::getDevicePixelSizeCorrection() const
{
	const qreal refDpi = 72.;
	const qreal refHeight = 700.;
	const qreal refWidth = 400.;
	const QRect &rect = QGuiApplication::primaryScreen()->geometry();
	const qreal &height = qMax(rect.width(), rect.height());
	const qreal &width = qMin(rect.width(), rect.height());

	const qreal &dpi = QGuiApplication::primaryScreen()->logicalDotsPerInch();

	qreal ratioFont = qMin(1., qMin(height*refDpi/(dpi*refHeight), width*refDpi/(dpi*refWidth)));

	LOG_CDEBUG("app") << "Device pixel size correction:" << ratioFont;

	return ratioFont;
}



/**
 * @brief AbstractApplication::retranslate
 * @param language
 */

void AbstractApplication::retranslate(const QString &language)
{
	QLocale locale(language);

	LOG_CINFO("app") << "Retranslate:" << qPrintable(language) << "-" << qPrintable(locale.name());

	std::unique_ptr<QTranslator> translator(new QTranslator());

	if (translator->load(locale, QStringLiteral("qt"), QStringLiteral("_"), QStringLiteral(":/"))) {
		LOG_CDEBUG("app") << "Translator file loaded:" << qPrintable(translator->filePath());

		if (m_translator)
			m_application->removeTranslator(m_translator.get());

		m_application->installTranslator(translator.get());
		m_translator = std::move(translator);
		m_engine->retranslate();
	} else {
		LOG_CWARNING("app") << "Can't load translator language:" << qPrintable(locale.name());
	}
}



/**
 * @brief AbstractApplication::fullScreenHelper
 * @return
 */

bool AbstractApplication::fullScreenHelper() const
{
	return m_mainWindow && m_mainWindow->visibility() == QWindow::FullScreen;
}



/**
 * @brief AbstractApplication::setFullScreenHelper
 * @param newFullScreenHelper
 */

void AbstractApplication::setFullScreenHelper(bool newFullScreenHelper)
{
	if (m_mainWindow)
		m_mainWindow->setVisibility(newFullScreenHelper ? QWindow::FullScreen : QWindow::Maximized);
}




#ifdef Q_OS_WASM


/**
 * @brief AbstractApplication::enableTabCloseConfirmation
 * @param enable
 */

void AbstractApplication::enableTabCloseConfirmation(bool enable)
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




/**
 * @brief AbstractApplication::wasmOpen
 * @param accept
 * @param func
 */

void AbstractApplication::wasmOpen(const QString &accept, std::function<void (const QByteArray &, const QString &)> func)
{
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
	[fileData, func]() {
		QByteArray content = fileData->buffer;
		QString name = fileData->name;

		if (func)
			func(content, name);

		delete fileData;
	});
}



/**
 * @brief AbstractApplication::wasmSave
 * @param content
 * @param name
 */

void AbstractApplication::wasmSave(const QByteArray &content, const QString &name)
{
	QWasmLocalFileAccess::saveFile(content.constData(), size_t(content.size()),
								   name.toStdString());
}



namespace {
void beforeUnloadhandler(emscripten::val event) {
	event.call<void>("preventDefault"); // call preventDefault as required by standard
	event.set("returnValue", std::string(" ")); // set returnValue to something, as required by Chrome
}
}

EMSCRIPTEN_BINDINGS(app) {
	function("app_beforeUnloadHandler", &beforeUnloadhandler);
}

#endif
