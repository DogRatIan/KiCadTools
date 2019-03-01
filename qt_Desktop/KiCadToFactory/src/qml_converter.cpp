//==========================================================================
//
//==========================================================================
//  Copyright (c) DogRatIan.  All rights reserved.
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//==========================================================================
// Naming conventions
// ~~~~~~~~~~~~~~~~~~
//                Class : Leading C
//               Struct : Leading T
//             Constant : Leading K
//      Global Variable : Leading g
//    Function argument : Leading a
//       Local Variable : All lower case
//==========================================================================
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>

#include "debug.h"
#include "qml_converter.h"

//==========================================================================
// Defines
//==========================================================================
#define CHAR_POSITION_FILE_SEPARATOR        ','
#define CHAR_BOM_FILE_SEPARATOR             ';'
#define STRING_LINE_FEED                    "\n"

#define FILENAME_CONVERSION_DB              "conversion.sqlite3"
#define FILENAME_PART_LIST_DB               "partlist.sqlite3"

struct TPartInfo {
    QString ref;
    QString value;
    QString package;
    QString type;
    QString partNumber;
    QString name;
    int padCount;
    bool foundInDb;
    bool isStandardStock;
    unsigned char padding[2];
};


//==========================================================================
// Constants
//==========================================================================


//==========================================================================
//==========================================================================
// Public members
// vvvvvvvvvvvvvv
//==========================================================================
//==========================================================================

//==========================================================================
// Constructor
//==========================================================================
CKiCadConverter::CKiCadConverter (QObject *aParent) :
    QObject(aParent) {
    currentPositionFilename = "JLC_PCB_Factory_SMT.csv";
    currentBomFilename = "JLC_PCB_Factory_BOM.csv";
}

//==========================================================================
// Destroyer
//==========================================================================
CKiCadConverter::~CKiCadConverter (void) {
    closeDb ();
}

//==========================================================================
// Convert footprint position file
//==========================================================================
bool CKiCadConverter::convertPositionFile (QString aFilePath) {
    bool ret = false;
    DEBUG_PRINTF ("CKiCadConverter::convertPositionFile: %s", aFilePath.toUtf8().data());
    QFileInfo input_info (aFilePath);
    QFile input_file (aFilePath);
    QString output_file_path = currentOutputPath + QDir::separator() + currentPositionFilename;
    QFileInfo output_info (output_file_path);
    QFile output_file (output_file_path);

    if (!input_file.exists()) {
        emit errorMessage (QString ("%1 not found.").arg (input_info.fileName()));
        return ret;
    }

    if (checkOutputPath() < 0) {
        return ret;
    }

    do {
        if (openDb() < 0) {
            break;;
        }

        if (!input_file.open(QIODevice::ReadOnly)) {
            emit errorMessage (QString ("%1\nUnable to open file.").arg(input_info.fileName()));
            break;
        }

        if (!output_file.open (QIODevice::ReadWrite | QIODevice::Truncate)) {
            emit errorMessage (QString ("%1\nUnable to open file.").arg(output_info.fileName()));
            break;
        }

        // Check file content
        auto line_count = checkFile (&input_file, CHAR_POSITION_FILE_SEPARATOR, 7);
        if (line_count < 0) {
            break;
        }
        emit message (QString ("Position file, %1 lines.").arg (line_count));
        DEBUG_PRINTF ("line_count=%d", line_count);

        // Conversion for JLC factory
        if (convPositionForJlc (&input_file, line_count, &output_file) < 0) {
            break;
        }

        // All done
        ret = true;
        emit message ("Position file converted.");
    } while (0);
    input_file.close ();
    output_file.close ();
    closeDb ();

    return ret;
}

