import QtQuick
import QtQuick.Controls
import qml

Page {
    anchors.fill: parent
    header: ChatToolBar {
    Label {
        text: qsTr("Contacts")
        font.pixelSize: 20
        anchors.centerIn: parent
    }
}
ListView {
    id: listView
    anchors.fill: parent
    topMargin: 48
    leftMargin: 48
    bottomMargin: 48
    rightMargin: 48
    spacing: 20
    model: ["avatar", "avatar"]
    delegate: ItemDelegate {
        text: modelData
        width: ListView.view.width - ListView.view.leftMargin - ListView.view.rightMargin
        height: avatar.implicitHeight
        leftPadding: avatar.implicitWidth + 32
        Image {
            id: avatar
            source: "TelegramQuickView/assets/images/" + modelData + ".jpg"
        }
    }
}
}