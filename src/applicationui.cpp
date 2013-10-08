/* Copyright (c) 2013 Ekkehard Gentz (ekke)
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/data/JsonDataAccess>

#include <QTimer>
#include <QSettings>

static const QString SETTINGS_KEY_EULA_ACCEPTED = "eula_read";

using namespace bb::cascades;
using namespace bb::data;

ApplicationUI::ApplicationUI(bb::cascades::Application *app) :
		QObject(app) {
	// prepare the localization
	m_pTranslator = new QTranslator(this);
	m_pLocaleHandler = new LocaleHandler(this);
	if (!QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()),
			this, SLOT(onSystemLanguageChanged()))) {
		// This is an abnormal situation! Something went wrong!
		// Add own code to recover here
		qWarning() << "Recovering from a failed connect()";
	}
	// initial load
	onSystemLanguageChanged();

	// QTimer
	qmlRegisterType<QTimer>("my.library", 1, 0, "QTimer");

	// Create scene document from main.qml asset, the parent is set
	// to ensure the document gets destroyed properly at shut down.
	QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

	// access to the Application
	qml->setContextProperty("app", this);

	// Create root object for the UI
	AbstractPane *root = qml->createRootObject<AbstractPane>();

	// Set created root object as the application scene
	app->setScene(root);
}

void ApplicationUI::onSystemLanguageChanged() {
	QCoreApplication::instance()->removeTranslator(m_pTranslator);
	// Initiate, load and install the application translation files.
	QString locale_string = QLocale().name();
	QString file_name = QString("cascades_eula_%1").arg(locale_string);
	if (m_pTranslator->load(file_name, "app/native/qm")) {
		QCoreApplication::instance()->installTranslator(m_pTranslator);
	}
}

/**
 * this will only be called if the user ACCEPTED the EULA
 * otherwise there will be no settings entry for this key
 */
void ApplicationUI::setEulaAccepted() {
	QSettings settings;
	bool accepted = true;
	settings.setValue(SETTINGS_KEY_EULA_ACCEPTED, QVariant(accepted));
}

/**
 * If no key is stored in settings,
 * we know that the user has not accepted the EULA yet
 * so we have to show the dialog
 */
bool ApplicationUI::showEula() {
	QSettings settings;
	if (settings.value(SETTINGS_KEY_EULA_ACCEPTED).isNull()) {
		return true;
	}
	return false;
}

/**
 * construct a Map for localized EULA
 *
 */
QVariant ApplicationUI::eulaContent() {
	// the current locale from the device
	QString myLocale;
	// Hint: this won't work: myLocale = QLocale::system().name();
	// we have to construct an instance
	myLocale = QLocale().name();
	// now myLocale contains the locale: 'en', 'de', 'ca_fr', 'ca_en', ...
	// this Map will be used as JS Object in QML to fill the dialog
	QVariantMap eulaMap;
	// all localized EULA's are stored in assets/app_data in eula.json
	eulaMap = euladoc(myLocale);
	if (!eulaMap.isEmpty()) {
		return eulaMap;
	}
	// FALLBACK if there's no eula.json found
	// we'll construct a default english EULA
	qWarning() << "no EULA found ! must be stored in assets/app_data/eula.json";
	eulaMap.insert("title", "LICENSE ACKNOWLEDGEMENT");
	QString body;
	body =
			"This software is licensed under Apache License 2.0";
	body += "\nBY CLICKING ";
	body += '"';
	body += "I AGREE";
	body += '"';
	body +=
			" BELOW, YOU ARE AGREEING THAT YOU ACCEPT THIS LICENSE ";
	body += "\n";
	eulaMap.insert("body", body);
	eulaMap.insert("button1", "I Agree");
	eulaMap.insert("button2", "I don't agree");
	return eulaMap;
}

QVariantMap ApplicationUI::euladoc(const QString& locale) {
	qDebug() << "get eula for " << locale;
	QVariantMap eulaMap;
	if (locale.isEmpty()) {
		qDebug() << "cannot get eula without locale";
		return eulaMap;
	}
	if (locale.length() < 2) {
		qDebug() << "cannot get eula for locale " << locale;
		return eulaMap;
	}
	QVariantList eulaList;
	eulaList = readEulaFromJson();

	// search locale and country
	for (int i = 0; i < eulaList.size(); ++i) {
		QVariantMap theMap;
		theMap = eulaList.at(i).toMap();
		if (theMap.value("locale").toString() == locale) {
			qDebug() << "found country and language " << locale;
			return theMap;
		}
	}
	// search for language only
	QString language = locale.left(2);
	for (int i = 0; i < eulaList.size(); ++i) {
		QVariantMap theMap;
		theMap = eulaList.at(i).toMap();
		if (theMap.value("locale").toString() == language) {
			qDebug() << "found language " << locale;
			return theMap;
		}
	}
	language = "en";
	for (int i = 0; i < eulaList.size(); ++i) {
		QVariantMap theMap;
		theMap = eulaList.at(i).toMap();
		if (theMap.value("locale").toString() == language) {
			qDebug() << "got en as fallback ";
			return theMap;
		}
	}
	if (eulaList.size() < 1) {
		qDebug() << "got no documents for eula";
		return eulaMap;
	}
	return eulaList.at(0).toMap();
}

QVariantList ApplicationUI::readEulaFromJson() {
	JsonDataAccess jda;
	QVariantList eulaList;
	QString eulaFilePath;
	eulaFilePath = QDir::currentPath() + "/app/native/assets/app_data/eula.json";
	QFile eulaFile(eulaFilePath);
	// test if file exists
	if (!eulaFile.exists()) {
		qDebug() << "no eulaFile file found in assets - using english";
		return eulaList;
	}
	bool ok = eulaFile.open(QIODevice::ReadOnly);
	if (ok) {
		eulaList = jda.loadFromBuffer(eulaFile.readAll()).toList();
		eulaFile.close();
	} else {
		qDebug() << "cannot read eulaFile file: " << eulaFilePath;
	}
	return eulaList;
}