//==========================================================================
// Convert BOM file
//==========================================================================
bool CKiCadConverter::convertBomFile (QString aFilePath) {
    bool ret = false;
    DEBUG_PRINTF ("CKiCadConverter::convertBomFile: %s", aFilePath.toUtf8().data());
    QFileInfo input_info (aFilePath);
    QFile input_file (aFilePath);
    QString output_file_path = currentOutputPath + QDir::separator() + currentBomFilename;
    QFileInfo output_info (output_file_path);
    QFile output_file (output_file_path);

    if (!input_file.exists()) {
        emit errorMessage (QString ("%1 not found.").arg (input_info.fileName()));
        return ret;
    }

    if (checkOutputPath() < 0) {
        return ret;
    }

    do {
        if (openDb() < 0) {
            break;;
        }

        if (!input_file.open(QIODevice::ReadOnly)) {
            emit errorMessage (QString ("%1\nUnable to open file.").arg(input_info.fileName()));
            break;
        }

        if (!output_file.open (QIODevice::ReadWrite | QIODevice::Truncate)) {
            emit errorMessage (QString ("%1\nUnable to open file.").arg(output_info.fileName()));
            break;
        }

        // Check file content
        auto line_count = checkFile (&input_file, CHAR_BOM_FILE_SEPARATOR, 8, true);
        if (line_count < 0) {
            break;
        }
        emit message (QString ("BOM file, %1 lines.").arg (line_count));
        DEBUG_PRINTF ("line_count=%d", line_count);

        // Conversion for JLC factory
        if (convBomForJlc (&input_file, line_count, &output_file) < 0) {
            break;
        }

        // All done
        ret = true;
        emit message ("BOM file converted.");
    } while (0);
    input_file.close ();
    closeDb ();

    return ret;
}


//==========================================================================
// Read/Write properties
//==========================================================================
QString CKiCadConverter::readOutputPath (void) {
    return currentOutputPath;
}

void CKiCadConverter::writeOutputPath (QString aValue) {
    currentOutputPath = aValue;
}

QString CKiCadConverter::readPositionFilename (void) {
    return currentPositionFilename;
}

void CKiCadConverter::writePositionFilename (QString aValue) {
    currentPositionFilename = aValue;
}

QString CKiCadConverter::readBomFilename (void) {
    return currentBomFilename;
}

void CKiCadConverter::writeBomFilename (QString aValue) {
    currentBomFilename = aValue;
}


//==========================================================================
//==========================================================================
// Public members
// ^^^^^^^^^^^^^^^
//==========================================================================
//==========================================================================
// Private members
// vvvvvvvvvvvvvv
//==========================================================================
//==========================================================================

//==========================================================================
// Open database
//==========================================================================
int CKiCadConverter::openDb (void) {
    int ret = -1;
    QString path_conversion = QCoreApplication::applicationDirPath();
    path_conversion.append (QDir::separator());
    path_conversion.append (FILENAME_CONVERSION_DB);
    QFileInfo info_conversion (path_conversion);
    QString path_part_list = QCoreApplication::applicationDirPath();
    path_part_list.append (QDir::separator());
    path_part_list.append (FILENAME_PART_LIST_DB);
    QFileInfo info_part_list (path_conversion);

    //
    if (!info_conversion.exists()) {
        emit errorMessage ("Conversion DB file not found.");
        return ret;
    }

    if (!info_part_list.exists()) {
        emit errorMessage ("Part list DB file not found.");
        return ret;
    }


    try {
        QString sql;
        int conn_ret;

        // Connect DBs
        conn_ret = dbConversion.connect (path_conversion.toUtf8().data(), SQLITE_OPEN_READONLY, nullptr);
        if (conn_ret != SQLITE_OK) {
            emit errorMessage ("Can't connect database.");
            return ret;
        }
        conn_ret = dbPartList.connect (path_part_list.toUtf8().data(), SQLITE_OPEN_READONLY, nullptr);
        if (conn_ret != SQLITE_OK) {
            emit errorMessage ("Can't connect database.");
            return ret;
        }

        // Setup DB
        dbConversion.set_busy_timeout (2000);
        dbPartList.set_busy_timeout (2000);

        if (dbConversion.execute ("PRAGMA encoding='UTF-8';") != SQLITE_OK)
            throw dbConversion.error_msg();
        if (dbPartList.execute ("PRAGMA encoding='UTF-8';") != SQLITE_OK)
            throw dbPartList.error_msg();

        // Check tables
        if (dbConversion.execute ("SELECT * FROM packageJlc LIMIT 1;") != SQLITE_OK)
            throw dbConversion.error_msg();

        if (dbPartList.execute ("SELECT * FROM partList LIMIT 1;") != SQLITE_OK)
            throw dbPartList.error_msg();


        //
        ret = 0;
    }
    catch (std::exception& aError) {
        emit errorMessage (QString ("CKiCadConverter DB error.\n%1").arg (aError.what()));
    }
    catch (const char *aError) {
        emit errorMessage (QString ("CKiCadConverter DB error.\n%1").arg (aError));
    }

    return ret;
}

