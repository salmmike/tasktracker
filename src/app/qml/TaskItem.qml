import QtQuick
import QtQuick.Controls

Item {
    id: root

    property alias taskName: taskName.text
    property alias startTime: startTime.text
    property alias doneButton: doneButton

    visible: true

    Rectangle {
        id: backround
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
        }
    }

    Text {
        id: startTime
    
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
        }

        font {
            pixelSize: 20
            bold: true
        }
        color: "white"
    }

    Text {
        id: taskName
        anchors {
            verticalCenter: startTime.verticalCenter
            left: startTime.right
        }
        padding: 20
        color: "white"

        font {
            pixelSize: 20
        }
    }

    Button {
        id: doneButton
        text: "Done!"
        padding: 5
        anchors {
            right: parent.right
            verticalCenter: parent.verticalCenter
        }
    }
}
