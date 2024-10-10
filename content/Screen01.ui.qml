

/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/
import QtQuick 6.5
import QtQuick.Controls 6.5
import Raise73

Rectangle {
    width: Constants.width
    height: Constants.height
    color: Constants.backgroundColor

    Row {
        id: row
        anchors.centerIn: parent
        width: parent.width
        height: parent.height 

        Rectangle {
            width: parent.width * 0.30
            height: parent.height

             Column {
                anchors.centerIn: parent
                spacing: 10 // Adjust the spacing as needed

                Text {
                    text: qsTr("Select your game type using the tumblers")
                    font.family: "Verdana"
                    font.pixelSize: 16 // Adjust the size as needed
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                }

                Text {
                    text: qsTr("Swipe up or down to scroll through the options.")
                    font.family: "Verdana"
                    font.pixelSize: 14 // Adjust the size as needed
                    color: "gray"
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                }
            }
        }

        Rectangle {
            width: parent.width * 0.20 // Half of the Row's width
            height: parent.height
 
            Tumbler {
                id: tumbler_chooseSeatsNumber
                anchors.centerIn: parent
                width: parent.width * 0.8// Adjust as needed
                currentIndex: 3
                wrap: true
                model: ["6-Max cash game", "Full-Ring cash game", "Heads-up Sit&Go", "6 players Sit&Go", "9 players Sit&Go"]    
            }                 
        }

        Rectangle {
            width: parent.width * 0.20 // Half of the Row's width
            height: parent.height
 
            Tumbler {
                id: tumbler_chooseOpponentsProfile
                anchors.centerIn: parent
                width: parent.width * 0.8 // Adjust as needed
                currentIndex: 2
                wrap: true
                model: ["Mostly ultra-tight opponents", "Mostly tight-aggressive opponents", "Mostly loose-aggressive opponents", "Mostly loose-maniac opponents", "Randomly chosen opponents"]
            }
        }
        Rectangle {
            width: parent.width * 0.30
            height: parent.height
            Button {
                text: qsTr("Start new game")
                anchors.centerIn: parent
                onClicked: {
                    swipeView.currentIndex = 1
                    cppInterface.startNewGame()
                }
            }
        }
    }
}
