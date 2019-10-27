#include<stdio.h>
#include<malloc.h>
#include<time.h>       //产生随机数需要
#include<stdlib.h>
#include<omp.h>
#include<iostream>
#define INFINITY 65535 //节点不可达
#define N 6   //图的节点个数

void initGraph();
int createMininumTree();
int* getMininumArray(int, int[]);


//采用邻接矩阵的形式存储图
int graph[N][N];
int c[N];       //最小生成树的节点集合
int t[N];       //剩余没有加入最小生成树的节点
int mininumTreeIndex[N - 1][3];   //最小生成树的路径个数，1,2,3列分别代表关系矩阵行、列、权值。

/** 使用prime算法求最小生成树，其中可以提高程序的效率，即并行性，
就是在计算最小权值时候，同时求c中各节点到t中节点的各个最小值，然后比较。
**/

int main()
{
	int i = 0, j = 0;
	int coreNum = opm_get_num_procs();  //获得处理器的个数
	printf("处理器个数%d\n", coreNum);
#pragma omp parallel for
	for (i = 1; i < 11; i++)
		printf("%d\t", i);


	/*initGraph();
	if (createMininumTree())
	{
	printf("最小生成树结构：\n");
	for (; i<N - 1; i++)
	{

	printf("索引%-4d索引列%-4d权值%-10d", mininumTreeIndex[i][0], mininumTreeIndex[i][1], mininumTreeIndex[i][2]);
	printf("\n");
	}
	}
	else
	{
	printf("无法产生最小生成树\n");
	}*/
	return 0;
}
/*
生成最小生成树,返回0代表不存在最小生成树
代表存在最小生成树
*/
int createMininumTree()
{
	int visitedCount = 1;          //已经添加到数组c中的节点索引
	int i = 0, j = 0, k = 0;
	int minValue = INFINITY, minRow, minCol;
	int *minArrayPointer;          //整型数组指针
	int flag = 0;                    //是否能够生成最小生成树的标记
	c[0] = 0;                        //将第一个节点加入c中，同时从t中删除
	t[0] = -1;

	//一次添加一个节点到c，同时从v中删除一个节点
	for (i = 0; i<N; i++)
	{
		flag = 0;
		minValue = INFINITY;
		minRow = INFINITY;
		minCol = INFINITY;
		/**
		得到从c到t权值最小值
		**/
		for (j = 0; j<visitedCount; j++)
		{
			minArrayPointer = getMininumArray(c[j], t);
			if (*(minArrayPointer + 2)<minValue)
			{
				minRow = *(minArrayPointer);
				minCol = *(minArrayPointer + 1);
				minValue = *(minArrayPointer + 2);
				flag = 1;
			}
		}
		/**
		该图有孤立连通图
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
	int j, *minPointer;

	for (j = 0; j<N; j++)
	{
		if (unVisited[j] == -1)
			continue;
		if (graph[i][j]<minValue)
		{
			minRow = i;
			minCol = j;
			minValue = graph[i][j];
		}
	}
	minArray[0] = minRow;
	minArray[1] = minCol;
	minArray[2] = minValue;
	minPointer = minArray;
	return minPointer;
}


/**初始化数据,使用随机数
**/
void initGraph()
{
	int i, j;
	srand(time(NULL));    //以当前时间作为随机数种子
	for (i = 0; i<N; i++)
	{
		c[i] = -1;
		t[i] = i;
		for (j = 0; j<N; j++)
		{

			//节点自身不可达
			if (i == j)
			{
				graph[i][j] = INFINITY;
				continue;
			}
			graph[i][j] = rand() % 100 + 1;
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
	printf("初始化图形结构*************:\n\n");
	for (i = 0; i<N; i++)
	{
		for (j = 0; j<N; j++)
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


