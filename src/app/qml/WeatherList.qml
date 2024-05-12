import QtQuick
import QtQuick.Controls
import com.tasktracker.WeatherListModel

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
    model: WeatherListModel
    spacing: 10

    delegate: Rectangle {
      id: delegate

      required property string temperature
      required property string date
      required property string rain
      required property string wind
      required property string clouds
      required property string timestamp

      width: listview.width / 10
      height: 200

      Text {
        id: dateText
        minimumPixelSize: 10
        color: "black"
        text: delegate.timestamp + " " + delegate.date + "\n" +
              "Temperature: " + delegate.temperature + "C\n" +
              "Rain: " + delegate.rain + "mm\n" +
              "Wind: " + delegate.wind + "m/s\n" +
              "Clouds: " + delegate.clouds + "%\n"


        anchors {
          horizontalCenter: parent.horizontalCenter
          top: parent.top
          leftMargin: 5
          topMargin: 2
        }

        font {
          pixelSize: 16
          bold: true
        }
      }
    }
  }
}