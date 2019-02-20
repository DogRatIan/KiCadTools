import QtQuick 2.11
import QtQuick.Controls 2.4

import com.dogratian.qml.Config 1.0
import com.dogratian.qml.FileInfo 1.0
import com.dogratian.qml.PartList 1.0
import com.dogratian.qml.KiCadConverter 1.0


ApplicationWindow {
    id: rootApp
    width: 600
    height: 600
    minimumWidth: 600
    minimumHeight: 600
    visible: true
    flags: Qt.FramelessWindowHint | Qt.Dialog
    title: qsTr("DogRatIan KiCad Converter")
    font.family: "Verdana"

    //==========================================================================
    // Properties
    //==========================================================================
    property var viewMessageLog
    property string currentPath: ""
    property real importProgress: 0

    //==========================================================================
    // Functions
    //==========================================================================
    function appendMessageToLog (aMsg) {
        var msg = {message: aMsg};
        listMessageLog.append (msg);
        if (listMessageLog.count > 100)
            listMessageLog.remove (0);
        if ((viewMessageLog) && (typeof viewMessageLog.positionViewAtEnd == "function")) {
            viewMessageLog.positionViewAtEnd();
        }
    }

    function showBusy (aShow) {
        indicatorBusy.visible = aShow;
    }

    function startNormal () {
        viewMessageLog = undefined;
        mainLoader.source = "";
        mainLoader.source = "Normal/Top.qml"
        console.log ("Load " + mainLoader.source);
    }

    function startPath2 () {
        viewMessageLog = undefined;
        mainLoader.source = "";
        mainLoader.source = "Path2/Top.qml"
        console.log ("Load " + mainLoader.source);
    }

    function showSystemMessage (aTitle, aMessage) {
        showBusy (false);
        textSystemMessage.text = aMessage;
        dialogSystemMessage.title = aTitle;
        dialogSystemMessage.open();
    }

    //==========================================================================
    // Visual area
    //==========================================================================
    Rectangle {
        anchors.fill: parent
        border.width: 1
        border.color: "#888888"
    }

    Item {
        anchors.fill: parent
        anchors.margins: 1

        Loader {
            id: mainLoader
            anchors.fill: parent
            focus: true
        }
    }

    //==========================================================================
    // Busy indicator
    //==========================================================================
    Rectangle {
        id: indicatorBusy
        visible: false
        anchors.fill: parent
        color: "#80000000"

        BusyIndicator  {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
        }

        MouseArea {
            anchors.fill: parent
        }
    }

    //==========================================================================
    // Dialog - System Message
    //==========================================================================
    Dialog {
        id: dialogSystemMessage
        title: "System Message"
        standardButtons: Dialog.Ok
        modal: true
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        implicitWidth: parent.width / 2
        implicitHeight: parent.height / 3

        header: ToolBar {
            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                text: dialogSystemMessage.title
            }
        }

        Label {
            id: textSystemMessage
            anchors.fill: parent
            anchors.margins: 5
            text: "-"
            wrapMode: Text.WordWrap
        }
    }

    //==========================================================================
    // C++ Object
    //==========================================================================
    Config {
        id: itemConfig
        filename: "KiCadToJLC.conf"

        onMessage: {
            rootApp.appendMessageToLog (aMessage);
        }
        onErrorMessage: {
            rootApp.showSystemMessage (qsTr ("ERROR"), aMessage);
        }
    }

    FileInfo {
        id: itemFileInfo
    }

    PartList {
        id: itemPartList

        onMessage: {
            rootApp.appendMessageToLog (aMessage);
        }
        onErrorMessage: {
            rootApp.showSystemMessage (qsTr ("ERROR"), aMessage);
        }
        onImportProgressUpdated: {
            rootApp.importProgress = aProgress;
        }
    }

    KiCadConverter {
        id: itemConverter

        onMessage: {
            rootApp.appendMessageToLog (aMessage);
        }
        onErrorMessage: {
            rootApp.showSystemMessage (qsTr ("ERROR"), aMessage);
        }
    }


    //==========================================================================
    // Models
    //==========================================================================
    ListModel {
        id: listMessageLog
        ListElement {
            message: "Message log"
        }
    }

    //==========================================================================
    // Creation/Destruction
    //==========================================================================
    Component.onCompleted: {
        startNormal ();
    }

}