//==========================================================================
// Close database
//==========================================================================
void CKiCadConverter::closeDb (void) {
    dbConversion.disconnect();
    dbPartList.disconnect();
}


//==========================================================================
// Check output path
//==========================================================================
int CKiCadConverter::checkOutputPath (void) {
    QFileInfo info (currentOutputPath);

    if (!info.exists()) {
        emit errorMessage ("Converter output path not found.");
        return -1;
    }

    if (!info.isDir()) {
        emit errorMessage ("Converter output path not a directory.");
        return -1;
    }

    return 0;
}

//==========================================================================
// Check file. Return line count if success, -1 if fail
//==========================================================================
int CKiCadConverter::checkFile (QFile *aInputFile, char aSeparator, int aFieldCount, bool skipFirstLine) {
    QFileInfo info (aInputFile->fileName());
    int line_count = 0;
    char line_buf[256];

    if (!aInputFile->isOpen()) {
        emit errorMessage ("File not opened.");
        return -1;
    }

    aInputFile->seek(0);
    while (aInputFile->readLine (line_buf, sizeof (line_buf)) >= 0) {
        line_count ++;

        if ((skipFirstLine) && (line_count == 1)) {
            continue;
        }

        QString str_line = QString::fromUtf8(line_buf);
        auto list = str_line.split(aSeparator);

        if (list.count() != aFieldCount) {
            emit errorMessage (QString ("%1\nLine %2 invalid. Expected %3 fields, found %4 fields.")
                               .arg(info.fileName()).arg (line_count).arg(aFieldCount).arg(list.count()));
            return -1;
        }
    }

    return line_count;
}

