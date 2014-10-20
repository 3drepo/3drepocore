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

unix:QMAKE_CXXFLAGS += -fpermissive -std=c++11
unix:QMAKE_CXXFLAGS_DEBUG -= -O1
unix:QMAKE_CXXFLAGS_DEBUG += -O0

#-------------------------------------------------------------------------------
# Boost
win32: LIBS += -LC:/local/boost_1_56_0/lib64-msvc-12.0/
win32:INCLUDEPATH += C:/local/boost_1_56_0/
win32:DEPENDPATH += C:/local/boost_1_56_0/

#-------------------------------------------------------------------------------
# Assimp
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lassimp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lassimpd
else:unix: LIBS += -L$$PWD/lib/ -lassimp

INCLUDEPATH += $$PWD/submodules/assimp/include
DEPENDPATH += $$PWD/submodules/assimp/include

#-------------------------------------------------------------------------------
# MongoDB C++ Driver

win32:LIBS += -lws2_32

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lmongoclient
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lmongoclientd
else:unix: LIBS += -L$$PWD/lib/ -lmongoclient

INCLUDEPATH += $$PWD/submodules/mongo-cxx-driver/src
DEPENDPATH += $$PWD/submodules/mongo-cxx-driver/src

#-------------------------------------------------------------------------------
