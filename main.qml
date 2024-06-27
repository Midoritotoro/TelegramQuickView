import QtQuick
import QtQuick.Controls

ApplicationWindow {
    width: 540
    height: 960
    visible: true
    Page {
        anchors.fill: parent
        header: Label {
            padding: 10
            text: qsTr("Contacts")
            font.pixelSize: 20
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
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
            width: listView.width - listView.leftMargin - listView.rightMargin
            height: avatar.implicitHeight
            leftPadding: avatar.implicitWidth + 32

            Image {
                id: avatar
                source: "TelegramQuickView/assets/images/" + modelData + ".jpg"
            }
        }
        }
    }
}
  