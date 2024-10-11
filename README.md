# Naive_Trader
This a C++ based naive trading tools. Currently only support CTP APIs, and will keep updating to support other trading platforms.

### ToDo list
- [x] Improve the current code repo (On going, there will always be bugs that I haven't noticed.)
- [ ] Finish Logging logics into trade and market apis (On going...)
- [ ] Implement simulator and runtime engine
- [ ] Learn and implement a statistical arbitrage strategy


### Folder Structure
```
Naive_trader
├── api/
├── docs/
└── src
    ├── apiadapter/
        └── market/
        └── src/
├── config/
├── example/
├── include/
├── loger/
├── utils/
```

### Some notes
Currently, this repo only supports CTP APIs (will include other platform in the future). The code are developed with vscode remote, some of the configurations are not provided here. 

Here are some hints that might be useful for coding and debuging c++ projects with vscode. When you need to add external .so share libs, modify task.json, add path to "args"
```
"args": [
    // ....
    "-L/path_to_your_lib",
    "-lyour_lib_name",
],
```
Note that after "-l", "lib" should not appear. 


### Acknowledgements
I learned and benefited a lot from [Zou's](https://gitee.com/lightning-trader/lightning-futures/) share of the ligntening trading tools, and learned lock/wait free synchronization algorithms and data structures from [1024cores](https://www.1024cores.net/home). The use of ringbuffer are from [Jan-ringbuffer](https://github.com/jnk0le/Ring-Buffer/blob/master/ringbuffer.hpp),  and log packages are from [Karthik-NanoLog](https://github.com/Iyengar111/NanoLog/tree/master). Thanks to their efforts and contributions.
