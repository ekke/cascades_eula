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
#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>
#include <qvariant.h>

namespace bb {
namespace cascades {
class Application;
class LocaleHandler;
}
}

class QTranslator;

using namespace bb::cascades;

class ApplicationUI: public QObject {
Q_OBJECT
public:
	ApplicationUI(bb::cascades::Application *app);
	virtual ~ApplicationUI() {
	}

	Q_INVOKABLE
	bool showEula();

	Q_INVOKABLE
	QVariant eulaContent();

	Q_INVOKABLE
	void setEulaAccepted();

private slots:
	void onSystemLanguageChanged();
private:
	QTranslator* m_pTranslator;
	bb::cascades::LocaleHandler* m_pLocaleHandler;

	QVariantMap euladoc(const QString& locale);
	QVariantList readEulaFromJson();

};

#endif /* ApplicationUI_HPP_ */
