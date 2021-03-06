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
cmake ../.. //usually you need to do that
//other cmake options:
//if you want to create a library which may use std lib use this option:
cmake -D U_BUILD_TYPE=DESKTOP_BUILD -D U_ARDUINO_LIBRARY_FOLDER="~/Arduino/libraries" ../..

make
//install the library into the default Arduino library path folder
make sketch
```

there's should be a static library (*.a) in build/Debug you can use in the Arduino code. 

## cmake options

The building process has some flags you have to be aware:

|Flag				|Possible value		|Description						|Example					|
|-------------------------------|-----------------------|-------------------------------------------------------|-----------------------------------------------|
|`U_BUILD_TYPE`			|`DESKTOP_BUILD`	|Incorporate in the build the std library		|`cmake -D U_BUILD_TYPE=DESKTOP_BUILD  ../..`	|
|`U_BUILD_TYPE`			|`AVR_BUILD`		|(default) don't build with std library			|`cmake -D U_BUILD_TYPE=AVR_BUILD  ../..`	|
|`U_ARDUINO_LIBRARY_FOLDER`	|a path			|the place where arduino puts the custom libraries	|`cmake -D U_ARDUINO_LIBRARY_FOLDER="a/p" ../..`|

## Documentation

You can also buld the documentation. You need some software to do so:

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

# Python planner web server

The web server uses Python3.6. The web server can be customized with the following CLI options:

```
cd Server/planner_wrapper/
virtualenv venv
pip install -f requirements.txt
python main.py --planner_location="place/where/the/planner/is" --use_planner="lpg" --ip="0.0.0.0" --port=5000
```

Alternatively you can simulate the planner by using:

```
python main.py --planner_location="ignored" --use_planner="fake" --ip="0.0.0.0" --port=5000`
```

## Sokoban solver

In *Server/planner_wrapper* you can find a python program generating a server which execute a classic planner to solve Sokoban problem. It is the problem of moving all the blocks in some goals in the map and then make the agent return to the docking station.
The server listen for POST requests at the URL `ip:port/sokoban_problem`. In the payload of the request, a json needs to be provided. Furthermore, be sure to add to the header of the request the "Content-Type" "application/json". The server will reply with a 200 response containing a payload with the plan computed.

