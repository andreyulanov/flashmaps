import QtQuick 2.0
import QtQuick.Window 2.14
import QtLocation 5.6
import QtPositioning 5.6

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
}
