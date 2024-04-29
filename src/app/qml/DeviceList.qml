import QtQuick
import QtQuick.Controls
import com.tasktracker.DeviceListModel

Rectangle {
  id: root
  height: parent.height
  width: parent.width

  color: "#333333"
  ListView {
    id: listview
    anchors {
      top: parent.top
      bottom: parent.bottom
      left: parent.left
      right: parent.right
    }

    flickableDirection: Flickable.HorizontalFlick
    orientation: ListView.Horizontal

    clip: true
    model: DeviceListModel
    spacing: 10

    delegate: Rectangle {
      id: delegate

      required property string deviceName
      required property bool isAlarm

      width: listview.width / 2
      height: 70

      color: isAlarm ? "red" : "green"

      Text {
        id: deviceNameText
        minimumPixelSize: 10
        color: "white"
        text: delegate.deviceName
        anchors {
          horizontalCenter: parent.horizontalCenter
          verticalCenter: parent.verticalCenter
          leftMargin: 20
        }

        font {
          pixelSize: 16
          bold: true
        }
      }

      MouseArea {
        id: deviceItemMouseArea
        anchors.fill: parent
        onPressAndHold: {
            DeviceListModel.setSnooze(delegate.index)
        }
      }
    }
  }
}