import QtQuick 2.0
import QtQuick.Window 2.14
import QtLocation 5.6
import QtPositioning 5.6
import QtQuick.Controls 2.5


Window {
    width: Screen.width
    height: Screen.height
    visible: true

    Plugin {
        id: mapPlugin
        name: "flashmaps"
    }

    Map {
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(59.9768, 30.3649)
        zoomLevel: 17
    }

    Item{
        anchors.top: parent.top
        anchors.left: parent.left
        height: 50
        width: 200


        AddButton {
                id: objectButton

                anchors.fill: parent
                anchors.margins: 5

                text: "Add new object"

                onClicked: popupObject.open()

        }

        NewObject{
            id: popupObject
            anchors.centerIn: Overlay.overlay

            width: objectButton.width * 2
            height: 250
        }

    }
}
