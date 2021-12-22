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





## TODO List

**整体方面：**

- [x] Code Review 对整体代码做一个回顾，是否可以改进，局部结构是否可以改善，细节部分代码是否可以更加简洁。
- [x] 算法的准确性及其效率的review，从老的算法包移植过来的算法不不知道是否正确。
- [ ] Indicator的计算，PF可否用公式计算，不用文件读写，以及新的indicator的添加，接口上的real_pop_num问题。
- [ ] Linux的移植
- [ ] ...

**功能方面：**

- [x] Table样式， note：看了一遍demo code，没有这种合并单元格的效果，可能可以提供过nested table来hack一个，最好自己抽象出来一个Experiment的Table类
- [x] 算法和问题类别
- [x] 画图的canvas大小，位置，让用户指定
- [x] EMOC不同状态对应的有些窗口要Disable，防止用户误操作
- [x] 一些metric计算之后的cache，防止重复计算，Note：现在SingleThreadResult用了unordered_map，MultiThreadResult也可以考虑相应更改一下，不要用一个额外的std::vector<bool>来表示是不是存了，直接用哈希表就行，**算了还是不用了，因为用了要提供一个额外的参数表示一共有多少run，不如直接size来的简洁明了**
- [x] SetUIPanelCurrentEvaluation()和CheckPauseAndStop()在指定条件下再调用（在Alogrithm的IsTermination里面）。包括EMOC各种运行状态的正确设置。
- [x] 提供一些提示，让用户可以知道Popup需要右键点击才会出现，或者引入icon button
- [x] Experiment下early stop之后后序任务不应再执行，应该直接停止for循环
- [ ] 画图和实验模块给一个简略的进度条。 note：不怎么重要，先不急
- [ ] 各个Panel下window的大小及其位置的事先固定
- [x] 画图文件存储的优化，和本身种群的Track不要重复。另外都画图用脚本，不要有的是指令，有的是命令，保持统一
- [x] 科学计数法显示，以及MEDIAN,IQR等等
- [x] 最佳结果标bold（添加额外字体）
- [x] 实验模块的假设检验统计测试
- [x] 实验结果table的导出功能
- [x] 参数合法性检验，以及对应的警告
- [ ] 统计测试的比较算法问题
- [x] 命令行调用设计 cxxopt还不错可以用一下
- [ ] python调用接口的重新设计
- [x] hv的计算，好了但没完全好
- [ ] 函数和问题的注册，可能要用到反射？之后再看，现在先搞米哈游的作业
- [ ] 算法自身的参数接口 （可以改，但有些麻烦，暂时没必要）
- [ ] 实验模块的画图分析  （暂时不知道画什么，不着急搞）
- [ ] ...

**Bug Fix：** 

- [x] ImGUI在某些条件下会出现Begin与End不对等的情况
- [x] WFG多线程下崩溃， Fixed：全局变量的访问问题
- [ ] Clock计时在多线程条件下表现不正确
- [ ] 实验模块问题参数设置在收起header之后对popup选中delete的时有些奇怪的表现（不是参数错误，是不正常的收起，可能是ImGui自己的问题，初步猜测是ImGui的ID问题，因为我是根据index设置id的，删除一个只会，后面可能会和删除的那个id一样，继承了之前的状态？這個bug問題不大,可以暂时不用管）
- [x] test module plot判断plot metric name的时候不要直接比指针
- [x] Test module下如果连续画同一个pop的metric，plot会不work。Fixed:因为同一个pop的metric存的file的name是一样，会出现data invalid（因为可能要画的时候，还正在写），改为了用selected_run的index来命名文件。
- [x] 运行结果的description显示不正确。 Fixed：指针在vector扩容时会变成野指针
- [x] Popup之后点击delete button后数组越界。Fixed：如果删除了，在当前帧不再继续显示
- [x] 单线程运行结果在table中显示不正确。 Fixed：用**引用**返回时会因为vector扩容出现内存泄露
- [ ] ...