//==========================================================================
// Position file Conversion for JLC factory
//==========================================================================
int CKiCadConverter::convPositionForJlc (QFile *aInputFile, int aInputLineCount, QFile *aOutputFile) {
    int ret = 0;
    int line_count = 0;
    char line_buf[256];
    QString str_output;

    if ((!aInputFile->isOpen()) || (!aOutputFile->isOpen())) {
        emit errorMessage ("File not opened.");
        return -1;
    }

    // Writer header
    str_output = "Designator,Footprint,Mid X,Mid Y,Ref X,Ref Y,Pad X,Pad Y,Layer,Rotation,Comment";
    str_output.append (STRING_LINE_FEED);
    str_output.append (STRING_LINE_FEED);
    aOutputFile->write(str_output.toUtf8());


    aInputFile->seek(0);
    while (aInputFile->readLine (line_buf, sizeof (line_buf)) >= 0) {
        //
        if ((line_count % 4) == 0) {
            QCoreApplication::processEvents();
        }
        line_count ++;

        // Skip first line
        if (line_count == 1) {
            continue;
        }

        // Extract line
        auto str_line = QString::fromUtf8(line_buf);
        auto list = str_line.split (CHAR_POSITION_FILE_SEPARATOR);

        auto str_ref = list.at(0).trimmed();
        auto str_value = list.at(1).trimmed();
        auto str_package = list.at(2).trimmed();
        auto pos_x = list.at(3).toDouble();
        auto pos_y = list.at(4).toDouble();
        auto rotation = list.at(5).toDouble();
        auto str_side = list.at(6).trimmed();

        // Convert fields
        str_ref.replace ("\"", "");
        str_value.replace ("\"", "");
        str_package.replace ("\"", "");

        if (str_side.compare("top", Qt::CaseInsensitive) == 0) {
            str_side = "T";
        }
        else if (str_side.compare("bottom", Qt::CaseInsensitive) == 0) {
            str_side = "B";
        }

        auto str_type = determinePartType (str_ref, str_value, str_package);
        auto str_comment = QString ("%1 %2").arg (str_type).arg (str_value.mid (0, 10));

        str_package = determinePackage (str_package);

        // Correct rotation for some type of parts
        if (str_type.compare ("TCAP") == 0) {
            rotation += 180;
        }
        if ((str_type.compare ("TRX") == 0) ||  (str_type.compare ("MOSFET") == 0)) {
            if (str_side.compare("T") == 0) {
                rotation += 270;
            }
            if (str_side.compare("B") == 0) {
                rotation += 90;
            }
        }


        if (str_type.compare ("RESxN") == 0) {
            rotation += 270;
        }

        if (rotation >= 360)
            rotation -= 360;

        // Write output
        str_output = QString ("%1,%2,%3mm,%4mm,,,,,%5,%6,%7")
                        .arg (str_ref).arg(str_package).arg(pos_x).arg(pos_y).arg(str_side).arg(rotation).arg(str_comment);
        str_output.append (STRING_LINE_FEED);
        aOutputFile->write(str_output.toUtf8());
    }

    //
    return ret;
}

//==========================================================================
// BOM file Conversion for JLC factory
//==========================================================================
int CKiCadConverter::convBomForJlc (QFile *aInputFile, int aInputLineCount, QFile *aOutputFile) {
    int ret = 0;
    int line_count = 0;
    char line_buf[256];
    QString str_output;
    struct TPartInfo part_info;

    if ((!aInputFile->isOpen()) || (!aOutputFile->isOpen())) {
        emit errorMessage ("File not opened.");
        return -1;
    }

    // Write header
    str_output = "Comment,Description,Designator,Footprint,LibRef,Pins,Quantity,编号";
    str_output.append (STRING_LINE_FEED);
    aOutputFile->write(str_output.toUtf8());

    aInputFile->seek(0);
    while (aInputFile->readLine (line_buf, sizeof (line_buf)) >= 0) {

        //
        if ((line_count % 4) == 0) {
            QCoreApplication::processEvents();
        }
        line_count ++;

        // Skip first line
        if (line_count == 1) {
            continue;
        }

        // Extract line
        QString str_line = QString::fromUtf8(line_buf);
        auto list = str_line.split (CHAR_BOM_FILE_SEPARATOR);

        auto str_ref = list.at(1).trimmed();
        auto str_package = list.at(2).trimmed();
        auto quantity = list.at(3).toInt();
        auto str_value = list.at(4).trimmed();
        QString str_part_number = "";
        QString str_pad_count = "";

        // Convert fields
        str_ref.replace ("\"", "");
        str_value.replace ("\"", "");
        str_package.replace ("\"", "");

        auto str_comment = str_value.mid (0, 60);
        auto str_type = determinePartType (str_ref, str_value, str_package);
        str_package = determinePackage (str_package);
        auto str_lib_ref = determinePartType (str_ref, str_value, str_package);
        str_lib_ref.append (" ");
        str_lib_ref.append (str_value.mid (0, 10));


        // Skip parts
        if ((str_ref.startsWith ("H")) || (str_ref.startsWith("TP")) || (str_ref.startsWith("G")))
            continue;

        // Try lookup from database
        part_info.ref = str_ref;
        part_info.package = str_package;
        part_info.value = str_value;
        part_info.type = str_type;
        part_info.foundInDb = false;
        part_info.isStandardStock = false;

        lookupFormPartList (&part_info);

        if (part_info.foundInDb) {
            str_comment = part_info.name;
            str_type = part_info.type;
            str_package = part_info.package;
            str_part_number = part_info.partNumber;
            str_pad_count.sprintf ("%d", part_info.padCount);
        }

        // Write output
        str_output = QString ("%1,%2,\"%3\",\"%4\",%5,%6,%7,%8")
                        .arg (str_comment).arg(str_type).arg(str_ref).arg(str_package)
                        .arg(str_lib_ref).arg(str_pad_count).arg(quantity).arg(str_part_number);
        str_output.append (STRING_LINE_FEED);
        aOutputFile->write(str_output.toUtf8());
    }

    //
    return ret;
}

