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
import bb.cascades 1.0
import bb.system 1.0
import my.library 1.0

/**
 * This sample is extracted from the BlackBerry Conference App for BBJam Asia 2013,
 * some small modifications done to make it easy to understand and to insert into your apps
 * 
 * You can download the conference app from BlackBerry World
 * 
 * more info also here:
 * http://devblog.blackberry.com/2013/09/blackberry-jam-asia-conference-app-now-live/
 * and from my blogs
 * http://ekkes-corner.org
 * http://ekkes-apps.org
 * 
 */
Page {
    attachedObjects: [
        // this is the EULA Dialog
        // the Dialog will be opened at startup, if the user hasn't already accepted EULA
        // if user cancels the EULA, the noEulaDialog will be opened one time
        // then user has a chance to accept the EULA - if not, the application exits
        EULADialog {
            id: eulaDialog
            property bool firstRun: true
            onClosed: {
                if (eulaDialog.result == "ACCEPTED") {
                    console.log("ACCEPTED");
                    app.setEulaAccepted()
                    // do your normal startup stuff now
                    return
                }
                if (firstRun) {
                    noEulaDialog.exec()
                    return
                }
                console.log("CANCEL EULA");
                Application.requestExit();
            }
        },
        // if user cancels EULA dialog, then this Dialog will come up
        // to inform the user that he/she must accept to start the APP
        SystemDialog {
            id: noEulaDialog
            title: "EULA License"
            body: qsTr("You must accept the EULA in order to use the App.") + Retranslate.onLanguageChanged
            confirmButton.label: qsTr("OK") + Retranslate.onLanguageChanged
            confirmButton.enabled: true
            cancelButton.enabled: false
            onFinished: {
                eulaDialog.firstRun = false
                eula()
            }
        },
        // single shot QTimer to open the EULA Dialog
        // using this timer will guarantee that there are no side-effects while QML is warming up
        QTimer {
            id: eulaTimer
            interval: 500
            singleShot: true
            onTimeout: {
                eula()
            }
        }
    ]
    Container {
        // this Container is only a placeholder where your APP content will be
        // normaly this is a TabbedPane or NavigationPane
        Label {
            text: qsTr("Welcome") + Retranslate.onLocaleOrLanguageChanged
            textStyle.base: SystemDefaults.TextStyles.TitleText
        }
        Label {
            id: doItAgin
            text: qsTr("To try it again:\nFrom TargetFileSystemNavigator\nSandbox of this APP\ndata/Settings\ndelete cascades_eula.conf") + Retranslate.onLocaleOrLanguageChanged
            textStyle.base: SystemDefaults.TextStyles.SubtitleText
            visible: false
            multiline: true
        }
    }
    // this function will be called from QTimer 
    function eula() {
        // get the localized EULA content from cpp
        // it's a QVariantMap with title, body and acceppt / cancel button texts
        var data = app.eulaContent()
        eulaDialog.title = data.title
        eulaDialog.body = data.body
        eulaDialog.button1 = data.button1
        eulaDialog.button2 = data.button2
        // now it's safe to open the Dialog
        eulaDialog.open()
    }
    onCreationCompleted: {
        // check if EULA was not accepted yet
        // HINT: don't open the EULA Dialog directly, there are situations on older 10.1 OS releases where your app freezes
        // so we're using a short delay with a one-shot QTimer starting the Dialog async
        if (app.showEula()) {
            // no EULA yet: start the timer to open the EULA Dialog
            eulaTimer.start()
        } else {
            // do your normal startup stuff now
            doItAgin.visible = true
        }
    }
}
