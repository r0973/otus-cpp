# Async Bulk Processor Project

## Project Description

This project implements a utility for processing commands received as a data stream. It is decomposed into two parts:

1.  **`libasync.so` library**: The command processing core, implemented using multithreading (Observer and Task Dispatcher patterns).
2.  **`bulk` client application**: A console utility that uses the library to read commands from standard input (`stdin`) and perform asynchronous logging to the console and files.

The project supports parallel processing of several independent command contexts via the library's C-interface (`connect`/`receive`/`disconnect`).

## Project Structure

The project is organized according to standard C++/CMake practices. The source code is divided into a base (single-threaded) version and multithreaded extensions:

```
.
├── include/    # Header Files
│ ├── async/    # Public API of the library (async.h)
│ ├── base/     # Header files of the base (single-threaded) version
│ └── mt/       # Header files of the multithreaded extensions
├── src/        # Implementation Files (.cpp)
│ ├── async/    # Implementation of the public API
│ ├── base/     # Implementation of the base (single-threaded) version
│ └── mt/       # Implementation of the multithreaded extensions
├── unit_tests/ # Unit Tests
│ ├── boost/    # Tests using Boost.Test
│ └── gtest/    # Tests using Google Test
└── googletest/ # Google Test Submodule
```  
## Building the Project with CMake

To build the project, you need a C++17 compiler (e.g., GCC 9+ or Clang 9+), CMake version 3.12 or higher, and Boost libraries (if Boost tests are included).

### 1. Cloning the Repository (including submodules)

`git clone https://github.com/r0973/otus-cpp.git`  
`cd otus-cpp`  
`git checkout feature/asyn_bulk_parser`

### 2. Creating the Build Directory

It is recommended to use a separate `build` directory for build artifacts.

`mkdir build`  
`cd build`  

### 3. Configuring the Project

`cmake ..`  

#### Optional: You can disable the building of Google Test or Boost tests by adding parameters during configuration:

`cmake .. -DWITH_GOOGLE_TEST=OFF -DWITH_BOOST_TEST=OFF`  

### 4. Building the Project
`cmake --build .`  

### 5. Installing the Project (Copying Artifacts to build/install/)
This step copies the compiled files (`bulk`, `libasync.so`, `run_test.sh`) to the target directory `build/install/bin` and `build/install/lib`.

`cmake --install .`  

### 6. Running and Testing

After installation, all necessary files are located in the `build/install/` directory.

#### 6.1. Running Manually
You can run the `bulk` client application manually from the `build/install/bin` directory. Do not forget to set the `LD_LIBRARY_PATH` environment variable so the system knows where to find `libasync.so`:

`cd build/install/bin/`  
`export LD_LIBRARY_PATH=../lib:$LD_LIBRARY_PATH`  
`./bulk 3`

#### 6.2. Using the Automatic Test Script
The project generates a `run_test.sh` script in the installation directory. This script runs two independent clients in parallel and automatically handles the `LD_LIBRARY_PATH`.
Run it from the `build/install/bin/` directory:

`cd build/install/bin/`  
`./run_test.sh`  

The script's output will show console logs and the contents of the generated files (`bulk*.log`), confirming the asynchronous processing is working.

#### 6.3. Running Unit Tests
If you built the tests, you can run them from the build directory:

`cd build/`  

#### 6.4. Running tests via ctest
`ctest`