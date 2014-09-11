#  Copyright (C) 2014 3D Repo Ltd
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Affero General Public License as
#  published by the Free Software Foundation, either version 3 of the
#  License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Affero General Public License for more details.
#
#  You should have received a copy of the GNU Affero General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

# http://qt-project.org/doc/qt-5/sharedlibrary.html

QT  -= core gui

TARGET = 3drepocore
TEMPLATE = lib

DESTDIR = 3drepocore

CONFIG += build_all

DEFINES += REPO_CORE_LIBRARY

SOURCES += repocore.cpp

INCLUDEPATH += src \

HEADERS +=  repocoreglobal.h \
            repocore.h\


unix {
    target.path = /usr/lib
    INSTALLS += target
}
