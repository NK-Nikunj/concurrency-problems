# Concurrency Synchronization Issues

This repository contains solutions to some common synchronization issues.

## Minimum Requirements

The source code is written in standard C++14 standards and uses HPX 
library for concurrency. To build the source code, you will require to 
have HPX installed.

### Installing HPX

You will require to install pre-requisites to HPX to build HPX (unless 
you're on Fedora, in which case you can install using `yum install hpx`).

1. Type the following command to install the pre-requisites (on any standard 
debian based OS):
```
sudo apt install libgoogle-perftools-dev hwloc libboost-dev
```

2. Clone the HPX repository and create a build directory
```
git clone https://github.com/STEllAR-GROUP/hpx hpx
cd hpx
mkdir build && cd build
```

3. Build the HPX repository
```
cmake -DHPX_WITH_EXAMPLES=OFF ..
make -j$(nproc)
make install
```

## Running examples

Now that you have installed HPX, you can build and run the examples of this 
repository. To run any of the examples, follow the build instructions given 
in the respective directories.