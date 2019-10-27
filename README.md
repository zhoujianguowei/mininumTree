# mininumTree
使用openmp求解最小生成树，最小生成树算法采用的是prime算法。模拟数据是随机生成的2000个节点所构成的图结构。根据单机的线程个数来计算多线程下提高算法的效率，使用openmp尽管能够提高程序的效率，但是需要注意**不是算法的每一个步骤都可以并行执行**，难点在于区分算法可以并行执行的部分。

程序截图如下：(我的本机是四核八线程i7-4710MQ处理器)

![程序运行截图](https://github.com/zhoujianguowei/mininumTree/raw/master/img/1.png)