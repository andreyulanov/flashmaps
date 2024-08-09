import QtQuick 2.5
import QtQuick.Controls 2.5
import QtLocation 5.15
import QtQuick.Layouts 1.11

Popup {
        id: addObject

        modal: true
        closePolicy: Popup.NoAutoClose

        //signal nameChanged(string objectName)

        Text {
            id: header
            anchors.margins: 5

            text: "New Object"
            font.bold: true
        }

        Item {
            id: boxClass
            width: parent.width
            anchors.top: header.bottom
            anchors.margins: 5
            height: 50


            ComboBox {
                id: comboBoxClass
                anchors.left: parent.left
                anchors.top: parent.top
                height: parent.height
                width: parent.width/2 - 10

                background: Rectangle {
                    radius: parent.height/5
                    border.color: "black"
                    color: parent.hovered ? "silver" : "white"
                    border.width: parent.down ? 3 : 1
                }

                model: ["Not selected", "FirstClass", "SecondClass"]



            }

            NewClass{
                id: popupClass
                anchors.centerIn: Overlay.overlay

                width: objectButton.width * 2
                height: 350
            }

            AddButton {
                    id: classButton
                    anchors.right: parent.right
                    anchors.top: parent.top
                    height: parent.height
                    width: parent.width/2 - 10

                    text: "Add new class"

                    onClicked: popupClass.open()


             }

        }

        TextField {
            id: objectName
            anchors.margins: 5
            height: parent.height/5
            anchors.top: boxClass.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            font.pointSize: 12

            background: Rectangle {

                radius: parent.height/5
                color: "transparent"
                border.color: "skyblue"
            }

            placeholderText: qsTr("Object name")

            /*onTextChanged: {
                nameChanged(text)
                //console.log(" Name changed")
            }*/
          }

        Item {
            id:attribute
            anchors.margins: 5
            height: parent.height/5
            anchors.top: objectName.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            Text{
                font.pointSize: 12
                text: "Attributes"
            }


        }

        Item {
            width: parent.width
            anchors.top: attribute.bottom
            anchors.margins: 5
            anchors.bottom: parent.bottom


            AddButton {

                anchors.left: parent.left
                anchors.top: parent.top
                height: parent.height
                width: parent.width/2 - 10

                text: qsTr("Add")

                onClicked: {
                    popupObject.close()
                    console.log(comboBoxClass.currentText)
                    console.log("Object name "+ objectName.text)

                }
            }

            AddButton {

                anchors.right: parent.right
                anchors.top: parent.top
                height: parent.height
                width: parent.width/2 - 10

                text: qsTr("Cancel")

                onClicked: popupObject.close()
            }
        }

}
