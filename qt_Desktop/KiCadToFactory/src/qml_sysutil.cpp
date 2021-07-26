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
#include <QSysInfo>
#include <QApplication>
#include <QUuid>

#include "debug.h"

#include "qml_sysutil.h"

//==========================================================================
// Defines
//==========================================================================

//==========================================================================
// Constants
//==========================================================================


//==========================================================================
// Static Variables
//==========================================================================
static QQmlApplicationEngine *gCurrentQmlEngine = nullptr;

//==========================================================================
//==========================================================================
// Public members
// vvvvvvvvvvvvvv
//==========================================================================
//==========================================================================

//==========================================================================
// Constructor
//==========================================================================
CSysUtil::CSysUtil (QObject *aParent) :
                                       QObject(aParent){

}

//==========================================================================
// Destroyer
//==========================================================================
CSysUtil::~CSysUtil (void) {

}

//==========================================================================
// Set gOfflineStoragePath
//==========================================================================
void CSysUtil::setQmlEngine (QQmlApplicationEngine *aValue) {
  gCurrentQmlEngine = aValue;
}

//==========================================================================
// Limit a string in UTF8 length
//==========================================================================
QString CSysUtil::limitedUtf8Len (QString aValue, int aMaxLen) {
  char buf [32];

  if (aMaxLen < 1)
    return "";

  qstrncpy (buf, aValue.toUtf8().data(), sizeof (buf));
  while (qstrlen (buf) > static_cast<size_t>(aMaxLen)) {
    aValue.remove (aValue.length() - 1, 1);
    qstrncpy (buf, aValue.toUtf8().data(), sizeof (buf));
  }
  return aValue;
}


//==========================================================================
// QML Properties
//==========================================================================
QString CSysUtil::readOfflineStoragePath (void) {
  if (gCurrentQmlEngine == nullptr) {
    return "";
  }
  else {
    return gCurrentQmlEngine->offlineStoragePath ();
  }
}

//==========================================================================
// Get application version
//==========================================================================
QString CSysUtil::getApplicationVersion (void) {
  return QCoreApplication::applicationVersion();
}

//==========================================================================
// Generate a UID
//==========================================================================
QString CSysUtil::generateUid (void) {
  auto gen_uuid = QUuid::createUuid();
  auto phoneUid = gen_uuid.toRfc4122();
  return phoneUid.toHex();
}

//==========================================================================
// Check OS is Windows
//==========================================================================
bool CSysUtil::isOsWin (void) {
  bool ret;
#ifdef Q_OS_WIN
  ret = true;
#else
  ret = false;
#endif
  return ret;
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

