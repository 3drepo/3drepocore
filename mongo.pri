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

#-------------------------------------------------------------------------------
# MongoDB C++ Driver

win32:DEFINES += WIN32_LEAN_AND_MEAN VC_EXTRALEAN
win32:LIBS += -lws2_32

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lmongoclient
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lmongoclientd
else:unix:!macx: LIBS += -L$$PWD/submodules/mongo-cxx-driver/lib -lmongoclient
else:macx: LIBS += -L$$PWD/submodules/mongo-cxx-driver/build/darwin/64/osx-version-min_10.9/use-system-boost -lmongoclient

INCLUDEPATH += $$PWD/submodules/mongo-cxx-driver/src
DEPENDPATH += $$PWD/submodules/mongo-cxx-driver/src
