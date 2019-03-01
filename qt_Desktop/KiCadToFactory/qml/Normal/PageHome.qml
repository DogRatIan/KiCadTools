import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Controls.Material 2.3

import "../LocalLib" as LocalLib

Page {
    id: rootPage
    title: qsTr("page1")

    //==========================================================================
    // Properties
    //==========================================================================
    property var selectedPositionFile: {"path": "", "filename": ""}
    property var selectedBomFile: {"path": "", "filename": ""}
    property string selectedDestPath: ""
    property string selectedDestPathTrimmed: ""
    property bool selectedJlc: true

    //==========================================================================
    // Functions
    //==========================================================================
    function clearSelection () {
        rootPage.selectedPositionFile = {"path": "", "filename": ""};
        rootPage.selectedBomFile = {"path": "", "filename": ""};
        rootPage.selectedDestPath = "";
        rootPage.selectedDestPathTrimmed = "";
    }

    function updateDatabaseStatus () {
        if (itemPartList.isReady) {
            labelDatabaseStatus.text = "Ready (" + itemPartList.rowCount + " parts)";
        }
        else {
            labelDatabaseStatus.text = "Not connected";
        }
    }

    function updateDestPath (aPath) {
        rootPage.selectedDestPath = aPath;
        if (aPath.length > 40) {
            rootPage.selectedDestPathTrimmed = "..." + aPath.substring (aPath.length - 40, aPath.length);
        }
        else {
            rootPage.selectedDestPathTrimmed = aPath
        }
    }

    function doConversion () {
        if (!itemPartList.isReady) {
            rootApp.showSystemMessage ("ERROR", "Database not ready.");
            return;
        }

        if (rootPage.selectedJlc == false) {
            rootApp.showSystemMessage ("ERROR", "Please select at least one output type.");
            return;
        }

        var position_file_path = selectedPositionFile.path + "/" + selectedPositionFile.filename;
        var bom_file_path = selectedBomFile.path + "/" + selectedBomFile.filename;
        itemConverter.outputPath = selectedDestPath;
        if (selectedDestPath.length == 0) {
            rootApp.showSystemMessage ("ERROR", "Please select the output path.");
            return;
        }
        if ((selectedPositionFile.filename.length === 0) && (selectedBomFile.filename.length === 0)) {
            rootApp.showSystemMessage ("ERROR", "Please select a position file\nor a BOM file or both.");
            return;
        }

        rootApp.showBusy (true);
        if (selectedPositionFile.filename.length > 0) {
            if (!itemConverter.convertPositionFile (position_file_path)) {
                rootApp.showBusy (false);
                return;
            }
        }
        if (selectedBomFile.filename.length > 0) {
            if (!itemConverter.convertBomFile (bom_file_path)) {
                rootApp.showBusy (false);
                return;
            }
        }
        rootApp.showBusy (false);
        rootApp.showSystemMessage ("INFO", "Conversion success.");
    }

    //==========================================================================
    // ToolBar
    //==========================================================================
    header: TitleBar {
        titleText: rootApp.title

        leftLogoVisible: true
        leftLogoIconSource: "qrc:/assets/app_icon.png"
        onLeftButtonClicked: itemDrawer.open();

        rightButtonVisible: true
        rightButtonEnabled: !rootPage.portOpened
        rightButtonIconSource: "qrc:/assets/settings-gears.svg"
        onRightButtonClicked: {
        }
    }

    Drawer {
        id: itemDrawer
        width: rootPage.width * 0.20
        height: rootPage.height

        background: Rectangle {
            anchors.fill: parent
            border.width: 1
            border.color: "black"
        }

        Column {
            anchors.fill: parent

            Label {
                text: qsTr ("MENU")
                font.bold: true
                width: parent.width
                height: 30
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            HorizontalLine {
            }

            ItemDelegate {
                text: qsTr("Exit")
                width: parent.width
                onClicked: {
                    Qt.quit();
                }
            }
        }
    }

    //==========================================================================
    // Main area
    //==========================================================================
    Item {
        anchors.fill: parent
        anchors.margins: 10

        Column {
            id: boxInfo
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: 10

            // Footprint position file
            Item {
                anchors.left: parent.left
                anchors.right: parent.right
                height: 50

                LabeledText {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    labelText: qsTr ("Position file:")
                    text: selectedPositionFile.filename
                }

                Label {
                    anchors.left: parent.left
                    anchors.right: buttonSelectPositionFile.left
                    anchors.bottom: parent.bottom
                    text: selectedPositionFile.path
                    font.pixelSize: 10
                    clip: true
                }

                Button {
                    id: buttonSelectPositionFile
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr ("Select")
                    onClicked: {
                        dialogSelectSource.title = qsTr ("Please select position file.");
                        dialogSelectSource.folder = rootApp.currentPath;
                        dialogSelectSource.nameFilters = ["CSV files (*.csv)", "All files (*)"];
                        dialogSelectSource.open (rootPage, "selectedPositionFile");
                    }
                }
            }

            HorizontalLine {
            }

            // BOM file
            Item {
                anchors.left: parent.left
                anchors.right: parent.right
                height: 50

                LabeledText {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    labelText: qsTr ("BOM file:")
                    text: selectedBomFile.filename
                }

                Label {
                    anchors.left: parent.left
                    anchors.right: buttonSelectBomFile.left
                    anchors.bottom: parent.bottom
                    text: selectedBomFile.path
                    font.pixelSize: 10
                    clip: true
                }

                Button {
                    id: buttonSelectBomFile
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr ("Select")
                    onClicked: {
                        dialogSelectSource.title = qsTr ("Please select BOM file.");
                        dialogSelectSource.folder = rootApp.currentPath;
                        dialogSelectSource.nameFilters = ["CSV files (*.csv)", "All files (*)"];
                        dialogSelectSource.open (rootPage, "selectedBomFile");
                    }
                }
            }

            HorizontalLine {
            }

            // Output PATH
            Item {
                anchors.left: parent.left
                anchors.right: parent.right
                height: 50

                LabeledText {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    labelText: qsTr ("Output path:")
                    text: selectedDestPathTrimmed
                }

                Label {
                    anchors.left: parent.left
                    anchors.right: buttonSelectDest.left
                    anchors.bottom: parent.bottom
                    text: selectedDestPath
                    font.pixelSize: 10
                    clip: true
                }

                Button {
                    id: buttonSelectDest
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr ("Select")
                    onClicked: {
                        dialogSelectDestination.open ();
                    }
                }
            }

            HorizontalLine {
            }

            // Output types
            Item {
                anchors.left: parent.left
                anchors.right: parent.right
                height: 50
                Row {
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom

                    LabeledText {
                        anchors.verticalCenter: parent.verticalCenter
                        labelText: qsTr ("Output type:")
                        text: " "
                    }
                    CheckBox {
                        anchors.verticalCenter: parent.verticalCenter
                        text: "JLC"
                        checked: rootPage.selectedJlc
                        onClicked: {
                            rootPage.selectedJlc = checked;
                        }
                    }
                }
            }

            HorizontalLine {
            }

            // Buttons
            Item {
                anchors.left: parent.left
                anchors.right: parent.right
                height: 50

                Row {
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    spacing: 10

                    Button {
                        text: qsTr ("Clear selection")
                        onClicked: {
                            rootPage.clearSelection();
                        }
                    }

                    Button {
                        text: qsTr ("Convert")
                        onClicked: {
                            rootPage.doConversion();
                        }
                    }

                }
            }


            HorizontalLine {
            }

            // Database
            Item {
                anchors.left: parent.left
                anchors.right: parent.right
                height: 50

                LabeledText {
                    id: labelDatabaseStatus
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    labelText: qsTr ("Parts datebase:")
                    text: "?"
                }
                Button {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr ("Manage")
                    onClicked: {
                        navStack.push (pageDatabase);
                    }
                }
            }
        }
    }

    //==========================================================================
    // Dialog
    //==========================================================================
    Loader {
        id: dialogSelectSource
        anchors.fill: parent
        focus: true

        property var updateObject
        property string updateProperty
        property var nameFilters: ["All files (*)"]
        property string folder: ""
        property string title: "Please select file"

        function open (aUpdateObject, aUpdateProperty) {
            updateObject = aUpdateObject;
            updateProperty = aUpdateProperty;

            if (dialogSelectSource.status == Loader.Ready) {
                dialogSelectSource.setupAndOpenDialog ();
            }
            else {
                dialogSelectSource.source = "DialogSelectFile.qml"
            }
        }

        function onFileSelected (aPath) {
            console.log ("Selected " + aPath);
            if (aPath.length > 0) {
                if ((updateObject) && (updateProperty)) {
                    if (updateObject.hasOwnProperty (updateProperty)) {
                        updateObject[updateProperty] = {"path": itemFileInfo.path (aPath),
                            "filename": itemFileInfo.fileName (aPath)};
                    }
                }
                if (rootPage.selectedDestPath.length == 0) {
                    updateDestPath (itemFileInfo.path (aPath));
                }

                rootApp.currentPath = itemFileInfo.path (aPath);
            }
        }

        function setupAndOpenDialog () {
            if (folder.length > 0) {
                dialogSelectSource.item.folder = "file:///" + folder;
            }
            else {
                dialogSelectSource.item.setDefaultFolder ();
            }            
            dialogSelectSource.item.title = dialogSelectSource.title;
            dialogSelectSource.item.nameFilters = dialogSelectSource.nameFilters;
            dialogSelectSource.item.open ();
        }

        onLoaded: {
            dialogSelectSource.item.callbackFunc = dialogSelectSource.onFileSelected;
            dialogSelectSource.setupAndOpenDialog ();
        }
    }

    Loader {
        id: dialogSelectDestination
        anchors.fill: parent
        focus: true

        function open () {
            if (dialogSelectDestination.status == Loader.Ready) {
                dialogSelectDestination.setupAndOpenDialog ();
            }
            else {
                dialogSelectDestination.source = "DialogSelectFile.qml"
            }
        }

        function onFileSelected (aPath) {
            console.log ("Selected " + aPath);
            if (aPath.length > 0) {
                rootApp.currentPath = aPath;
                updateDestPath (aPath);
            }
        }

        function setupAndOpenDialog () {
            dialogSelectDestination.item.selectFolder = true;
            if (rootPage.selectedDestPath.length > 0) {
                dialogSelectDestination.item.folder = "file:///" + rootPage.selectedDestPath;
            }
            else if (rootApp.currentPath.length > 0) {
                dialogSelectDestination.item.folder = "file:///" + rootApp.currentPath;
            }
            else {
                dialogSelectDestination.item.setDefaultFolder ();
            }
            dialogSelectDestination.item.title = "Please select the destination";
            dialogSelectDestination.item.nameFilters = ["All files (*)"];
            dialogSelectDestination.item.open ();
        }

        onLoaded: {
            dialogSelectDestination.item.callbackFunc = dialogSelectDestination.onFileSelected;
            dialogSelectDestination.setupAndOpenDialog ();
        }
    }

    //==========================================================================
    // Creation/Destruction
    //==========================================================================
    Component.onCompleted: {
        rootApp.appendMessageToLog (objectName + " created. ");

        itemDelayFunc.forceSet(1000, rootPage.updateDatabaseStatus);
        rootPage.clearSelection();

    }

    Component.onDestruction: {
    }

    //==========================================================================
    // Library
    //==========================================================================
    LocalLib.DelayFunc {
        id: itemDelayFunc
    }

}
