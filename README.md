# Loki Project

The **Loki Project** is an implementation of a client aimed at recreating the classic experience of one of the most
iconic MMORPGs. This project is designed to maintain high fidelity to the original game while modernizing its internals
for better performance and maintainability.

## Requirements

- CMake
- C++ Compiler
    - GCC 12 and later
    - Clang 15 and later
    - Visual Studio 2019 version 16.8 and later

## How to Build

```bash
cmake -B build && cmake --build build --target Loki -j8
```

## Third Party Libraries

The Loki Project relies on the following external libraries and frameworks:

- `glfw3`
- `glew`
- `imgui`
- `glm`
- `spdlog`
- `cli11`
- `mimalloc`
- `stormlib`
- `sockpp`
- `glob`
- `libassert`
- `openssl`
- `pfr`

## Special Thanks

A special thanks to the TrinityCore project, whose work has been invaluable in making this project possible. Their
dedication to the community and commitment to open-source software is deeply appreciated.

## Contributing

Contributions are welcome! If you would like to help improve the Loki Project, please fork the repository and submit a
pull request. Please ensure your code adheres to the project's coding standards.

## Copyright

This project is licensed under the GPL v2 License. See the LICENSE file for details.
