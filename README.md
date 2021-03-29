# EMOC

EMOC is a multi-objective optimization library written in c++11 which involves some basic evolutionary algorithms (now is 10+). 



## Build

The CMakeLists.txt file is provided to build project.

For **windows**, we recommend  use visual studio 2017 or later version to build project:

```c++
mkdir build
cd build
cmake -G "Visual Studio 15 2017" ..
```

then open the generated .sln file and click build button, you can find the .exe file has already in the project directory. 



For **linux**, compile the project with following command:

```c++
mkdir build
cd build
cmake ..
make -j4
```

the compiled executable file EMOC will appear in the project directory.

  

## Usage

When you have built the project, you can run the executable file directly with some default parameters settings, e.g.:

for windows:

```c++
// in the project directory
EMOC.exe
```

for linux:

```c++
// in the project directory
./EMOC
```



Or you can set some basic parameters with parameter value pair, e.g.:

for windows:

```c++
// in the project directory
EMOC.exe -algorithm moead -problem zdt3 -N 300 -D 30
```

for linux:

```c++
// in the project directory
./EMOC -algorithm moeadde -problem zdt3 -N 300 -D 30 -evaluation 300000
```

All the optional parameters are listed bellow:

| Parameter name | Description                        |
| -------------- | ---------------------------------- |
| -algorithm     | Algorithm name                     |
| -problem       | Problem name                       |
| -N             | Population size                    |
| -D             | Number of variables                |
| -M             | Number of objectives               |
| -evaluation    | Number of evaluations              |
| -run           | Run number                         |
| -save          | The interval of saving populations |
| -thread        | Number of thread                   |



Another way to use EMOC is setting the parameters within a config file, and run with '-file filepath'

```c++
./EMOC -file ./src/config/config.txt
```

there is an example of config file in the corresponding directory.