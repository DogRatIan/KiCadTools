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
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>

#include "qml_fileinfo.h"

//==========================================================================
// Defines
//==========================================================================

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
CFileInfo::CFileInfo (QObject *aParent) :
    QObject(aParent) {
}

//==========================================================================
// Destroyer
//==========================================================================
CFileInfo::~CFileInfo (void) {
}

//==========================================================================
// Returns the base name of the file without the path.
//==========================================================================
QString CFileInfo::baseName (QString aPath) {
    auto info = QFileInfo (aPath);
    return info.baseName ();
}

//==========================================================================
// Returns the name of the file, excluding the path.
//==========================================================================
QString CFileInfo::fileName (QString aPath) {
    auto info = QFileInfo (aPath);
    return info.fileName ();
}

//==========================================================================
// Returns the file's path. This doesn't include the file name.
//==========================================================================
QString CFileInfo::path (QString aPath) {
    auto info = QFileInfo (aPath);
    return info.path ();
}

//==========================================================================
// Returns the directory that contains the application executable.
//==========================================================================
QString CFileInfo::applicationDirPath (void) {
    return QCoreApplication::applicationDirPath ();
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
