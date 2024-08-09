import QtQuick 2.0
import QtQuick.Controls 2.5

Button {
    font.pointSize: 12
    font.bold: true

    background: Rectangle {
        radius: parent.height/5
        border.color: "black"
        color: parent.hovered ? "silver" : "white"
        border.width: parent.down ? 3 : 1
    }
}
