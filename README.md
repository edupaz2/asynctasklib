# asynctasklib
Asynchronous Task Library in C++ and Boost

## Dependencies
Boost::Fiber > 1.62

### In Ubuntu 18.04
```
sudo apt-get install libboost-fiber1.65-dev
```

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