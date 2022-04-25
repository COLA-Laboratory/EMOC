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

  