In the following we will assume the following problem (made thanks by the website [sokoban-editor](https://deificx.github.io/sokoban-editor/) ):

![sokoban example](https://github.com/STB1019/Robotieee/blob/master/images/sokobanExample.PNG "Sokoban example")

The yellow circle is the robot, the crate is the block that needs to be put on the red cross. If this is the state of the world, the following can be applied.

### JSON request format

For solving a Sokoban problem using *push* actions, use the following json:

```
{
  "version": "1.0",
  "world": {
    "rows": 3,
    "columns": 3,
    "cells": [
      { "y": 0, "x": 0, "entities": "G"},
      { "y": 0, "x": 1, "entities": "RD"},
      { "y": 0, "x": 2, "entities": "U"},
      { "y": 1, "x": 0, "entities": ""},
      { "y": 1, "x": 1, "entities": "B"},
      { "y": 1, "x": 2, "entities": ""},
      { "y": 2, "x": 0, "entities": ""},
      { "y": 2, "x": 1, "entities": ""},
      { "y": 2, "x": 2, "entities": ""}
    ]
  }
}
```

where in the string field *entities*:

 - *B* stands for *block*;
 - *R* stands for *robot*;
 - *G* stands for *goal*;
 - *U* stands for *untraversable*;
 - *D* stands for *docking station*;

Multiple objects can be within the same cell.

### JSON reply

```
{
  "version": "1.0", 
  "actions": [
    {"action": "move",            "player": "player-01", "from": {"y": 0, "x": 0}, "to": {"y": 1, "x": 0}, "direction": "dir-down"}, 
    {"action": "move",            "player": "player-01", "from": {"y": 1, "x": 0}, "to": {"y": 2, "x": 0}, "direction": "dir-down"}, 
    {"action": "move",            "player": "player-01", "from": {"y": 2, "x": 0}, "to": {"y": 2, "x": 1}, "direction": "dir-right"}, 
    {"action": "move",            "player": "player-01", "from": {"y": 2, "x": 1}, "to": {"y": 2, "x": 2}, "direction": "dir-right"}, 
    {"action": "move",            "player": "player-01", "from": {"y": 2, "x": 2}, "to": {"y": 1, "x": 2}, "direction": "dir-up"}, 
    {"action": "push-to-nongoal", "player": "player-01", "stone": "stone-00", "player-start-pos": {"y": 1, "x": 2}, "start-pos": {"y": 1, "x": 1}, "end-pos": {"y": 1, "x": 0}, "direction": "dir-left"},
    {"action": "move",            "player": "player-01", "from": {"y": 1, "x": 1}, "to": {"y": 2, "x": 1}, "direction": "dir-down"}, 
    {"action": "move",            "player": "player-01", "from": {"y": 2, "y": 1}, "to": {"y": 2, "x": 0}, "direction": "dir-left"}, 
    {"action": "push-to-goal",    "player": "player-01", "stone": "stone-00", "player-start-pos": {"y": 2, "x": 0}, "start-pos": {"y": 1, "x": 0}, "end-pos": {"y": 0, "x": 0}, "direction": "dir-up"}, 
    {"action": "move",            "player": "player-01", "from": {"y": 1, "x": 0}, "to": {"y": 1, "x": 1}, "direction": "dir-right"}, 
    {"action": "move",            "player": "player-01", "from": {"y": 1, "x": 1}, "to": {"y": 0, "x": 1}, "direction": "dir-up"}
  ]
}
```

## Exploration Solver

The python server can also call the planner to solve a new PDDL problem, exploration. Given a map, it is the problem of visiting all the cells within the map.
The agent (robot) won't return in any prefixed cell. The server listen for POST requests at the URL `ip:port/exploration_problem`. In the payload of the request, a json needs to be provided. Furthermore, be sure to add to the header of the request the "Content-Type" "application/json". The server will reply with a 200 response containing a payload with the plan computed.

In the following we will assume the following problem (made thanks by the website [sokoban-editor](https://deificx.github.io/sokoban-editor/) ):

![exploration example](https://github.com/STB1019/Robotieee/blob/master/images/explorationProblem.png "Exploration example")

The yellow circle is the robot, the crate is the block which can't be moved.

### JSON request format

For solving a exploration problem use the following json:

```
{
  "version": "1.0",
  "world": {
    "rows": 3,
    "columns": 3,
    "cells": [
      { "y": 0, "x": 0, "entities": "R"},
      { "y": 0, "x": 1, "entities": ""},
      { "y": 0, "x": 2, "entities": "U"},
      { "y": 1, "x": 0, "entities": ""},
      { "y": 1, "x": 1, "entities": "B"},
      { "y": 1, "x": 2, "entities": ""},
      { "y": 2, "x": 0, "entities": ""},
      { "y": 2, "x": 1, "entities": ""},
      { "y": 2, "x": 2, "entities": ""}
    ]
  }
}
```

where in the string field *entities*:

 - *B* stands for *block*;
 - *R* stands for *robot*;
 - *G* stands for *goal*;
 - *U* stands for *untraversable*;
 - *D* stands for *docking station*;
 - *V* stands for a cell which has already been visited by the robot;

Multiple objects can be within the same cell.

### JSON reply

The server will reply with a json formatted like this:

```
{
  "version": "1.0", 
  "actions": [
    {"action": "move", "player": "player-01", "from": {"y": 0, "x": 0}, "to": {"y": 1, "x": 0}, "direction": "dir-down"}, 
    {"action": "move", "player": "player-01", "from": {"y": 1, "x": 0}, "to": {"y": 2, "x": 0}, "direction": "dir-down"}, 
    {"action": "move", "player": "player-01", "from": {"y": 2, "x": 0}, "to": {"y": 2, "x": 1}, "direction": "dir-right"}, 
    {"action": "move", "player": "player-01", "from": {"y": 2, "x": 1}, "to": {"y": 2, "x": 2}, "direction": "dir-right"}, 
    {"action": "move", "player": "player-01", "from": {"y": 2, "x": 2}, "to": {"y": 1, "x": 2}, "direction": "dir-up"}, 
    {"action": "move", "player": "player-01", "from": {"y": 1, "x": 2}, "to": {"y": 2, "x": 2}, "direction": "dir-down"}, 
    {"action": "move", "player": "player-01", "from": {"y": 2, "x": 2}, "to": {"y": 2, "x": 1}, "direction": "dir-left"}, 
    {"action": "move", "player": "player-01", "from": {"y": 2, "x": 1}, "to": {"y": 2, "x": 0}, "direction": "dir-left"}, 
    {"action": "move", "player": "player-01", "from": {"y": 2, "x": 0}, "to": {"y": 1, "x": 0}, "direction": "dir-up"}, 
    {"action": "move", "player": "player-01", "from": {"y": 1, "x": 0}, "to": {"y": 0, "x": 0}, "direction": "dir-up"}, 
    {"action": "move", "player": "player-01", "from": {"y": 0, "x": 0}, "to": {"y": 0, "x": 1}, "direction": "dir-right"}
  ]
}
```


# Middelware Blue

Middleware to send data between Bluetooth (App-Robot) and Wifi (App-Server).

_coming soon_

# References

 - [Sumo Robot manual](https://www.pololu.com/docs/0J63);
 - [Zumo 32U4 API library](http://pololu.github.io/zumo-32u4-arduino-library/)
 - [Create a custom Arduino library tutorial](https://www.arduino.cc/en/Hacking/LibraryTutorial);
