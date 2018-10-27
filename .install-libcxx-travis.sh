sudo apt-get install -y dpkg
wget -c http://launchpadlibrarian.net/360656578/libc++-helpers_6.0-2_all.deb
sudo dpkg -i libc++-helpers_6.0-2_all.deb
wget -c http://launchpadlibrarian.net/360656583/libc++abi1_6.0-2_amd64.deb
sudo dpkg -i libc++abi1_6.0-2_amd64.deb
wget -c http://launchpadlibrarian.net/360656580/libc++1_6.0-2_amd64.deb
sudo dpkg -i libc++1_6.0-2_amd64.deb
wget -c http://launchpadlibrarian.net/360656576/libc++-dev_6.0-2_amd64.deb
sudo dpkg -i libc++-dev_6.0-2_amd64.deb
wget -c http://launchpadlibrarian.net/360656581/libc++abi-dev_6.0-2_amd64.deb
sudo dpkg -i libc++abi-dev_6.0-2_amd64.deb
sudo apt-get install -f
