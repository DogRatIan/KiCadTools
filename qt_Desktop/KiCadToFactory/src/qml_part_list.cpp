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
#include <QDir>
#include <QDateTime>
#include <cmath>

#include "debug.h"

#include "qml_part_list.h"

//==========================================================================
// Defines
//==========================================================================
#define DEFAULT_DB_FILENAME "partlist.sqlite3"

#define IMPORT_FIELD_PER_LINE   7

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
CPartList::CPartList (QObject *aParent) :
    QObject(aParent) {
    databaseReady = false;
}

//==========================================================================
// Destroyer
//==========================================================================
CPartList::~CPartList (void) {
    database.disconnect();
}


//==========================================================================
// Initialization
//==========================================================================
bool CPartList::init (void) {
    try {
        QString sql;
        auto path = getPath ();

        // Connecto to database file
        auto conn_ret = database.connect (path.toUtf8().data(), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
        if (conn_ret != SQLITE_OK) {
            emit errorMessage ("Can't connect database.");
            return false;
        }
        database.set_busy_timeout (2000);

        sql = "CREATE TABLE IF NOT EXISTS partList ("
                "partNumber     STRING PRIMARY KEY UNIQUE NOT NULL,"
                "name           VARCHAR,"
                "type           VARCHAR,"
                "package        VARCHAR,"
                "padCount       INTEGER,"
                "brand          VARCHAR,"
                "isStanardStock BOOLEAN DEFAULT (false)"
                ");";
        DEBUG_PRINTF ("SQL:%s", sql.toUtf8().data());
        if (database.execute (sql.toUtf8().data()) != SQLITE_OK)
            throw database.error_msg();

        sql = "CREATE INDEX IF NOT EXISTS name ON partList ("
                "name"
                ");";
        DEBUG_PRINTF ("SQL:%s", sql.toUtf8().data());
        if (database.execute (sql.toUtf8().data()) != SQLITE_OK)
            throw database.error_msg();

        sql = "CREATE INDEX IF NOT EXISTS name_package ON partList ("
                "name, package"
                ");";
        DEBUG_PRINTF ("SQL:%s", sql.toUtf8().data());
        if (database.execute (sql.toUtf8().data()) != SQLITE_OK)
            throw database.error_msg();

        sql = "CREATE INDEX IF NOT EXISTS name_type ON partList ("
                "name, type"
                ");";
        DEBUG_PRINTF ("SQL:%s", sql.toUtf8().data());
        if (database.execute (sql.toUtf8().data()) != SQLITE_OK)
            throw database.error_msg();

        //
        if (database.execute ("PRAGMA journal_mode = TRUNCATE") != SQLITE_OK)
            throw database.error_msg();
        if (database.execute ("PRAGMA encoding='UTF-8';") != SQLITE_OK)
            throw database.error_msg();

        //
        databaseReady = true;
        updateFileSize ();
        updateRowCount ();
    }
    catch (std::exception& aError) {
        emit errorMessage (QString ("CPartList init error. %1").arg (aError.what()));
    }
    catch (const char *aError) {
        emit errorMessage (QString ("CPartList init error.\n%1").arg (aError));
    }

    return true;
}

//==========================================================================
// Import CSV file to database
//==========================================================================
bool CPartList::importCsvFile (QString aFilePath) {
    bool ret = false;
    DEBUG_PRINTF ("CPartList::importCsvFile: %s", aFilePath.toUtf8().data());
    QFileInfo info (aFilePath);
    QFile input_file (aFilePath);

    if (!input_file.exists()) {
        emit errorMessage (QString ("%1 not found.").arg (info.fileName()));
        return ret;
    }

    do {
        if (!input_file.open(QIODevice::ReadOnly)) {
            break;
        }

        // Check file content
        auto line_count = checkImportFile (&input_file);
        if (line_count < 0) {
            break;
        }
        emit message (QString ("Importing CSV file, %1 lines.").arg (line_count));
        DEBUG_PRINTF ("line_count=%d", line_count);

        // Clear DB
        if (removeAllData () < 0) {
            break;
        }
        emit message ("Database data removed.");

        // Import data
        emit message ("Importing data...");
        if (doImport (&input_file, line_count) < 0) {
            break;
        }

        // All done
        ret = true;
        emit message ("Import done.");
    } while (0);
    input_file.close ();

    updateFileSize();
    updateRowCount();

    return ret;
}

//==========================================================================
// Read/Write properties
//==========================================================================
QString CPartList::readFilename (void) {
    if (currentFilename.length() <= 0)
        return DEFAULT_DB_FILENAME;
    else
        return currentFilename;
}

double CPartList::readFileSize (void) {
    return currentFileSize;
}

int CPartList::readRowCount (void) {
    return currentRowCount;
}


bool CPartList::readIsReady (void) {
    return databaseReady;
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
// Generate config file PATH
//==========================================================================
QString CPartList::getPath (void) {
    QString path = QCoreApplication::applicationDirPath();
    path.append (QDir::separator());
    path.append (readFilename());
    return path;
}

//==========================================================================
// Update file size
//==========================================================================
void CPartList::updateFileSize (void) {
    QFileInfo info (getPath ());
    if (info.exists()) {
        currentFileSize = static_cast<double>(info.size()) / 1024 / 1024;
    }
    else {
        currentFileSize = std::nan("");
    }
}

//==========================================================================
// Update row count
//==========================================================================
void CPartList::updateRowCount (void) {
    QString sql;

    if (!databaseReady) {
        currentRowCount = 0;
        return;
    }

    //
    try {
        sql = "SELECT count (*) FROM partList;";
        DEBUG_PRINTF ("SQL:%s", sql.toUtf8().data());
        sqlite3pp::query qry (database, sql.toUtf8().data());

        auto row = qry.begin();
        currentRowCount = (*row).get<int>(0);
    }
    catch (std::exception& aError) {
        emit errorMessage (QString ("CPartList update row count error. %1").arg (aError.what()));
        currentRowCount = 0;
    }
}

//==========================================================================
// Remove all data
//==========================================================================
int CPartList::removeAllData (void) {
    QString sql;
    int ret = -1;

    if (!databaseReady)
        return ret;

    try {
        sql = QString ("DELETE FROM partList;");
        DEBUG_PRINTF ("SQL:%s", sql.toUtf8().data());
        if (database.execute (sql.toUtf8().data()) != SQLITE_OK)
            throw database.error_msg();

        // ALL done
        ret = 0;
    }
    catch (std::exception& aError) {
        emit errorMessage (QString ("CPartList remove data error. %1").arg (aError.what()));
    }
    catch (const char *aError) {
        emit errorMessage (QString ("CPartList remove data error.\n%1").arg (aError));
    }

    return ret;

}

//==========================================================================
// Check import file. Return line count if success, -1 if fail
//==========================================================================
int CPartList::checkImportFile (QFile *aInputFile) {
    int line_count = 0;
    char line_buf[256];

    if (!aInputFile->isOpen()) {
        emit errorMessage ("Import error.\nFile not opened.");
        return -1;
    }

    aInputFile->seek(0);
    while (aInputFile->readLine (line_buf, sizeof (line_buf)) >= 0) {
        line_count ++;

//        QString str_line (line_buf);
        QString str_line = QString::fromUtf8(line_buf);
        auto list = str_line.split('\t');
        if (list.count() != IMPORT_FIELD_PER_LINE) {
            emit errorMessage (QString ("Import file error.\nLine %1 invalid.").arg (line_count));
            return -1;
        }
    }

    return line_count;
}

//==========================================================================
// Import data from CSV file
//==========================================================================
int CPartList::doImport (QFile *aInputFile, int aTotalLineCount) {
    QString sql;
    int record_count = 0;
    int line_count = 0;
    char line_buf[256];

    if (!aInputFile->isOpen()) {
        emit errorMessage ("Import error.\nFile not opened.");
        return -1;
    }

    // Turn off sync to speed up the INSERT
    database.execute ("PRAGMA synchronous = OFF");

    aInputFile->seek(0);
    while (aInputFile->readLine (line_buf, sizeof (line_buf)) >= 0) {
        QString str_line = QString::fromUtf8(line_buf);
        str_line.replace ("\n", "");
        str_line.replace ("\r", "");

        auto list = str_line.split('\t');

        //
        if ((line_count % 32) == 0) {
            emit importProgressUpdated (static_cast<double>(line_count) / aTotalLineCount * 100);
            QCoreApplication::processEvents();
        }
        line_count ++;

        // Check header
        if (list.at(0).compare ("元件编号") == 0) {
            emit message ("Header found. Line skipped.");
            continue;
        }

        //
        try {
            QString is_standard_stock = "false";
            if (list.at (6).compare ("基础库") == 0) {
                is_standard_stock = "true";
            }
            sql = QString ("INSERT INTO partList (partNumber, name, type, package, padCount, brand,isStanardStock)"
                            " VALUES ('%1', '%2', '%3', '%4', '%5', '%6', %7);")
                            .arg (list.at(0)).arg (list.at(1)).arg (list.at(2)).arg (list.at(3)).arg (list.at(4))
                            .arg (list.at(5)).arg (is_standard_stock);
            if (database.execute (sql.toUtf8().data()) != SQLITE_OK)
                throw database.error_msg();
            record_count ++;
        }
        catch (std::exception& aError) {
            emit errorMessage (QString ("Import database error. %1").arg (aError.what()));
            record_count = -1;
            break;
        }
        catch (const char *aError) {
            emit errorMessage (QString ("Import database error.\n%1").arg (aError));
        }
    }

    if (line_count == aTotalLineCount)
        emit importProgressUpdated (100);

    emit message (QString ("%1 records created.").arg (record_count));

    // Restore sync for database
    database.execute ("PRAGMA synchronous = ON");

    return record_count;
}
