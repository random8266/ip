# IP
This program checks whether provided IP address is among stored list of IP addresses.

# Build
IP project uses CMake for building. During the build process **build** directory is created. The executable is called **ip** and it's located inside build directory. 

## Build Steps
- remove old build directory: rm -rf build
- create **build** drectory: mkdir build 
- enter **build** directory: cd build
- call cmake: cmake ..
- call make: make

## Build Script
- call build script: ip_build.sh