//==========================================================================
// Determine part type
//==========================================================================
QString CKiCadConverter::determinePartType (QString aRef, QString aValue, QString aPackage) {

    if (aRef.startsWith ("RN")) {
        return "RESxN";
    }

    if (aRef.startsWith ("C")) {
        if (aPackage.startsWith("CP_EIA-"))
            return "TCAP";
        else
            return "CAP";
    }
    if (aRef.startsWith ("R")) {
        return "RES";
    }
    if (aRef.startsWith ("L")) {
        return "IND";
    }
    if (aRef.startsWith ("J")) {
        return "CON";
    }
    if (aRef.startsWith ("Q")) {
        if (aValue.startsWith ("IRLM"))
            return "MOSFET";

        return "TRX";
    }

    if (aRef.startsWith ("D")) {
        if (aPackage.contains("LED"))
            return "LED";
        else
            return "DIODE";
    }

    if ((aValue.contains("MHz")) || (aValue.contains("kHz"))) {
        return "XTAL";
    }

    if ((aPackage.contains("QFN")) || (aPackage.contains("SOP"))) {
        return "IC";
    }

    return aRef.mid(0, 1);
}

//==========================================================================
// Determine package
//==========================================================================
QString CKiCadConverter::determinePackage (QString aPackage) {
    if (aPackage.mid (1, 5).compare ("_0603") == 0) {
        return "0603";
    }
    if (aPackage.mid (1, 5).compare ("_0805") == 0) {
        return "0805";
    }
    if (aPackage.mid (1, 5).compare ("_1206") == 0) {
        return "1206";
    }

    try {
        QString sql;

        sql = "SELECT compareValue, convertTo, compareType FROM packageJlc;";
        sqlite3pp::query qry (dbConversion, sql.toUtf8().data());
        for (auto row = qry.begin(); row != qry.end(); ++row) {
            auto compare_value = (*row).get<char const*>(0);
            auto convert_to = (*row).get<char const*>(1);
            auto compare_type = (*row).get<char const*>(2);

            if ((strlen(compare_value) == 0) || (strlen (convert_to) == 0) || (strlen (compare_type) == 0)) {
                continue;
            }

            switch (compare_type[0]) {
            case 'S':
                if (aPackage.startsWith (compare_value)) {
                    return convert_to;
                }
                break;
            case 'C':
                if (aPackage.contains (compare_value)) {
                    return convert_to;
                }
                break;
            }
        }
    }
    catch (std::exception& aError) {
        emit message (QString ("Determine package error. %1").arg (aError.what()));
    }
    catch (const char *aError) {
        emit message (QString ("Determine package error. %1").arg (aError));
    }

    return aPackage.mid (0, 20);
}

