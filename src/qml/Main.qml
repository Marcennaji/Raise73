import QtQuick 6.5
import QtQuick.Controls 6.5

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    Text {
        text: "coucou"
        anchors.centerIn: parent
    }
}