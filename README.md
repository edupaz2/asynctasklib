# asynctasklib
Asynchronous Task Library in C++ and Boost

Context switching is expensive. Potentially, very expensive.

### Advantages of fibers over threads
- Very fast context switch: Thread switching in operating systems is a very expensive operation that involves switching to kernel mode. A typical OS thread context switch executes in thousands of CPU cycles. Fibers on the other head can switch in about 100 CPU cycles.
- No concurrency issues: Unlike threads, fibers are scheduled with cooperative multitasking. A fiber runs until it chooses to yield control back to the fiber scheduler. Thus fibers could update shared data structures without the risk of another fiber trampling over their updates.
- Choose your scheduler: With threads you are stuck with the thread scheduling approaches baked into the OS. With fibers, you can choose a scheduler that is custom fitted for your application.

CppCon 2017: David Sackstein “Boost Your Program’s Health by Adding Fibers to your Coroutine”
[Video](https://www.youtube.com/watch?v=mCD6VLVS_y4)

Parallelizing the Naughty Dog engine using fibers
[Video](https://www.gdcvault.com/play/1022186/Parallelizing-the-Naughty-Dog-Engine)
[PDF](http://twvideo01.ubm-us.net/o1/vault/gdc2015/presentations/Gyrling_Christian_Parallelizing_The_Naughty.pdf)

Fibers: are more about maintainability than performance
[Boost fiber in your code](https://medium.com/software-design/boost-fiber-in-your-code-9dcdda70ca00)
[Coroutines and Fibers. Why and When](https://medium.com/software-development-2/coroutines-and-fibers-why-and-when-5798f08464fd)

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