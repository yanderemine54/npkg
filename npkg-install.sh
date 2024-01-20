# Copyright (C) 2024 Yanderemine54
# 
# This file is part of npkg.
# 
# npkg is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# npkg is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with npkg.  If not, see <http://www.gnu.org/licenses/>.

echo "Configuring source..."
./configure --prefix=/usr/local/npkg-0.3.0
echo "Source configured."

echo "Compiling source..."
make -j$(nproc)
echo "Source compiled."

if [ $EUID -ne 0 ]
then
        echo "User doesn't have root privileges. Not installing package."
        exit
else
        echo "Installing package..."
        gpg --keyserver keyserver.ubuntu.com --recv-keys AF3B8064
        ./trust-key.exp AF3B8064
        make install
        ln -s /usr/local/npkg-0.2.0/bin/npkg /usr/local/bin/npkg
        echo "Package installed."
fi