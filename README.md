# Nealog

Nealog is a logging framework supporting C++17. It is developed to support multithreading out of the box and can be used headeronly or build as a static library.

This is made possible by structuring the code in header files and only including those inside the cpp files used for compiling for static libs (technique shamelessly copied from [spdlog](https://github.com/gabime/spdlog)).


## Features
| Core                        |         |
|:----------------------------|:--------|
| Configuration through file  | planned |
| Specify log output format   | planned |

| Sinks             |         |
|:------------------|:--------|
| StdOut            | done    |
| std::stringstream | done    |
| File              | planned |
| TCP               | planned |
| UDP               | planned |

## Compile

Neolog uses CMake as build generation tool.

```sh
cmake -G Ninja -B build
```

For using as headeronly library build with Cmake option `NEALOG_HEADERONLY=ON` which is turned off by default.

```sh
cmake -G Ninja -DNEALOG_HEADERONLY=ON -B build
```

## Link against

To link against the static lib use the target `nealog`.
```cmake
  add_executable(myproject <SOURCES>)
  target_link_libraries(myproject PUBLIC nealog)
```

To link against the headeronly lib use the target `nealog::headeronly`.
```cmake
  add_executable(myproject <SOURCES>)
  target_link_libraries(myproject PUBLIC nealog::headeronly)
```


## Alternatives
If you're searching for a stable logging framework which supports C++11 take a look at [spdlog](https://github.com/gabime/spdlog).
