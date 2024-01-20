import QtQuick
import QtQuick.Controls
import com.tasktracker.TaskListModel
import com.tasktracker.TaskListModel

ApplicationWindow {
  id: root
  visible: true
  title: qsTr("Tasktracker")
  visibility: Window.FullScreen
  // Without setting these, the window isn't showing correct
  // even though one might think they aren't needed when FullScreen is set.
  width: Screen.width
  height: Screen.height

  Rectangle {
    id: topOptionsBar
    height: 50
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
        bottom: parent.bottom
      }
      verticalAlignment: Text.AlignVCenter
      horizontalAlignment: Text.AlignHCenter
    }

  }

  Rectangle {
    id: mainSection

    anchors {
      top: topOptionsBar.bottom
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