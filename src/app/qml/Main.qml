import QtQuick
import QtQuick.Controls
import com.tasktracker.TaskListModel

Window {
  id: root
  width: 640
  height: 480
  visible: true
  title: qsTr("Hello World")

  Rectangle {
    id: topOptions
    height: 0

    anchors {
      top: parent.top
      left: parent.left
      right: parent.right
    }
  }

  Rectangle {
    id: mainSection

    anchors {
      top: topOptions.bottom
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