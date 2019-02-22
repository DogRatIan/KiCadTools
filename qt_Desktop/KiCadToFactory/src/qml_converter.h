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
#ifndef INC_QML_CONVERTER_H
#define INC_QML_CONVERTER_H

//==========================================================================
//==========================================================================
#include <QObject>
#include <QFile>

#include "sqlite3pp.h"

//==========================================================================
// Defines
//==========================================================================
struct TPartInfo;

//==========================================================================
//==========================================================================
class CKiCadConverter : public QObject {
    Q_OBJECT

public:
    explicit CKiCadConverter (QObject *aParent = nullptr);
     ~CKiCadConverter (void);

    // Methods for QML
    Q_INVOKABLE bool convertPositionFile (QString aFilePath);
    Q_INVOKABLE bool convertBomFile (QString aFilePath);

    // Properties for QML
    Q_PROPERTY (QString outputPath READ readOutputPath WRITE writeOutputPath)
    Q_PROPERTY (QString positionFilename READ readPositionFilename WRITE writePositionFilename)
    Q_PROPERTY (QString bomFilename READ readBomFilename WRITE writeBomFilename)

    //
    QString readOutputPath (void);
    void writeOutputPath (QString aValue);
    QString readPositionFilename (void);
    void writePositionFilename (QString aValue);
    QString readBomFilename (void);
    void writeBomFilename (QString aValue);

signals:
    void message (const QString aMessage);
    void errorMessage (const QString aMessage);

private:
    QString currentOutputPath;
    QString currentPositionFilename;
    QString currentBomFilename;

    sqlite3pp::database dbConversion;
    sqlite3pp::database dbPartList;

    int openDb (void);
    void closeDb (void);
    int checkOutputPath (void);
    int checkFile (QFile *aInputFile, char aSeparator, int aFieldCount, bool skipFirstLine = false);
    int convPositionForJlc (QFile *aInputFile, int aInputLineCount, QFile *aOutputFile);
    int convBomForJlc (QFile *aInputFile, int aInputLineCount, QFile *aOutputFile);
    QString determinePartType (QString aRef, QString aValue, QString aPackage);
    QString determinePackage (QString aPackage);
    int lookupFormPartList (struct TPartInfo *aPartInfo);
    void lookupCapacitor (struct TPartInfo *aPartInfo, QString aSearchType);
    void lookupResister (struct TPartInfo *aPartInfo, QString aSearchType);
    void lookupInductor (struct TPartInfo *aPartInfo, QString aSearchType);
    void lookupLed (struct TPartInfo *aPartInfo, QString aSearchType);
    void lookupGeneral (struct TPartInfo *aPartInfo, QString aSearchType);
};

//==========================================================================
//==========================================================================
#endif
