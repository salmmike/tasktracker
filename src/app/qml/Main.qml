import QtQuick
import QtQuick.Controls
import com.tasktracker.TaskListModel
import com.tasktracker.DeviceListModel

ApplicationWindow {
  id: root
  visible: true
  title: qsTr("Tasktracker")
  visibility: Window.FullScreen
  // Without setting these, the window isn't showing correct
  // even though one might think they aren't needed when FullScreen is set.
  width: Screen.width
  height: Screen.height

  flags: Qt.WA_AcceptTouchEvents

  Rectangle {
    id: topOptionsBar
    height: 70
    color: "#333333"

    anchors {
      top: parent.top
      left: parent.left
      right: parent.right
    }

    Text {
      id: addressText
      text: "Server: " + TopOptions.webAddress
      color: "white"
      font.pointSize: 20
      anchors {
        top: parent.top
        left: parent.left
        right: parent.right
      }
      verticalAlignment: Text.AlignVCenter
      horizontalAlignment: Text.AlignHCenter
    }

    Text {
      id: dateText
      text: TaskListModel.date()
      color: "white"
      font.pointSize: 20
      anchors {
        top: addressText.bottom
        left: parent.left
        right: parent.right
        bottom: parent.bottom
      }
      topPadding: 10
      verticalAlignment: Text.AlignVCenter
      horizontalAlignment: Text.AlignHCenter

      Connections {
        target: TaskListModel
        function onDateChanged(date) {
          dateText.text = date
        }
      }
    }
  }
  Rectangle {
    id: boredSection

    height: 70

    anchors {
      top: topOptionsBar.bottom
      left: parent.left
      right: parent.right
    }

    color: "#1e1e1e"

    DeviceList {
      id: deviceList
      anchors {
        verticalCenter: parent.verticalCenter
        left: parent.left
        right: parent.right
      }
    }
  }

  Rectangle {
    id: mainSection

    anchors {
      top: boredSection.bottom
      bottom: parent.bottom
      left: parent.left
      right: parent.right
    }

    color: "#1e1e1e"

    TaskList {
      id: tasklist

      anchors {
        verticalCenter: parent.verticalCenter
        left: parent.left
        right: parent.right
      }
    }
  }

}