import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Pdf

ApplicationWindow {
    id: root
    width: 800
    height: 1024
    color: "lightgrey"
    title: "PDFViewer"
    visible: true
    property string source

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            anchors.rightMargin: 6
            ToolButton {
                action: Action {
                    shortcut: StandardKey.ZoomIn
                    enabled: view.renderScale < 10
                    icon.source: "qrc:/pdfviewer/resources/zoom-in.svg"
                    onTriggered: view.renderScale *= Math.sqrt(2)
                }
            }
            ToolButton {
                action: Action {
                    shortcut: StandardKey.ZoomOut
                    enabled: view.renderScale > 0.1
                    icon.source: "qrc:/pdfviewer/resources/zoom-out.svg"
                    onTriggered: view.renderScale /= Math.sqrt(2)
                }
            }
            SpinBox {
                id: currentPageSB
                from: 1
                to: doc.pageCount
                editable: true
                onValueModified: view.goToPage(value - 1)
                Shortcut {
                    sequence: StandardKey.MoveToPreviousPage
                    onActivated: view.goToPage(currentPageSB.value - 2)
                }
                Shortcut {
                    sequence: StandardKey.MoveToNextPage
                    onActivated: view.goToPage(currentPageSB.value)
                }
            }
            Shortcut {
                sequence: StandardKey.Quit
                onActivated: Qt.quit()
            }
        }
    }
    Dialog {
        id: passwordDialog
        title: "Пароль"
        standardButtons: Dialog.Ok | Dialog.Cancel
        modal: true
        closePolicy: Popup.CloseOnEscape
        anchors.centerIn: parent
        width: 300

        contentItem: TextField {
            id: passwordField
            placeholderText: qsTr("Пожалуйста, введите пароль")
            echoMode: TextInput.Password
            width: parent.width
            onAccepted: passwordDialog.accept()
        }
        onOpened: passwordField.forceActiveFocus()
        onAccepted: doc.password = passwordField.text
    }

    Dialog {
        id: errorDialog
        title: "Ошибка загрузки " + doc.source
        standardButtons: Dialog.Close
        modal: true
        closePolicy: Popup.CloseOnEscape
        anchors.centerIn: parent
        width: 300
        visible: doc.status === PdfDocument.Error

        contentItem: Label {
            id: errorField
            text: doc.error
        }
    }

    PdfDocument {
        id: doc
        source: Qt.resolvedUrl(root.source)
        onPasswordRequired: passwordDialog.open()
    }

    PdfMultiPageView {
        id: view
        anchors.fill: parent
        document: doc
        onCurrentPageChanged: currentPageSB.value = view.currentPage + 1
    }

    DropArea {
        anchors.fill: parent
        keys: ["text/uri-list"]
        onEntered: (drag) => {
            drag.accepted = (drag.proposedAction === Qt.MoveAction || drag.proposedAction === Qt.CopyAction) &&
                drag.hasUrls && drag.urls[0].endsWith("pdf")
        }
        onDropped: (drop) => {
            doc.source = drop.urls[0]
            drop.acceptProposedAction()
        }
    }


}
