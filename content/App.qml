import QtQuick 6.5
import QtQuick.Controls 6.5
import Raise73

Window {
    width: Constants.width
    height: Constants.height

    visible: true

    SwipeView {
        id: swipeView
        anchors.top: tabBar.bottom
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        currentIndex: tabBar.currentIndex

        Screen01 {
        }

        Screen02 {
        }
    }

    TabBar {
        anchors.left: parent.left
        anchors.right: parent.right

        id: tabBar
        currentIndex: swipeView.currentIndex

        TabButton {
            text: qsTr("Game settings")
            onClicked: {
                swipeView.currentIndex = 0
            }
        }
        TabButton {
            text: qsTr("Play")
            onClicked: {
                swipeView.currentIndex = 1
            }
        }
    }
}
