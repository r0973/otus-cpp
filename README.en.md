# Duplicate File Detection by Content

## Project Description
A utility for finding duplicate files based on block-by-block hashing. The project uses CMake and supports multiple hashing algorithms (CRC32, MD5).

## Project Structure
The project is organized according to standard C++/CMake practices. The source code has the following structure:  
```
.
├── CMakeLists.txt
├── Doxyfile
├── README.md
├── @googletest/
├── scripts
│   ├── test_crc32.sh
│   ├── test_funcs.sh
│   ├── test_md5.sh
│   ├── test_nested.sh
│   └── test_runner.sh
├── src
│   ├── CMakeLists.txt
│   ├── Comparison.cpp
│   ├── Comparison.h
│   ├── Config.cpp
│   ├── Config.h
│   ├── FileScanner.cpp
│   ├── FileScanner.h
│   ├── HashManager.cpp
│   ├── HashManager.h
│   ├── lib_version.cpp
│   ├── lib_version.h
│   ├── main.cpp
│   └── version.h.in
└── unit_tests
    ├── CMakeLists.txt
    ├── boost
    │   ├── CMakeLists.txt
    │   ├── testComparison.cpp
    │   ├── testFileScanner.cpp
    │   ├── testHashManager.cpp
    │   └── test_version.cpp
    └── gtest
        ├── CMakeLists.txt
        ├── test_main_gtest.cpp
        └── test_version.cpp
```

## Building the Project with CMake
To build the project, you will need a C++17 compiler (e.g., GCC 9+ or Clang 9+), CMake version 3.12 or higher, and the Boost libraries. 

### 1. Cloning the Repository (including submodules)

`git clone https://github.com/r0973/otus-cpp.git`  
`cd otus-cpp`  
`git checkout feature/duplicate_files_filter`

### 2. Creating the Build Directory

`mkdir build`  
`cd build`  

### 3. Configuring the Project

`cmake ..`  

#### ОOptionally: You can disable the build of Google Test or Boost tests by adding parameters during configuration:

`cmake .. -DWITH_GOOGLE_TEST=OFF -DWITH_BOOST_TEST=OFF`  

### 4. Building the Project
`cmake --build .`  

### 5. Installing the Project (Copying artifacts to build/install/)

`cmake --install .`  

### 6. Running and Testing
After installation, all necessary files are located in the `build/install/` directory.

#### 6.1. Project Structure After Build
```
build/install/
├── bin/
│   ├── bayan              # Main utility
│   ├── test_runner.sh     # Main test script
│   ├── test_crc32.sh      # CRC32 algorithm test
│   ├── test_md5.sh        # MD5 algorithm test
│   ├── test_nested.sh     # Nested directories test
│   └── test_funcs.sh      # Common test functions
└── lib/
    └── libbayan_logic.so  # Dynamic library
```

#### 6.2. Manual Run
You can run the bayan client application manually from the build/install/bin/ directory. Don't forget to set the LD_LIBRARY_PATH environment variable so the system knows where to find `libbayan_logic.so`:

`cd build/install/bin/`  
`export LD_LIBRARY_PATH=../lib:$LD_LIBRARY_PATH`  
`./bayan -i "/path/to/folder" -e "/path/to/exclude_dir" --hash md5`

####  Using Automated Test Scripts
The project generates `test_runner.sh` scripts in the installation directory. This script tests the utility and automatically sets the `LD_LIBRARY_PATH` environment variable. Run it from the build directory `build`:

`cd build`  
`./install/bin/test_runner.sh`  

or from any other directory by specifying the relative path to the script.

#### Available Tests
```
test_crc32.sh # CRC32 algorithm testing 
recursive duplicate search  
directory exclusion  
different scan levels

test_md5.sh # MD5 algorithm testing 
comparison with CRC32 results  
edge case testing  

test_nested.sh # Nested directories testing
deeply nested structures
symbolic links
```

#### 6.3. Running Unit Tests
If you built the tests, you can run them from the build directory:

`cd build/install/bin` 
`./unit_tests_boost`

#### 6.4. Running Tests via ctest
`ctest`