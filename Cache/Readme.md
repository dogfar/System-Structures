4个文件all.cpp，4way.cpp，8way.cpp和direct.cpp分别对应组织方式为全相联映射、4路组映射、8路组映射和直接映射。其中参数的定义如下所示：

宏定义部分，Block_Byte代表块的大小，如果是3则代表8B，如果是5则代表32B，如果是6则代表64B；

```
#define Block_Byte 3 // 3, 5, 6 -> 8B, 32B, 64B
```

对于8way.cpp的情况，还需要选择替换策略、是否写回、是否写分配。参数定义在main函数的第二段：

```
    int policy = LRU;
    bool WriteBack = true;
    bool WriteAllocate = true;
```

policy的选项为LRU，TREE和RAND，分别对应下面的宏定义：

```
#define LRU 0
#define TREE 1
#define RAND 2
```

WriteBack为true则为写回，否则为写直达；WriteAllocate为true则为写分配，否则为写不分配。

在指定上述参数后编译运行即可。直接make即可生成4个cpp文件对应的可执行文件。

4个文件运行后，可以输入astar或bzip2或mcf或perlbench来决定读入哪个trace。8way.cpp会在当前目录下生成以读入trace名命名的log文件。