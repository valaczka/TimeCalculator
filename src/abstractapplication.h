/*
 * ---- Call of Suli ----
 *
 * baseapplication.h
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

#ifndef ABSTRACTAPPLICATION_H
#define ABSTRACTAPPLICATION_H

#include <QGuiApplication>
#include <QQuickWindow>
#include <QQuickItem>
#include <QtQml>

class Utils;

#ifndef OPAQUE_PTR_Utils
#define OPAQUE_PTR_Utils
Q_DECLARE_OPAQUE_POINTER(Utils*)
#endif


/**
 * @brief The AbstractApplication class
 */

class AbstractApplication : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QQuickItem *mainStack READ mainStack WRITE setMainStack NOTIFY mainStackChanged FINAL)
    Q_PROPERTY(QQuickWindow *mainWindow READ mainWindow WRITE setMainWindow NOTIFY mainWindowChanged FINAL)

    Q_PROPERTY(qreal safeMarginLeft READ safeMarginLeft WRITE setSafeMarginLeft NOTIFY safeMarginLeftChanged FINAL)
    Q_PROPERTY(qreal safeMarginRight READ safeMarginRight WRITE setSafeMarginRight NOTIFY safeMarginRightChanged FINAL)
    Q_PROPERTY(qreal safeMarginTop READ safeMarginTop WRITE setSafeMarginTop NOTIFY safeMarginTopChanged FINAL)
    Q_PROPERTY(qreal safeMarginBottom READ safeMarginBottom WRITE setSafeMarginBottom NOTIFY safeMarginBottomChanged FINAL)

    Q_PROPERTY(bool fullScreenHelper READ fullScreenHelper WRITE setFullScreenHelper NOTIFY fullScreenHelperChanged)

    Q_PROPERTY(Utils* Utils READ utils CONSTANT)
    Q_PROPERTY(bool debug READ debug CONSTANT)

public:
    AbstractApplication(QGuiApplication *app);
    virtual ~AbstractApplication();

    int run();

    static int versionMajor();
    static int versionMinor();
    static int versionBuild();
    static const char *version();
    static void initialize();

    QGuiApplication *application() const;
    QQmlApplicationEngine *engine() const;

    static AbstractApplication *instance();
    Utils *utils() const;
    bool debug() const;

    Q_INVOKABLE void messageInfo(const QString &text, const QString &title = "") const;
    Q_INVOKABLE void messageWarning(const QString &text, const QString &title = "") const;
    Q_INVOKABLE void messageError(const QString &text, const QString &title = "") const;
    Q_INVOKABLE void snack(const QString &text) const;

    Q_INVOKABLE QQuickItem *stackPushPage(QString qml, QVariantMap parameters = {}) const;
    Q_INVOKABLE bool stackPop(int index = -1, const bool &forced = false) const;
    Q_INVOKABLE bool stackPop(QQuickItem *page) const;
    Q_INVOKABLE bool stackPopToPage(QQuickItem *page) const;

    Q_INVOKABLE bool closeWindow(const bool &forced = false);
    Q_INVOKABLE void notifyWindow();

    Q_INVOKABLE void safeMarginsGet();
    void setSafeMargins(const QMarginsF &margins);

    Q_INVOKABLE qreal getDevicePixelSizeCorrection() const;

    Q_INVOKABLE void retranslate(const QString &language = QStringLiteral("hu"));

    bool fullScreenHelper() const;
    void setFullScreenHelper(bool newFullScreenHelper);

    QQuickItem *mainStack() const;
    void setMainStack(QQuickItem *newMainStack);

    QQuickWindow *mainWindow() const;
    void setMainWindow(QQuickWindow *newMainWindow);

    qreal safeMarginLeft() const;
    void setSafeMarginLeft(qreal newSafeMarginLeft);

    qreal safeMarginRight() const;
    void setSafeMarginRight(qreal newSafeMarginRight);

    qreal safeMarginTop() const;
    void setSafeMarginTop(qreal newSafeMarginTop);

    qreal safeMarginBottom() const;
    void setSafeMarginBottom(qreal newSafeMarginBottom);

public slots:
    virtual void onApplicationStarted() = 0;

signals:
    void fullScreenHelperChanged();
    void mainStackChanged();
    void mainWindowChanged();
    void safeMarginLeftChanged();
    void safeMarginRightChanged();
    void safeMarginTopChanged();
    void safeMarginBottomChanged();

protected:
    virtual bool loadMainQml();
    virtual bool loadResources() = 0;
    virtual void registerQmlTypes() = 0;
    virtual void setAppContextProperty() = 0;

    void loadFonts(const QStringList &fontList);
    void loadQaterial();

protected:
    static const int m_versionMajor;
    static const int m_versionMinor;
    static const int m_versionBuild;
    static const char* m_version;

    static AbstractApplication *m_instance;
    static const bool m_debug;

    QGuiApplication *const m_application;
    std::unique_ptr<QQmlApplicationEngine> m_engine;
    std::unique_ptr<Utils> m_utils;
    std::unique_ptr<QTranslator> m_translator;

    QQuickItem *m_mainStack = nullptr;
    QQuickWindow *m_mainWindow = nullptr;
    bool m_mainWindowClosable = false;

    qreal m_safeMarginLeft = 0;
    qreal m_safeMarginRight = 0;
    qreal m_safeMarginTop = 0;
    qreal m_safeMarginBottom = 0;

private:
    void _message(const QString &text, const QString &title, const QString &type) const;

};

#endif // ABSTRACTAPPLICATION_H

