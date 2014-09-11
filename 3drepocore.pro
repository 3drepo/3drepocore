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

QT  -= core gui

TEMPLATE = lib

CONFIG += build_all

DEFINES += REPO_CORE_LIBRARY

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lassimp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lassimpd
else:unix: LIBS += -L$$PWD/lib/ -lassimp

INCLUDEPATH += $$PWD/submodules/assimp/include
DEPENDPATH += $$PWD/submodules/assimp/include

unix {
    target.path = /usr/lib
    INSTALLS += target
}

HEADERS +=  RepoCoreGlobal \
            src/repocore.h\
            src/repocoreglobal.h \
            src/graph/repo_bounding_box.h \

SOURCES +=  src/repocore.cpp \
            src/graph/repo_bounding_box.cpp \


