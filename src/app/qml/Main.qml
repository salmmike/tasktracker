import QtQuick
import QtQuick.Controls
import com.tasktracker.TaskListModel
import com.tasktracker.TaskListModel

Window {
  id: root
  width: 640
  height: 480
  visible: true
  title: qsTr("Hello World")

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