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
# Boost
win32: LIBS += -LC:/local/boost_1_56_0/lib64-msvc-12.0/
win32:INCLUDEPATH += C:/local/boost_1_56_0/
win32:DEPENDPATH += C:/local/boost_1_56_0/

macx {
    _BOOST_PATH = /usr/local/Cellar/boost/1.56.0
    INCLUDEPATH += "$${_BOOST_PATH}/include/"
    LIBS += -L$${_BOOST_PATH}/lib
    LIBS += -lboost_chrono-mt -lboost_system -lboost_filesystem
}
