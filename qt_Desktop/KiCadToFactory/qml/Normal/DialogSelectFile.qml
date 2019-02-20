import QtQuick 2.11
import QtQuick.Dialogs 1.2

FileDialog {
    id: rootItem
    visible: false
    title: "DialogSelectFile"
    selectExisting: true
    selectFolder: false
    selectMultiple: false
    folder: shortcuts.home

    property var callbackFunc;

    function urlToPath (aUrlString) {
        var path = aUrlString.toString().replace (/^(file:\/{3})|(qrc:\/{2})|(http:\/{2})/,"");
        return decodeURIComponent(path);
    }

    function setDefaultFolder () {
        folder = shortcuts.home;
    }

    onAccepted: {
        console.log("You chose: " + rootItem.fileUrl)
        close ();
        if (typeof (callbackFunc) === "function") {
            callbackFunc (urlToPath (rootItem.fileUrl));
        }
    }
    onRejected: {
        console.log("Canceled")
        close ();
        if (typeof (callbackFunc) === "function") {
            callbackFunc ("");
        }
    }
}
