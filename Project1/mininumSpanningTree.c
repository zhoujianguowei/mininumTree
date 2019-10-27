#include<omp.h>
#include<stdio.h>
#include<malloc.h>
#include<time.h>       //产生随机数需要
#include<stdlib.h>
#define INFINITY 65535 //节点不可达
#define N 2000    //图的节点个数

void initGraph();
int createMininumTree();
int* getMininumArray(int, int[]);
void printGraph();
void printMininumTree();
void initOriginalData();
void copyOriginalData();

//采用邻接矩阵的形式存储图,temp只是保存初始化时候的数据
int graph[N][N], originalGraph[N][N];
int c[N], originalC[N];       //最小生成树的节点集合
int t[N], originalT[N];       //剩余没有加入最小生成树的节点
int mininumTreeIndex[N - 1][3];  //最小生成树的路径个数，1,2,3列分别代表关系矩阵行、列、权值。
float efficiency;       //加速比


/** 使用prime算法求最小生成树，其中可以提高程序的效率，即并行性，
就是在计算最小权值时候，同时求c中各节点到t中节点的各个最小值，然后比较。
**/
int main()
{
	//并行线程数
	int i = 1;
	unsigned long t1, t2;
	int threadsNums = omp_get_num_procs();
	printf("本机逻辑处理器个数为%d\n\n", threadsNums);
	unsigned int singleThreadRunTime = 0;  //单线程运行时间

	initGraph();
	//初始化原始数据
	initOriginalData();
	for (; i <= threadsNums; i++)
	{
		copyOriginalData();
		printf("并行线程个数为：%d\n", i);
		t1 = clock();
		if (createMininumTree(i))
		{
			t2 = clock();
			printf("%d个节点建立的最小生成树所花费时间为：%d毫秒\t", N, t2 - t1);
			if (i == 1)
			{
				singleThreadRunTime = t2 - t1;
			}
			else
			{
				efficiency = singleThreadRunTime*1.0f / (t2 - t1);
				printf("加速比为%f", efficiency);

			}


			//printf("最小生成树结构：\n");
			//printMininumTree();
		}
		else
		{
			printf("无法产生最小生成树\n");
		}
		printf("\n\n");
	}

	return 0;
}
/*
生成最小生成树,返回0代表不存在最小生成树
代表存在最小生成树
参数 parallelThreadsNum代表并行线程数
*/
int createMininumTree(int parallelThreadsNums)
{
	int visitedCount = 1;          //已经添加到数组c中的节点索引
	int i = 0, j = 0, k = 0;
	int minValue = INFINITY, minRow, minCol;
	int flag = 0;                    //是否能够生成最小生成树的标记
	c[0] = 0;                        //将第一个节点加入c中，同时从t中删除
	t[0] = -1;                       //t[k]=-1代表第索引为k的节点已经从t中删除
	int threadCountFlag = 0;   //并行区域并行线程个数标记，只在第一次运行时候输出

	//一次添加一个节点到c，同时从v中删除一个节点
	for (i = 0; i < N; i++)
	{
		flag = 0;
		minValue = INFINITY;
		minRow = INFINITY;
		minCol = INFINITY;
		/**
		得到从c到t权值最小值
		**/
		//这儿可以动态改变线程个数
#pragma omp parallel num_threads(parallelThreadsNums)
		{
			int *minArrayPointer;          //整型数组指针
#pragma omp for schedule(dynamic,2)
			for (j = 0; j < visitedCount; j++)
			{
				//#pragma omp flush
				//				if (!threadCountFlag)
				//				{
				//					threadCountFlag = 1;
				//					printf("并行区域使用线程个数%d\n", omp_get_num_threads());
				//				}
#pragma omp flush
				minArrayPointer = getMininumArray(c[j], t);

#pragma omp critical
				if (*(minArrayPointer + 2) < minValue)
				{
					minRow = *(minArrayPointer);
					minCol = *(minArrayPointer + 1);
					minValue = *(minArrayPointer + 2);
					flag = 1;
				}
			}

		}
		/**
		该图有孤立连通图,必须等待for循环语句执行完成之后才能判断
		*/
		if (flag == 0)
			break;
		else
		{
			mininumTreeIndex[visitedCount - 1][0] = minRow;
			mininumTreeIndex[visitedCount - 1][1] = minCol;
			mininumTreeIndex[visitedCount - 1][2] = minValue;
			c[visitedCount++] = minCol;
			t[minCol] = -1;

		}

	}

	if (visitedCount == N)
		return 1;
	else
		return 0;
}

/**
得到节点i到为访问节点集合t中的最小权值的行、列、值,这儿可以进行并行设计

**/
int* getMininumArray(int i, int unVisited[N])
{
	int minArray[3];
	int minCol = INFINITY, minRow = INFINITY, minValue = INFINITY;
	//printf("&minCol=%d\t&minArray=%d", &minCol,minArray);
	int j, *minPointer;
	//printf("线程%d\n", omp_get_thread_num());
	for (j = 0; j < N; j++)
	{
		if (unVisited[j] == -1)
			continue;
		if (graph[i][j] < minValue)
		{
			minRow = i;
			minCol = j;
			minValue = graph[i][j];
		}

	}



	//	printf("线程id=%d\n", omp_get_thread_num());
	//printf("internal minvalue=%d\t", minValue);

	minArray[0] = minRow;
	minArray[1] = minCol;
	minArray[2] = minValue;
	minPointer = minArray;
	//#pragma omp critical
	return minPointer;
}


/**初始化数据,使用随机数
**/
void initGraph()
{
	int i, j;
	srand(time(NULL));    //以当前时间作为随机数种子
	for (i = 0; i < N; i++)
	{
		c[i] = -1;
		t[i] = i;
		for (j = 0; j < N; j++)
		{

			//节点自身不可达
			if (i == j)
			{
				graph[i][j] = INFINITY;
				continue;
			}
			graph[i][j] = rand() % N + 1;
		}
	}

	/**	for(i=0;i<N;i++)
	{
	for(j=0;j<N;j++)
	if(i==0||i==1||j==0||j==1)
	graph[i][j]=INFINITY;

	}
	graph[1][0]=3;
	graph[0][1]=20;
	**/
	// printf("初始化图形结构*************:\n\n");
	//printGraph();



}

/**
打印最小生成树结构*/
void printMininumTree()
{
	int i = 0;
	for (; i < N - 1; i++)
	{

		printf("索引%-4d索引列%-4d权值%-10d", mininumTreeIndex[i][0], mininumTreeIndex[i][1], mininumTreeIndex[i][2]);
		printf("\n");
	}
}
/**
打印图形结构
*/
void printGraph()
{
	int i, j;
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			if (graph[i][j] == INFINITY)
			{
				printf("%-10s", "INFINITY");
			}
			else
				printf("%-10d", graph[i][j]);
		}
		printf("\n");
	}
}
/*
初始化原始数据，保存原始数据
*/
void initOriginalData()
{
	int i = 0, j = 0;
	for (; i < N; i++)
		for (j = 0; j < N; j++)
			originalGraph[i][j] = graph[i][j];
	for (i = 0; i < N; i++)
	{
		originalC[i] = 0;
		originalT[i] = 0;
	}




}
/*
数据进行初始化，每当并行线程数量改变时候调用，传值调用
*/
void copyOriginalData()
{
	int i, j;
	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
			graph[i][j] = originalGraph[i][j];

	for (i = 0; i < N; i++)
	{
		c[i] = originalC[i];
		t[i] = originalT[i];
	}
}



