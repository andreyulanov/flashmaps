import QtQuick 2.5
import QtQuick.Controls 2.5
import QtLocation 5.15
import QtQuick.Layouts 1.11

Popup {
        id: addClass

        modal: true
        closePolicy: Popup.NoAutoClose
        signal widthChanged(string widthLine)

        Text {
            id: headerClass
            anchors.margins: 5

            text: "New Class"
            font.bold: true
        }

        ComboBox {
            id: comboBoxForm
            anchors.left: parent.left
            anchors.top: headerClass.bottom
            anchors.margins: 5
            height: 50
            width: parent.width

            background: Rectangle {
                radius: parent.height/5
                border.color: "black"
                color: parent.hovered ? "silver" : "white"
                border.width: parent.down ? 3 : 1
            }

            model: ["Point", "Line", "Rectangle"]



        }



        GridLayout {
            id: gridColor
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: comboBoxForm.bottom
            anchors.margins: 5

            columns: 2
            rowSpacing: 2
            columnSpacing: 10

            Text{ text: "Line width, mm"}
            TextField {
                id:widthLine
                Layout.fillWidth: true
                font.pointSize: 12

                background: Rectangle {

                    radius: parent.height/5
                    color: "transparent"
                    border.color: "skyblue"
                }

                validator: IntValidator {bottom: 1; top: 10}

                //onWidthChanged: {            }


                onEditingFinished: console.log("Width changed")
            }


            Text{ text: "Line color"}
            TextFieldColor {
                id: lineColor; Layout.fillWidth: true; placeholderText: qsTr("HEX color")
            }

            Text{ text: "Background color"}
            TextFieldColor {id:backgroundColor; Layout.fillWidth: true; placeholderText: qsTr("HEX color")}

            Text{ text: "Text color"}
            TextFieldColor {id: textColor; Layout.fillWidth: true; placeholderText: qsTr("HEX color")}

            Text{ text: "Layer"}
            TextField {
                id: layerNumber
                Layout.fillWidth: true
                font.pointSize: 12

                background: Rectangle {

                    radius: parent.height/5
                    color: "transparent"
                    border.color: "skyblue"
                }

                validator: IntValidator {
                    bottom: 0; top: 23
                    }

                placeholderText: qsTr("between 0 and 23")
                color: acceptableInput ? "black" : "red"

            }

            Text{ text: "Max mip"}
            TextField {
                id:maxMip
                Layout.fillWidth: true

                background: Rectangle {

                    radius: parent.height/5
                    color: "transparent"
                    border.color: "skyblue"
                }
            }

        }

        Item {
            width: parent.width
            anchors.top: gridColor.bottom
            anchors.margins: 5
            anchors.bottom: parent.bottom


            AddButton {

                anchors.left: parent.left
                anchors.top: parent.top
                height: parent.height
                width: parent.width/2 - 10

                text: qsTr("Add")

                onClicked: {
                    popupClass.close()
                    console.log(comboBoxForm.currentText)
                    console.log("line width "+ widthLine.text)
                    console.log("line color " + lineColor.text)
                    console.log("background color " + backgroundColor.text)
                    console.log("text color " + textColor.text)
                    console.log("layer " + layerNumber.text)
                    console.log("max mip " + maxMip.text)


                }
            }

            AddButton {

                anchors.right: parent.right
                anchors.top: parent.top
                height: parent.height
                width: parent.width/2 - 10

                text: qsTr("Cancel")

                onClicked: popupClass.close()
            }
        }





}
