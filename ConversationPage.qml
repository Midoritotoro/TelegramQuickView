import QtQuick
import QtQuick.Layouts
import QtQuick.Controls


Page {
    id: root

    property string inConversationWith

    header: ToolBar {
        ToolButton {
            text: qsTr("Back")
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            onClicked: root.StackView.view.pop()
        }
    Label {
        id: pageTitle
        text: inConversationWith
        font.pixelSize: 20
        anchors.centerIn: parent
    }
}
    ColumnLayout {
        anchors.fill: parent

        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

    Pane {
        id: pane
        Layout.fillWidth: true

        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: pane.leftPadding + messageField.leftPadding
            displayMarginBeginning: 40
            displayMarginEnd: 40
            verticalLayoutDirection: ListView.BottomToTop
            spacing: 12
            model: 10
            delegate: Row {
                readonly property bool sentByMe: index % 2 == 0

                anchors.right: sentByMe ? listView.contentItem.right : undefined
                spacing: 6

                Rectangle {
                    id: avatar
                    width: height
                    height: parent.height
                    color: "grey"
                    visible: !sentByMe
                    onClicked: root.StackView.view.push("ConversationPage.qml", { inConversationWith: modelData })
                }

                Rectangle {
                    width: 80
                    height: 40
                    color: sentByMe ? "lightgrey" : "steelblue"
                    Label {
                        anchors.centerIn: parent
                        text: index
                        color: sentByMe ? "black" : "white"
                    }
                }
            }

            ScrollBar.vertical: ScrollBar {}
                }
            }
        }
    }