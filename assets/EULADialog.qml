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

Dialog {
    id: eulaDialog
    property string result
    property alias title: titleLabel.text
    property alias body: bodyLabel.text
    property alias button1: acceptButton.text
    property alias button2: cancelButton.text
    Container {
        id: dialogContainer
        // we need a solid background, so nothing shines through from the App
        // if it's a dark theme, we use Black - otherwise White
        background: isDark() ? Color.Black : Color.White
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        Container {
            id: titleContainer
            // for the titlebar we're using Blue
            background: Color.Blue
            topPadding: 5
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            minHeight: 90
            maxHeight: 90
            Label {
                id: titleLabel
                text: "EULA"
                textStyle.base: SystemDefaults.TextStyles.TitleText
                textStyle.color: Color.White
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    //spaceQuota: 1
                }
            }
        } // end titlecontainer
        Container {
            id: contentContainer
            // if your text is short, you can use some more pixels for padding
            leftPadding: 20
            rightPadding: 20
            // always use a ScrollView, so all works on small screens like Q10
            ScrollView {
                preferredWidth: Infinity
                Container {
                    id: bodyContainer
                    topPadding: 60
                    Label {
                        id: bodyLabel
                        text: ""
                        multiline: true
                    }
                    // the buttons are always below the body text,
                    // so the user has to scroll dwon to read all text and to see the buttons
                    // you can rearrange if buttons should always be visible
                    Container {
                        id: buttonContainer
                        topPadding: 20
                        Container {
                            id: cancelButtonContainer
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }  
                            Button {
                                id: cancelButton
                                text: qsTr("Cancel")
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 1
                                }
                                onClicked: {
                                    eulaDialog.result = "CANCELED"
                                    eulaDialog.close()
                                }
                            }
                            Button {
                                id: acceptButton
                                text: qsTr("I Agree")
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 1
                                }
                                focusPolicy: FocusPolicy.KeyAndTouch
                                onClicked: {
                                    eulaDialog.result = "ACCEPTED"
                                    eulaDialog.close()
                                }
                            }
                        }
                    } // end buttonContainer
                } // end body container
            } // end scrollView
        } // end contentContainer
    }
    function isDark() {
        return Application.themeSupport.theme.colorTheme.style == VisualStyle.Dark
    }
}