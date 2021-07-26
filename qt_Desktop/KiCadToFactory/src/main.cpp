#include <QApplication>
#include <QQmlApplicationEngine>

#include "qml_config.h"
#include "qml_fileinfo.h"
#include "qml_part_list.h"
#include "qml_converter.h"
#include "qml_sysutil.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    qmlRegisterType <CConfig>("com.dogratian.qml.Config", 1, 0, "Config");
    qmlRegisterType <CSysUtil>("com.dogratian.qml.SysUtil", 1, 0, "SysUtil");
    qmlRegisterType <CFileInfo>("com.dogratian.qml.FileInfo", 1, 0, "FileInfo");
    qmlRegisterType <CPartList>("com.dogratian.qml.PartList", 1, 0, "PartList");
    qmlRegisterType <CKiCadConverter>("com.dogratian.qml.KiCadConverter", 1, 0, "KiCadConverter");


    QApplication::setOrganizationName("DogRatIan");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/qml/Main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
