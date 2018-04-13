#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <tchar.h>
#include <string>
#include <iostream>
#include <fstream>
#define MaxIter 10000
#define MaxPoint 1002
using namespace std;

typedef struct ArcNode{
	int adjvex;
	struct ArcNode *next;
}ArcNode;

typedef struct VerNode{
	ArcNode *first;
}VerNode;

typedef struct Move{
    int u,vi,vj,delt;
}Move;
VerNode *adjList;
int *sol,*bestSol;
int adj_color_table[MaxPoint][MaxPoint],tabu_table[MaxPoint][MaxPoint];
int point_num, edge_num,iter,f,best_f;

void insert_adjList(int i,int j);//插入邻接表
void dynamic_alloc();
int tabusearch(int k);
Move FindMove(int k);
void MakeMove(int u,int vi,int vj);

int main(int argc,char *argv[])
{
	int i,j,k;
	char c,s1[100],s2[100];
	FILE *fp;
	if ((fp=fopen(".\\instances\\DSJC1000.9.col","r"))==NULL) {
		cout << "file not open";
		return 0;
	}
	do{
		fgets(s1,100,fp);
	} while (s1[0] == 'c');
	sscanf(s1, "%c %s %d %d",&c,s2, &point_num, &edge_num);

	dynamic_alloc();
	memset(adjList,0,sizeof(VerNode)*(point_num+1));//将first指针全部初始化为NULL
	while(!feof(fp)){
		fscanf(fp,"%c %d %d\n",&c,&i,&j);
		insert_adjList(i,j);
		insert_adjList(j,i);
	}
	for(k=point_num-1;k>=1;k--){//将优化问题转化为判定问题，从多到少进行判断
	 	memset(adj_color_table,0,sizeof(adj_color_table));
	 	memset(tabu_table,0,sizeof(tabu_table));
	 	f= best_f =0;
		if(tabusearch(k) == 0)
			break;
		cout <<k<<endl;
	}
	cout <<"the color num is"<<k+1<<endl;
    return 0;
}

void insert_adjList(int i,int j){
	ArcNode *temp1 = (ArcNode *)malloc(sizeof(ArcNode));
	temp1->adjvex = i;
	temp1->next = adjList[j].first;
	adjList[j].first = temp1;
}
void dynamic_alloc(){//动态存储分配内存
	int i;
	adjList = (VerNode *)malloc(sizeof(VerNode)*(point_num+1));
	sol = (int *)malloc(sizeof(int)*(point_num+1));
//	adj_color_table = (int **)malloc(sizeof(int *)*(point_num+1));
//	tabu_table = (int **)malloc(sizeof(int *)*(point_num+1));
//	for(i=0;i<=point_num;i++){
//		adj_color_table[i] = (int *)malloc(sizeof(int)*(point_num+1));
//		tabu_table[i] = (int *)malloc(sizeof(int)*(point_num+1));
//	}

}
int tabusearch(int k){
	int i;
	srand((unsigned)time(NULL));
	for(i=1;i<=point_num;i++){
		sol[i] = rand()%k+1;
	}
	for(i=1;i<=point_num;i++){
		ArcNode *temp =adjList[i].first;
		while(temp){//遍历i节点相邻的节点，如果颜色相同的话则将冲突数加1，最后再除以二
            if(sol[temp->adjvex] == sol[i]){
                f++;
            }
            adj_color_table[i][sol[temp->adjvex]]++;
            temp = temp->next;
		}
	}
	f = f/2;
	best_f = f;
	iter=0;
	while(iter < MaxIter){
		if(f == 0)
            break;
        Move mymove = FindMove(k);
        MakeMove(mymove.u,mymove.vi,mymove.vj);
        iter++;
	}
	if(iter == MaxIter)
        return 0;
    return 1;
}

Move FindMove(int k){
    int i,j,delt;
    Move tabu_move,non_tabu_move;
    float tabu_cnt,non_tabu_cnt;
    tabu_move.delt = non_tabu_move.delt = 100000;//给它赋一个较大值，方便后面求最小值
    srand((unsigned)time(NULL));
    for(i=1;i<=point_num;i++){
        if(adj_color_table[i][sol[i]]>0){
            for(j=1;j<=k;j++){
                int temp_delt = adj_color_table[i][j] -adj_color_table[i][sol[i]];
                if(iter < tabu_table[i][j]){//满足这个条件，j这种颜色处于禁忌当中
                    if(temp_delt < tabu_move.delt){
                        tabu_move.delt = temp_delt;
                        tabu_move.u = i;
                        tabu_move.vi = sol[i];
                        tabu_move.vj = j;
                        tabu_cnt =1 ;
                    }
                    else if(temp_delt == tabu_move.delt){
                    	tabu_cnt++;
                    	double t = 1.0/tabu_cnt;
                    	
                    	if((rand()/(RAND_MAX+1.0)) < t){//产生0-1的随机数，如果比t小 
                    		tabu_move.u = i;
	                        tabu_move.vi = sol[i];
	                        tabu_move.vj = j;
						}
                    		
					}
                    
                }
                else{
                    if(temp_delt < non_tabu_move.delt){
                        non_tabu_move.delt = temp_delt;
                        non_tabu_move.u = i;
                        non_tabu_move.vi = sol[i];
                        non_tabu_move.vj = j;
                        non_tabu_cnt=1;
                    }
                    else if(temp_delt == non_tabu_move.delt){
                    	non_tabu_cnt++;
                    	double t = 1.0/non_tabu_cnt;
                    	if((rand()/(RAND_MAX+1.0)) < t){//产生0-1的随机数，如果比t小 
                    		non_tabu_move.u = i;
	                        non_tabu_move.vi = sol[i];
	                        non_tabu_move.vj = j;
						}
                    		
					}
                }
            }
        }
    }
    int temp1 = f+tabu_move.delt ,temp2 = f+non_tabu_move.delt;
    if(temp1<best_f && temp1<temp2){//比历史最优要好，且比非禁忌邻域中的结果要好
        f = best_f = temp1;
        return tabu_move;
    }
    else{
        if(temp2<best_f)
            best_f = temp2;
        f = temp2;
        return non_tabu_move;
    }

}

void MakeMove(int u,int vi,int vj){
    sol[u] = vj;
    srand((unsigned)time(NULL));
    tabu_table[u][vj] = f+iter+rand()%10;
    ArcNode *temp = adjList[u].first;
    while(temp){
        adj_color_table[temp->adjvex][vi]--;
        adj_color_table[temp->adjvex][vj]++;
        temp=temp->next;
    }
}
