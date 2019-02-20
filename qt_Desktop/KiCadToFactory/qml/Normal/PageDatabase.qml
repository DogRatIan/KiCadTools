import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Controls.Material 2.3

Page {
    id: rootPage
    title: qsTr("Database")

    //==========================================================================
    // Properties
    //==========================================================================
    property string selectedImportFilename: ""
    property string selectedImportPath: ""

    //==========================================================================
    // Functions
    //==========================================================================
    function updateStatus () {
        if (itemPartList.isReady) {
            textDatabaseStatus.text = "Ready";
        }
        else {
            textDatabaseStatus.text = "Not connected";
        }
        textDatabaseFilename.text = itemPartList.filename;

        var file_size =  Number (itemPartList.fileSize);
        if (file_size < 1) {
            file_size *= 1024;
            textDatabaseFileSize.text = file_size.toFixed(2) + " KiB";
        }
        else {
            textDatabaseFileSize.text = file_size.toFixed(2) + " MiB";
        }
        textDatabaseRowCount.text = itemPartList.rowCount;
    }

    //==========================================================================
    // ToolBar
    //==========================================================================
    header: TitleBar {
        titleText: rootPage.title

        leftButtonVisible: true
        leftButtonIconSource: "qrc:/assets/back-arrow.svg"
        onLeftButtonClicked: navStack.pop ();
    }

    //==========================================================================
    // Main area
    //==========================================================================
    Item {
        anchors.fill: parent
        anchors.margins: 10

        Column {
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: 10

            LabeledText {
                id: textDatabaseStatus
                labelText: "DB status:"
            }
            LabeledText {
                id: textDatabaseFilename
                labelText: "DB filename:"
            }
            LabeledText {
                id: textDatabaseFileSize
                labelText: "DB file size:"
            }
            LabeledText {
                id: textDatabaseRowCount
                labelText: "Number of data:"
            }

            HorizontalLine {
            }

            // Import file
            Item {
                anchors.left: parent.left
                anchors.right: parent.right
                height: 50

                LabeledText {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    labelText: qsTr ("Import file:")
                    text: selectedImportFilename
                }

                Label {
                    anchors.left: parent.left
                    anchors.right: boxImportButtons.left
                    anchors.bottom: parent.bottom
                    text: selectedImportPath
                    font.pixelSize: 10
                    clip: true
                }

                Label {
                    anchors.left: parent.left
                    anchors.right: boxImportButtons.left
                    anchors.top: parent.top
                    text: "*Please use CSV file with TAB separator."
                    horizontalAlignment: Text.AlignRight
                    font.pixelSize: 10
                    font.italic: true
                }

                Row {
                    id: boxImportButtons
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    height: buttonSelectImportFile.height
                    spacing: 10

                    Button {
                        id: buttonSelectImportFile
                        text: qsTr ("Browse")
                        onClicked: {
                            dialogSelectImport.open ();
                        }
                    }
                    Button {
                        text: qsTr ("Import")
                        onClicked: {
                            var path = selectedImportPath + "/" + selectedImportFilename;
                            rootApp.importProgress = 0;
                            rootApp.showBusy (true);
                            if (itemPartList.importCsvFile (path)) {
                                rootApp.showSystemMessage ("INFO", "Import success.");
                            }
                            rootApp.showBusy (false);
                            rootPage.updateStatus();
                        }
                    }
                }
            }
            Item {
                anchors.left: parent.left
                anchors.right: parent.right
                height: 50

                ProgressBar {
                    id: progressImporting
                    anchors.right: parent.right
                    width: parent.width * 0.7
                    height: 20
                    value: rootApp.importProgress
                    from: 0
                    to: 100
                    background: Rectangle {
                        border.width: 1
                        radius: progressImporting.height / 2

                    }
                    contentItem: Item {
                        Rectangle {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            anchors.margins: 1
                            width: progressImporting.visualPosition * parent.width - 2
                            height: parent.height - 2
                            radius: progressImporting.height / 2
                            color: Material.primary
                        }
                    }
                }

                Label {
                    anchors.horizontalCenter: progressImporting.horizontalCenter
                    anchors.verticalCenter: progressImporting.verticalCenter
                    text: rootApp.importProgress.toFixed (2) + "%"
                }
            }

        }
    }

    //==========================================================================
    // Dialog
    //==========================================================================
    Loader {
        id: dialogSelectImport
        anchors.fill: parent
        focus: true

        function open () {
            if (dialogSelectImport.status == Loader.Ready) {
                dialogSelectImport.setupAndOpenDialog ();
            }
            else {
                dialogSelectImport.source = "DialogSelectFile.qml"
            }
        }

        function onFileSelected (aPath) {
            console.log ("Selected " + aPath);
            if (aPath.length > 0) {
                rootPage.selectedImportFilename = itemFileInfo.fileName (aPath);
                rootPage.selectedImportPath = itemFileInfo.path (aPath);
            }
        }

        function setupAndOpenDialog () {
            dialogSelectImport.item.folder = itemFileInfo.applicationDirPath ();

            dialogSelectImport.item.title = "Please select file for import.";
            dialogSelectImport.item.nameFilters = ["CSV files (*.csv)", "All files (*)"];
            dialogSelectImport.item.open ();
        }

        onLoaded: {
            dialogSelectImport.item.callbackFunc = dialogSelectImport.onFileSelected;
            dialogSelectImport.setupAndOpenDialog ();
        }
    }

    //==========================================================================
    // Creation/Destruction
    //==========================================================================
    Component.onCompleted: {
        appendMessageToLog (objectName + " created.");
        updateStatus ();
    }
    Component.onDestruction: {
    }
}
