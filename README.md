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
g++ -o program src-cmd/program.cpp src-lib/asynctasklib.cpp src-lib/processor.cpp src-lib/priority_scheduler.cpp -I include/ -I src-lib/ -lboost_fiber -lboost_context -lpthread
```