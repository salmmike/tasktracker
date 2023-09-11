import QtQuick
import QtQuick.Controls
import com.tasktracker.TaskListModel

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

    clip: true
    model: TaskListModel
    spacing: 10

    delegate: Rectangle {
      id: delegate

      required property string taskName
      required property string startTime

      width: listview.width
      height: 50

      color: "#1e1e1e"

      Text {
        id: startTimeText
        minimumPixelSize: 10
        color: "gray"
        text: delegate.startTime
        anchors {
          verticalCenter: parent.verticalCenter
        }

        font {
          pixelSize: 14
          bold: true
        }
      }

      Text {
        id: taskNameText
        minimumPixelSize: 6
        color: "white"
        text: delegate.taskName
        padding: 20
        anchors {
          left: startTimeText.right
          verticalCenter: startTimeText.verticalCenter
        }

        font {
          pixelSize: 16
        }
      }
      Button {
        id: doneButton
        text: "Done!"
        anchors {
          right: parent.right
          verticalCenter: parent.verticalCenter
        }
      }

    }
  }
}