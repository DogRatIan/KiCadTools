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
#ifndef INC_QML_SYSUTIL_H
#define INC_QML_SYSUTIL_H

//==========================================================================
//==========================================================================
#include <QObject>
#include <QTranslator>
#include <QQmlApplicationEngine>

//==========================================================================
// Defines
//==========================================================================

//==========================================================================
//==========================================================================
class CSysUtil : public QObject {
  Q_OBJECT

 public:
  explicit CSysUtil (QObject *aParent = nullptr);
  ~CSysUtil (void);

  // Methods for QML
  Q_INVOKABLE QString limitedUtf8Len (QString aValue, int aMaxLen);
  Q_INVOKABLE QString getApplicationVersion (void);
  Q_INVOKABLE QString generateUid (void);
  Q_INVOKABLE bool isOsWin (void);

  // Properties for QML
  Q_PROPERTY (QString offlineStoragePath READ readOfflineStoragePath)

  //
  QString readOfflineStoragePath (void);

  // Static functions
  static void setQmlEngine (QQmlApplicationEngine *aValue);

 signals:

 private:
  QString langFileLoaded;

  QByteArray generateAesKey (QString aKey);
};

//==========================================================================
//==========================================================================
#endif