//==========================================================================
// Lookup parts from Database
//==========================================================================
int CKiCadConverter::lookupFormPartList (struct TPartInfo *aPartInfo) {

    try {
        if (aPartInfo->type.compare ("CAP") == 0) {
            lookupCapacitor (aPartInfo, "贴片电容");
        }
        else if (aPartInfo->type.compare ("TCAP") == 0) {
            lookupCapacitor (aPartInfo, "钽电容");
        }
        else if (aPartInfo->type.compare ("RES") == 0) {
            lookupResister (aPartInfo, "贴片电阻");
        }
        else if (aPartInfo->type.compare ("IND") == 0) {
            lookupInductor (aPartInfo, "贴片电阻");
        }
        else if (aPartInfo->type.compare ("XTAL") == 0) {
            lookupGeneral (aPartInfo, "贴片无源晶振");
        }
        else if (aPartInfo->type.compare ("TRX") == 0) {
            lookupGeneral (aPartInfo, "三极管");
        }
        else if (aPartInfo->type.compare ("LED") == 0) {
            lookupLed (aPartInfo, "发光二极管");
        }
        else if (aPartInfo->type.compare ("DIODE") == 0) {
            lookupGeneral (aPartInfo, "二极管");
        }
        else if (aPartInfo->type.compare ("MOSFET") == 0) {
            lookupGeneral (aPartInfo, "场效应管");
        }
        else if (aPartInfo->type.compare ("RESxN") == 0) {
            lookupResister (aPartInfo, "贴片排阻");
        }
    }
    catch (std::exception& aError) {
        emit message (QString ("Lookup part list error. %1").arg (aError.what()));
        return -1;
    }
    catch (const char *aError) {
        emit errorMessage (QString ("Lookup part list error. %1").arg (aError));
        return -1;
    }

    return 0;
}

//==========================================================================
// Lookup Capacitor
//==========================================================================
void CKiCadConverter::lookupCapacitor (struct TPartInfo *aPartInfo, QString aSearchType) {
    QString sql;
    QString str_value = aPartInfo->value;
    int idx;
    str_value.replace ("F", "");    // Remove unit

    // Remove tailing stuff after ±
    idx = str_value.indexOf ("±");
    DEBUG_PRINTF ("n at %d", idx);
    if (idx > 0)
        str_value.remove (idx, str_value.length());

    // Remove tailing stuff after +/-
    idx = str_value.indexOf ("+/-");
    if (idx > 0)
        str_value.remove (idx, str_value.length());

    // Remove tailing stuff after space
    idx = str_value.indexOf (' ');
    if (idx > 0)
        str_value.remove (idx, str_value.length());


    // Correct uF, nF, pF
    if (str_value.endsWith('u')) {
        double f = std::atof (str_value.toUtf8().data());
        if (f < 1) {
            // uF to nF
            f *= 1000;
            str_value.sprintf ("%.0fn", f);
        }
    }
    else if (str_value.endsWith ('n')) {
        double f = std::atof (str_value.toUtf8().data());
        if (f < 1) {
            // nF to pF
            f *= 1000;
            str_value.sprintf ("%.0fp", f);
        }
        else if (f >= 1000) {
            // nF to uF
            f /= 1000;
            str_value.sprintf ("%.0fu", f);
        }
    }
    else if (str_value.endsWith ('p')) {
        double f = std::atof (str_value.toUtf8().data());
        if (f >= 1000) {
            // pF to nF
            f /= 1000;
            str_value.sprintf ("%.0fn", f);
        }
    }

    // Add unit
    str_value.append ("F");

    //
    sql = QString ("SELECT partNumber,name,type,package,padCount,isStanardStock FROM partList"
                   " WHERE name LIKE '%1%' AND package='%2' AND type LIKE '%%3%' ORDER BY isStanardStock DESC;")
                    .arg(str_value).arg(aPartInfo->package).arg(aSearchType);
    DEBUG_PRINTF ("SQL:%s", sql.toUtf8().data());
    sqlite3pp::query qry (dbPartList, sql.toUtf8().data());

    auto row = qry.begin();
    if (row != qry.end()) {

        auto part_number = (*row).get<char const*>(0);
        auto name = (*row).get<std::string>(1);
        auto type = (*row).get<char const*>(2);
//        auto package = (*row).get<char const*>(3);
        auto pad_count = (*row).get<int>(4);
        auto is_standard = (*row).get<int>(5);

        aPartInfo->foundInDb = true;
        aPartInfo->partNumber = part_number;
        aPartInfo->name = name.c_str();
        aPartInfo->type = type;
        aPartInfo->padCount = pad_count;
        if (is_standard)
            aPartInfo->isStandardStock = true;
        else
            aPartInfo->isStandardStock = false;

        DEBUG_PRINTF ("Part found.");
    }
}

