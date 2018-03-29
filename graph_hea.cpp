#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <tchar.h>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#define MaxPoint 1002
#define P 20
using namespace std;
typedef struct Partion{

}Partion;

typedef struct Popu{
    int min_f,min_p;
    int fnum[P+1];
}Popu;
Popu popu;
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
int sol[P+1][MaxPoint];//��������p����Ⱥ
int adj_color_table[MaxPoint][MaxPoint],tabu_table[MaxPoint][MaxPoint];
int point_num, edge_num,f,best_f,p;
long long iter,MaxIter ;
int res_iter;//��������ʹ�����ս����ɫ�������ĵ�����
double res_time;// ����ʱ��


void insert_adjList(int i,int j);//�����ڽӱ�
void dynamic_alloc();
int tabusearch(int k);
Move FindMove(int k);
void MakeMove(int u,int vi,int vj);

/*
argv[1]:�ļ���
argv[2]:������󲽳�
argv[3]:��ɫ��k
*/
int main(int argc,char *argv[])
{
	int i,j,k;
	int right,left,middle,res = 10000;
	char c,s1[100],s2[100],file[100];
	FILE *fp;
	sprintf(file,".\\instances\\%s",argv[1]);
	MaxIter = strtol(argv[2]);
	if ((fp=fopen(file,"r"))==NULL) {//���������ж�ȡ�ļ�������
		cout << "file not open";
		return 0;
	}
	do{
		fgets(s1,100,fp);
	} while (s1[0] == 'c');
	sscanf(s1, "%c %s %d %d",&c,s2, &point_num, &edge_num);

	dynamic_alloc();
	memset(adjList,0,sizeof(VerNode)*(point_num+1));//��firstָ��ȫ����ʼ��ΪNULL
	while(!feof(fp)){
		fscanf(fp,"%c %d %d\n",&c,&i,&j);
		insert_adjList(i,j);
		insert_adjList(j,i);
	}

    popu.min_f = 100000;
	for(p=1;p<=P;p++){//��ó�ʼ����Ⱥ
	 	memset(adj_color_table,0,sizeof(adj_color_table));
	 	memset(tabu_table,0,sizeof(tabu_table));
	 	f= best_f =0;
		tabusearch(atoi(argv[3]));
		popu.fnum[p] = f;
		if(f < popu.min_f){
            popu.min_f = f;
            popu.min_p = p;
		}
	}




	fp = fopen(".\\result.txt","a+");
	if(fp == NULL)
		printf("output file open error\n");
	fprintf(fp,"%s %-9d %-12d %lf\n",argv[1],k+1,res_iter,res_time);
    return 0;
}

void insert_adjList(int i,int j){
	ArcNode *temp1 = (ArcNode *)malloc(sizeof(ArcNode));
	temp1->adjvex = i;
	temp1->next = adjList[j].first;
	adjList[j].first = temp1;
}
void dynamic_alloc(){//��̬�洢�����ڴ�
	int i;
	adjList = (VerNode *)malloc(sizeof(VerNode)*(point_num+1));
}
int tabusearch(int k){
	int i,j;
	srand((unsigned)time(NULL));
	for(i=1;i<=point_num;i++){
		sol[p][i] = rand()%k+1;
		//cout << sol[i] <<' ';
	}
	//cout <<endl;
	for(i=1;i<=point_num;i++){
		ArcNode *temp =adjList[i].first;
		while(temp){//����i�ڵ����ڵĽڵ㣬�����ɫ��ͬ�Ļ��򽫳�ͻ����1������ٳ��Զ�
            if(sol[p][temp->adjvex] == sol[p][i]){
                f++;
            }
            adj_color_table[i][sol[p][temp->adjvex]]++;
            temp = temp->next;
		}
	}
	f = f/2;
	//cout <<"initial_f = "<<f<<endl;
	best_f = f;
	iter=0;
	clock_t start = clock();
	while(iter < MaxIter){
		if(f == 0)
            break;
        Move mymove = FindMove(k);
        MakeMove(mymove.u,mymove.vi,mymove.vj);
        iter++;
	}
	clock_t ends = clock();
	if(iter == MaxIter)
        return 0;

    res_iter = iter;
	res_time = (double)(ends - start)/ CLOCKS_PER_SEC;
    return 1;
}

Move FindMove(int k){
    int i,j,delt;
    Move tabu_move,non_tabu_move;
    float tabu_cnt,non_tabu_cnt;
    tabu_move.delt = non_tabu_move.delt = 100000;//������һ���ϴ�ֵ�������������Сֵ
    srand((unsigned)time(NULL));
    for(i=1;i<=point_num;i++){
        if(adj_color_table[i][sol[p][i]]>0){
            for(j=1;j<=k;j++){
            	if(j !=sol[p][i]){
            		int temp_delt = adj_color_table[i][j] -adj_color_table[i][sol[p][i]];
	                //cout <<temp_delt<<'\t';
	                if(iter < tabu_table[i][j]){//�������������j������ɫ���ڽ��ɵ���
	                    if(temp_delt < tabu_move.delt){
	                        tabu_move.delt = temp_delt;
	                        tabu_move.u = i;
	                        tabu_move.vi = sol[p][i];
	                        tabu_move.vj = j;
	                        tabu_cnt =1 ;
	                    }
	                    else if(temp_delt == tabu_move.delt){
	                    	tabu_cnt++;
	                    	double t = 1.0/tabu_cnt,r= rand()/(RAND_MAX+1.0);
	                    	if(r < t){//����0-1��������������tС
	                    		tabu_move.u = i;
		                        tabu_move.vi = sol[p][i];
		                        tabu_move.vj = j;
							}

						}

	                }
	                else{
	                    if(temp_delt < non_tabu_move.delt){
	                        non_tabu_move.delt = temp_delt;
	                        non_tabu_move.u = i;
	                        non_tabu_move.vi = sol[p][i];
	                        non_tabu_move.vj = j;
	                        non_tabu_cnt=1;
	                    }
	                    else if(temp_delt == non_tabu_move.delt){
	                    	non_tabu_cnt++;
	                    	double t = 1.0/non_tabu_cnt;
	                    	if((rand()/(RAND_MAX+1.0)) < t){//����0-1��������������tС
	                    		non_tabu_move.u = i;
		                        non_tabu_move.vi = sol[p][i];
		                        non_tabu_move.vj = j;
							}

						}
	                }
				}

            }
        }
    }
    int temp1 = f+tabu_move.delt ,temp2 = f+non_tabu_move.delt;
    if(temp1<best_f && temp1<temp2){//����ʷ����Ҫ�ã��ұȷǽ��������еĽ��Ҫ��
        f = best_f = temp1;
        //cout <<"best_f=f="<<f<<endl;
        return tabu_move;
    }
    else{

        if(temp2<best_f)
            best_f = temp2;
        f = temp2;
        //cout<<"delt="<<non_tabu_move.delt<<"\tbest_f = "<<best_f<<"  f ="<<f<<endl;
        return non_tabu_move;
    }

}

void MakeMove(int u,int vi,int vj){
    sol[p][u] = vj;
    srand((unsigned)time(NULL));
    tabu_table[u][vi] = f+iter+rand()%10;
    ArcNode *temp = adjList[u].first;
    while(temp){
        adj_color_table[temp->adjvex][vi]--;
        adj_color_table[temp->adjvex][vj]++;
        temp=temp->next;
    }
}
