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

# http://qt-project.org/doc/qt-5/qmake-variable-reference.html
# http://google-styleguide.googlecode.com/svn/trunk/cppguide.html

include(header.pri)
include(boost.pri)
include(assimp.pri)
include(mongo.pri)

TEMPLATE = app
#CONFIG += debug_and_release
#release: TARGET = repo_cli
#debug: TARGET = repo_cli_debug

QT += core gui # TODO: remove Qt dependencies, ie it should be -=

#-------------------------------------------------------------------------------
# 3drepocore

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/release/ -l3drepocore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/debug/ -l3drepocore
else:unix: LIBS += -L$$OUT_PWD/ -l3drepocore

INCLUDEPATH += $$PWD/src
DEPENDPATH += $$PWD/src

#-------------------------------------------------------------------------------
# Input
#HEADERS +=
SOURCES += src/cli.cpp
#-------------------------------------------------------------------------------
