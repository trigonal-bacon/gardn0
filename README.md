# gardn

This is a scrapped version of the gardn project, inspired by previous work on another project. It contains a templated entity generator, partial ECS, and a custom UI rendering library. All code is written in vanilla C++ with no frameworks or external engines.

# Requirements

Requires Emscripten and uWebSockets.

build uWebSockets. For installation instructions see https://github.com/uNetworking/uWebSockets.git.

To compile and run the server, run
```
cd Server
mkdir build
cd build
cmake ..
make
./gardn-server
```

To compile the client, run
```
cd Client
mkdir build
cd build
cmake ..
make
```
then move the js and wasm file from build to public.

The clientside website can be served using any http server. 


