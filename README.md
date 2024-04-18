# stalcraft-auction-parser
To build and run application you need to install cmake.

```
git clone --recursive https://github.com/AlexDyakonov/stalcraft-auction-parser.git
```

Create build directory:
```
mkdir build && cd build
```

Then build project:
```
cmake ..
```

```
make
```

Before using, make sure that Clickhouse is on ( `docker-compose up --build -d`)

Ready to use!

```
./stalcraft-auction-parser --help
```
