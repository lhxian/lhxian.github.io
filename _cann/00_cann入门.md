---
title: 00_cann入门
layout: post
---
# cann入门
本篇笔记涉及到的是传统的**AscendC**算子编程，也就是**SIMT**编程，更新的**950**芯片上已经有更新的**SIMD**编程，但是学习**910**的一些编程范式对学习cann也有意义。

## 静态Tensor编程
静态Tensor编程即需要自己管理**UB**等**LocalTensor**管理的区域，以vector上的操作为例，一般来说**UB**空间的大小为$192KB$，实际可用的空间为**192KB -256B**，有**256**的空间为系统使用。
使用静态Tensor编程时，需要自己管理数据和计算单元之间的同步。
首先需要明确的是，**AscendC**编写的代码，编程之后执行时，由**Scalar**单元进行指令发射(issue)，也就是将kernel的代码按照指令的种类分发到不同的计算单元去执行，例如管理内存数据运输的MTE2,MTE3单元，vector计算单元等，不同的计算单元执行是异步的，也就是不会检查算子中的数据依赖，只要有指令被**Scalar**单元派发到达，那么对应的计算单元会执行，有时候一个计算单元中的指令操作的数据需要依赖另外一个单元的一些操作完成才能正确进行，例如将数据从Global Memory搬运到UB中进行vector计算，这时候vector单元需要等待mte2单元数据搬运完成，这时候需要使用一对信号，可以用两个点的通信来理解，MTE2对应PIPE_MTE2，vector单元对应PIPE_V，在AscendC代码中使用一对Set/Wait Flag函数来等待相应的标志。
需要注意的是，一对Set/Wait很多时候是相邻出现在代码中的，但是会执行在不同的单元部件上，上面的例子，vector等待mte2数据就绪，WaitFlag执行的时候是在vector单元上的，所以会阻塞vector单元知道通信的部件调用SetFlag来进行通知。
如果同一个单元部件，出现数据依赖，例如vector中两个张量的数据冒险，对于单元内部的同步，使用**PipeBarrier**来进行阻塞等待，可以理解为**PipeBarrier**执行的时候，需要等待之前代码完全执行之后（一般是针对读写命令），后面的指令才可能执行，推测可能是因为同一个单元内容可能存在指令的乱序执行，或者单元内容也被分为了一个异步的字单元系统，不同类型的指令也可能是异步执行在不同小单元上。

### 一些需要注意的地方
在使用静态tensor编程的时候，**LocalTensor**的构造函数中，UB等local memory的偏移量最好不要出现重叠。还有在一些vector计算中，如果dst local tensor和src local tensor是同一个对象的话，可能会出现写入的问题，如下面的写法：
```cpp
LocalTensor<T> a,b;
AscendC::Add(a,a,b,4);
```
dst和src中的一个对象一样，测试的时候发现实际上tensor a的数据没有被更新。

## 查找设备信息
[参考文章](https://blog.csdn.net/qq_40949938/article/details/148796485)
到安装cann的目录(ASCEND_HOME_PATH)中，进入**aarch64-linux/data/platform_config**即可找到对应型号设备信息。
