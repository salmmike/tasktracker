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
      bottom: nextDayButton.top
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
      required property bool skipped
      required property bool finished
      required property int index

      width: listview.width
      height: 70

      color: finished ? "green" : skipped ? "orange" : "#1e1e1e"

      MouseArea {

        id: taskItemMouseArea
        anchors.fill: parent
        onPressAndHold: {
          if (skipped | finished) {
            TaskListModel.setUndone(delegate.index)
          } else {
            contextMenu.popup()
          }
        }
        Menu {
          id: contextMenu
          MenuItem {
            text: "Remove"
            onClicked: TaskListModel.removeTask(delegate.index)
            font.pixelSize: 30
          }
          MenuItem {
            text: "Cancel"
            font.pixelSize: 30
          }
        }
      }

      Text {
        id: startTimeText
        minimumPixelSize: 10
        color: "gray"
        text: delegate.startTime
        anchors {
          verticalCenter: parent.verticalCenter
          left: parent.left
          leftMargin: 20
        }

        font {
          pixelSize: 16
          bold: true
        }
      }

      Text {
        id: taskNameText
        minimumPixelSize: 10
        color: "gray"
        text: delegate.taskName
        anchors {
          left: startTimeText.right
          verticalCenter: startTimeText.verticalCenter
          leftMargin: 20
        }

        font {
          pixelSize: 16
          bold: true
        }
      }

      Button {
        id: doneButton
        text: finished | skipped ? "Undo" : "Done!"
        padding: 15
        visible: !finished && !skipped

        anchors {
          right: parent.right
          verticalCenter: parent.verticalCenter
          rightMargin: 20
        }

        onClicked:  {
          TaskListModel.setFinished(delegate.index)
        }

        onPressAndHold: {
          TaskListModel.setSkipped(delegate.index)
        }
      }
    }
  }
  Button {
    id: nextDayButton
    text: 'Next day'
    padding: 15
    onClicked: {
      TaskListModel.nextDay()
    }

    anchors {
      bottom: parent.bottom
      right: parent.right
    }

    width: parent.width/4
    height: 70
  }
  Button {
    id: previousDayButton
    text: 'Prevous day'
    padding: 15
    onClicked: {
      TaskListModel.previousDay()
    }
    anchors {
      bottom: parent.bottom
      left: parent.left
    }
    width: parent.width/4
    height: 70
  }

  Button {
    id: todayButton
    text: 'Today'
    padding: 15
    onClicked: {
      TaskListModel.setToday()
    }
    anchors {
      bottom: parent.bottom
      horizontalCenter: parent.horizontalCenter
    }
    width: parent.width/4
    height: 70
  }
}