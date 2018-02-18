# Robotieee

# Getting Started

 - Install Arduino IDE from [Installing Arduino IDE](https://www.arduino.cc/en/Guide/Linux);
 - Follow the guide of the [Sumo Robot Arduino IDE configuration](https://www.pololu.com/docs/0J63/5.2);


# Building robo-utils

Main library: you can choose to build on debug (no optimizartions but you can debug it with gdb or valgrind) or in production (optimization but no debuggin support).
If you want the debugging create the directory `build/Debug` otherwise `build/Release`:

```
cd robo-utils
mkdir -p build/Debug
cd build/Debug
//if you want to create a library which may use std lib use this option:
cmake -D U_BUILD_TYPE=DESKTOP_BUILD ../..
//if you want to create a library which can't use std lib use this option:
cmake -D U_BUILD_TYPE=AVR_BUILD ../..
make
```

there's should be a static library (*.a) in build/Debug you can use in the Arduino code. You can also buld the documentation. You need some software to do so:

```
//dot installation
sudo apt-get install graphviz
//doxygen installation (copied from http://www.stack.nl/~dimitri/doxygen/download.html)
cd ~
git clone https://github.com/doxygen/doxygen.git
cd doxygen
mkdir build
cd build
cmake -G "Unix Makefiles" ..
make
sudo make install
```

```
cd robo-utils
doxygen robo-utils.doxygen
```

The output is in `build/doc`, accessible via HTML on the file `index.html`

# References

 - [Sumo Robot manual](https://www.pololu.com/docs/0J63)
