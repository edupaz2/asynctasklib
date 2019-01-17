# asynctasklib
Asynchronous Task Library in C++ and Boost

## Dependencies
Boost::Fiber > 1.62

### In Ubuntu 18.04
```
sudo apt-get install libboost-fiber1.65-dev
```

### Notes
If you cannot find Boost::Fiber greater than 1.62, please check Troubleshooting

## Build Library
```
mkdir build
cd build && cmake ..
make
make install
```

## Build Command line program
```
g++ -o program src-cmd/program.cpp -I include/ -lasynctasklib -lboost_fiber -lboost_context -lpthread
```

## Troubleshooting
How to download and prepare Boost library from source
```
mkdir tmp && cd tmp
wget https://dl.bintray.com/boostorg/release/1.69.0/source/boost_1_69_0.tar.bz2
tar --bzip2 -xf boost_1_69_0.tar.bz2
rm boost_1_69_0.tar.bz2
cd boost_1_69_0
./bootstrap.sh --prefix=../../third_party/boost --with-libraries=fiber
./b2 cxxflags="-std=c++14" --reconfigure --with-fiber install
```