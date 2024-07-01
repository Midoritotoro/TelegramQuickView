import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: window
    width: 540
    height: 960
    visible: true
    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: ConversationPage {}
    }
}
  