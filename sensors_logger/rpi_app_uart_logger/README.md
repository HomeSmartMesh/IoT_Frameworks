# C++ Sensors Streamer
- Serial port reader
- Conversion of sensors registers to values through calibration parameters for BME280
- Database manager : files and memory management of binary data, text std::string request and response
- websocket manager : interface to the nodejs server

# Environment and Dependencies
- Scons : a single line for the whole compilation, linker stuf,...
- C++11 : Modern c++ is easier than javascript (the luxury of having a compiler)
- Boost 1.60 (filesystem) : A warm welcome to Boost on RPI (see install instructions)
- json for modern c++ : yes, json as easy to use as in javascript https://github.com/nlohmann/json
- C++ websocket client : https://github.com/dhbaird/easywsclient

## Boost installation on the Raspberry pi
Minimal install for filesystem support:
```
mkdir boost
cd boost
wget http://sourceforge.net/projects/boost/files/boost/1.60.0/boost_1_60_0.tar.bz2
tar xvfo boost_1_60_0.tar.bz2
cd boost_1_60_0
./bootstrap.sh --with-libraries=filesystem,system
sudo ./b2 install
```
