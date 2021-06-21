## Building:
Should be easy to build as usial CMake project  
Use `BUILD_TESTS` option to turn on/off tests building (`-DBUILD_TESTS=<ON/OFF>` during build configuration)  
`BUILD_TESTS` turned on by default

## Installation:
### Installation as `pkg-config` package:
```
cd <path to build directory>
sudo make install
```

## Demo application:
[link](https://github.com/thequickf/polygon-decomposition-demo)  
Note that demo application searches for this library as `pkg-config` package