//==========================================================================
// Lookup Resister
//==========================================================================
void CKiCadConverter::lookupResister (struct TPartInfo *aPartInfo, QString aSearchType) {
    QString sql;
    QString str_value = aPartInfo->value;
    int idx;

    // Remove unit
    str_value.replace ("OHM", "", Qt::CaseInsensitive);
    str_value.replace ("Ω", "");

    // Remove tailing stuff after ±
    idx = str_value.indexOf ("±");
    DEBUG_PRINTF ("n at %d", idx);
    if (idx > 0)
        str_value.remove (idx, str_value.length());

    // Remove tailing stuff after +/-
    idx = str_value.indexOf ("+/-");
    if (idx > 0)
        str_value.remove (idx, str_value.length());

    // Remove tailing stuff after space
    idx = str_value.indexOf (' ');
    if (idx > 0)
        str_value.remove (idx, str_value.length());

    // Correct nR, nRn
    str_value.replace ("R", ".");
    if (str_value.endsWith ("."))
        str_value.chop(1);

    // Correct nkn
    if (((str_value.at(1) == 'k') || (str_value.at(1) == 'K')) && (str_value.length() > 2)) {
        str_value.replace ("k", ".");
        str_value.append ("k");
    }

    // Add unit
    str_value.append ("Ω");

    //
    sql = QString ("SELECT partNumber,name,type,package,padCount,isStanardStock FROM partList"
                   " WHERE name LIKE '%1%' AND package='%2' AND type LIKE '%%3%' ORDER BY isStanardStock DESC;")
                    .arg(str_value).arg(aPartInfo->package).arg(aSearchType);
    DEBUG_PRINTF ("SQL:%s", sql.toUtf8().data());
    sqlite3pp::query qry (dbPartList, sql.toUtf8().data());

    auto row = qry.begin();
    if (row != qry.end()) {

        auto part_number = (*row).get<char const*>(0);
        auto name = (*row).get<std::string>(1);
        auto type = (*row).get<char const*>(2);
//        auto package = (*row).get<char const*>(3);
        auto pad_count = (*row).get<int>(4);
        auto is_standard = (*row).get<int>(5);

        aPartInfo->foundInDb = true;
        aPartInfo->partNumber = part_number;
        aPartInfo->name = name.c_str();
        aPartInfo->type = type;
        aPartInfo->padCount = pad_count;
        if (is_standard)
            aPartInfo->isStandardStock = true;
        else
            aPartInfo->isStandardStock = false;

        DEBUG_PRINTF ("Part found.");
    }
}

//==========================================================================
// Lookup Inductor
//==========================================================================
void CKiCadConverter::lookupInductor (struct TPartInfo *aPartInfo, QString aSearchType) {
    QString sql;
    QString str_value = aPartInfo->value;

    // Remove unit
    str_value.replace ("H", "");

    // Add unit
    str_value.append ("H");

    //
    sql = QString ("SELECT partNumber,name,type,package,padCount,isStanardStock FROM partList"
                   " WHERE name LIKE '%1%' AND package='%2' AND type LIKE '%%3%'"
                   " ORDER BY isStanardStock DESC;")
                    .arg(str_value).arg(aPartInfo->package).arg(aSearchType);
    DEBUG_PRINTF ("SQL:%s", sql.toUtf8().data());
    sqlite3pp::query qry (dbPartList, sql.toUtf8().data());

    auto row = qry.begin();
    if (row != qry.end()) {

        auto part_number = (*row).get<char const*>(0);
        auto name = (*row).get<std::string>(1);
        auto type = (*row).get<char const*>(2);
//        auto package = (*row).get<char const*>(3);
        auto pad_count = (*row).get<int>(4);
        auto is_standard = (*row).get<int>(5);

        aPartInfo->foundInDb = true;
        aPartInfo->partNumber = part_number;
        aPartInfo->name = name.c_str();
        aPartInfo->type = type;
        aPartInfo->padCount = pad_count;
        if (is_standard)
            aPartInfo->isStandardStock = true;
        else
            aPartInfo->isStandardStock = false;

        DEBUG_PRINTF ("Part found.");
    }
}

