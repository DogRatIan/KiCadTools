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
#ifndef INC_QML_PART_LIST_H
#define INC_QML_PART_LIST_H

//==========================================================================
//==========================================================================
#include <QObject>
#include <QFile>

#include "sqlite3pp.h"

//==========================================================================
// Defines
//==========================================================================

//==========================================================================
//==========================================================================
class CPartList : public QObject {
    Q_OBJECT

public:
    explicit CPartList (QObject *aParent = nullptr);
     ~CPartList (void);

    // Methods for QML
    Q_INVOKABLE bool init (void);
    Q_INVOKABLE bool importCsvFile (QString aFilePath);

    // Properties for QML
    Q_PROPERTY (QString filename READ readFilename)
    Q_PROPERTY (double fileSize READ readFileSize)
    Q_PROPERTY (int rowCount READ readRowCount)
    Q_PROPERTY (bool isReady READ readIsReady)

    //
    QString readFilename (void);
    double readFileSize (void);
    int readRowCount (void);
    bool readIsReady (void);

    //

signals:
    void message (const QString aMessage);
    void errorMessage (const QString aMessage);
    void importProgressUpdated (double aProgress);

private:
    QString currentFilename;
    double currentFileSize;
    int currentRowCount;
    sqlite3pp::database database;
    bool databaseReady;

    QString getPath (void);
    void updateFileSize (void);
    void updateRowCount (void);
    int removeAllData (void);
    int checkImportFile (QFile *aInputFile);
    int doImport (QFile *aInputFile, int aTotalLineCount);
};

//==========================================================================
//==========================================================================
#endif
