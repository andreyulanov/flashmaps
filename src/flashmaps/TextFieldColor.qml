import QtQuick 2.0
import QtQuick.Controls 2.5

TextField {

    background: Rectangle {
        radius: parent.height/5
        color: "transparent"
        border.color: "skyblue"
    }

    font.pointSize: 12
    maximumLength: 6
}