//==========================================================================
// Lookup LED
//==========================================================================
void CKiCadConverter::lookupLed (struct TPartInfo *aPartInfo, QString aSearchType) {
    QString sql;
    QString str_value = aPartInfo->value;

    // Determine color
    if ((str_value.contains("BLUE")) || (str_value.contains("BLE"))) {
        str_value = "蓝";
    }
    if ((str_value.contains("GREEN")) || (str_value.contains("GRN"))) {
        str_value = "绿";
    }
    if (str_value.contains("RED")) {
        str_value = "红";
    }

    //
    sql = QString ("SELECT partNumber,name,type,package,padCount,isStanardStock FROM partList"
                   " WHERE name LIKE '%1%' AND package='%2' AND type LIKE '%%3%'"
                   " ORDER BY isStanardStock DESC;")
                    .arg(str_value).arg(aPartInfo->package).arg(aSearchType);
    DEBUG_PRINTF ("SQL:%s", sql.toUtf8().data());
    sqlite3pp::query qry (dbPartList, sql.toUtf8().data());

    auto row = qry.begin();
    if (row != qry.end()) {

        auto part_number = (*row).get<char const*>(0);
        auto name = (*row).get<std::string>(1);
        auto type = (*row).get<char const*>(2);
//        auto package = (*row).get<char const*>(3);
        auto pad_count = (*row).get<int>(4);
        auto is_standard = (*row).get<int>(5);

        aPartInfo->foundInDb = true;
        aPartInfo->partNumber = part_number;
        aPartInfo->name = name.c_str();
        aPartInfo->type = type;
        aPartInfo->padCount = pad_count;
        if (is_standard)
            aPartInfo->isStandardStock = true;
        else
            aPartInfo->isStandardStock = false;

        DEBUG_PRINTF ("Part found.");
    }
}


//==========================================================================
// Lookup General parts
//==========================================================================
void CKiCadConverter::lookupGeneral (struct TPartInfo *aPartInfo, QString aSearchType) {
    QString sql;

    //
    sql = QString ("SELECT partNumber,name,type,package,padCount,isStanardStock FROM partList"
                   " WHERE name LIKE '%%1%' AND package='%2' AND type LIKE '%%3%'"
                   " ORDER BY isStanardStock DESC;")
                    .arg(aPartInfo->value).arg(aPartInfo->package).arg(aSearchType);
    DEBUG_PRINTF ("SQL:%s", sql.toUtf8().data());
    sqlite3pp::query qry (dbPartList, sql.toUtf8().data());

    auto row = qry.begin();
    if (row != qry.end()) {

        auto part_number = (*row).get<char const*>(0);
        auto name = (*row).get<std::string>(1);
        auto type = (*row).get<char const*>(2);
//        auto package = (*row).get<char const*>(3);
        auto pad_count = (*row).get<int>(4);
        auto is_standard = (*row).get<int>(5);

        aPartInfo->foundInDb = true;
        aPartInfo->partNumber = part_number;
        aPartInfo->name = name.c_str();
        aPartInfo->type = type;
        aPartInfo->padCount = pad_count;
        if (is_standard)
            aPartInfo->isStandardStock = true;
        else
            aPartInfo->isStandardStock = false;

        DEBUG_PRINTF ("Part found.");
    }
}
