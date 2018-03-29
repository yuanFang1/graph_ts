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


typedef struct P_sol{
	int psol[MaxPoint][MaxPoint];
	int num[MaxPoint];
	int index1[MaxPoint],index2[MaxPoint];
}P_sol;

typedef struct Popu{
    int min_f,min_p;
    int fnum[P+1];
}Popu;
Popu popu;
P_sol p_sol[P+1];
int conflict[MaxPoint],conf_index[MaxPoint];
int conf_num=0;
typedef struct ArcNode{
	int adjvex;
	struct ArcNode *next;
}ArcNode;

typedef struct VerNode{
	ArcNode *first;
}VerNode;

typedef struct Move{
    int u,vi,vj;
}Move;
VerNode *adjList;
int sol[P+1][MaxPoint];
int adj_color_table[MaxPoint][MaxPoint],tabu_table[MaxPoint][MaxPoint];
int point_num, edge_num,f,best_f;
long long iter;
int res_iter;//ÓÃÀ´±£´æÊ¹ÓÃ×îÖÕ½á¹ûÑÕÉ«Êý¾­ÀúµÄµü´úÊý 
double res_time;// ±£´æÊ±¼ä 

void insert_adjList(int i,int j);//²åÈëÁÚ½Ó±í
void dynamic_alloc();
int tabusearch(int k);
Move FindMove(int k);
void MakeMove(int u,int vj);
void add_conf(int adjvex); 
void del_conf(int adjvex);
/*
argv[1]:ÎÄ¼þÃû
argv[2]:µü´ú×î´ó²½³¤
argv[3]:ÑÕÉ«Êýk
*/
int main(int argc,char *argv[])
{
	int i,j,k,p;
	int right,left,middle,res = 10000;
	char c,s1[100],s2[100],file[100];
	FILE *fp;
	sprintf(file,".\\instances\\%s",argv[1]);
	MaxIter = strtol(argv[2]);
	if ((fp=fopen(file,"r"))==NULL) {//´ÓÃüÁîÐÐÖÐ¶ÁÈ¡ÎÄ¼þÃû²ÎÊý
		cout << "file not open";
		return 0;
	}
	do{
		fgets(s1,100,fp);
	} while (s1[0] == 'c');
	sscanf(s1, "%c %s %d %d",&c,s2, &point_num, &edge_num);

	dynamic_alloc();
	memset(adjList,0,sizeof(VerNode)*(point_num+1));//½«firstÖ¸ÕëÈ«²¿³õÊ¼»¯ÎªNULL
	while(!feof(fp)){
		fscanf(fp,"%c %d %d\n",&c,&i,&j);
		insert_adjList(i,j);
		insert_adjList(j,i);
	}

    popu.min_f = 100000;
	for(p=1;p<=P;p++){//»ñµÃ³õÊ¼µÄÖÖÈº
	 	memset(adj_color_table,0,sizeof(adj_color_table));
	 	memset(tabu_table,0,sizeof(tabu_table));
	 	f= best_f =0;
		tabusearch(atoi(argv[3]));
		popu.fnum[p] = f;
		if(f == 0)
			break;
		if(f < popu.min_f){
            popu.min_f = f;
            popu.min_p = p;
		}
	}
	memset(p_sol,0,sizeof(p_sol));
	if(p > P){
		for(p=1;p<=P;p++){//将解转变成划分的形式
			for(i=1;i<=point_num;i++){
				int color = p_sol[p].index1[i] = sol[p][i];
				int color_num = p_sol[p].num[color];
				p_sol[p].psol[color][color_num] = i;
				p_sol[p].index2[i] = color_num++;
				p_sol[p].num[color] =color_num;
			}
		}
	}




	fp = fopen(".\\result.txt","a+");
	if(fp == NULL)
		printf("output file open error\n");
	fprintf(fp,"%s %-9d %-12d %lf\n",argv[1],argv[3],res_iter,res_time);
    return 0;
}

void insert_adjList(int i,int j){
	ArcNode *temp1 = (ArcNode *)malloc(sizeof(ArcNode));
	temp1->adjvex = i;
	temp1->next = adjList[j].first;
	adjList[j].first = temp1;
}
void dynamic_alloc(){//¶¯Ì¬´æ´¢·ÖÅäÄÚ´æ
	int i;
	adjList = (VerNode *)malloc(sizeof(VerNode)*(point_num+1));

}
int tabusearch(int k){
	int i,j,is_conf;
	srand((unsigned)time(NULL));
	for(i=1;i<=point_num;i++){
		sol[p][i] = rand()%k+1;
		//cout << sol[i] <<' ';
	}
	//cout <<endl;

	for(i=1;i<=point_num;i++){
		ArcNode *temp =adjList[i].first;
		is_conf =0;
		while(temp){//±éÀúi½ÚµãÏàÁÚµÄ½Úµã£¬Èç¹ûÑÕÉ«ÏàÍ¬µÄ»°Ôò½«³åÍ»Êý¼Ó1£¬×îºóÔÙ³ýÒÔ¶þ
            if(sol[p][temp->adjvex] == sol[p][i]){
            	is_conf =1;
                f++;
            }
            adj_color_table[i][sol[p][temp->adjvex]]++;
            temp = temp->next;
		}
		if(is_conf){
			conflict[conf_num] = i;
			conf_index[i] = conf_num++;
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
        MakeMove(mymove.u,mymove.vj);
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
    int i,j,delt,conf_i;
    int sol_i;//ÓÃÀ´±£´æi½áµãµÄÒ»Ð©ÌØÕ÷£¬ÌáÉýËÙ¶È 
    Move tabu_move[MaxPoint],non_tabu_move[MaxPoint];//²ÉÓÃ¹·ÐÜêþÓñÃ×µÄ·½Ê½ÓÉÓÚÃ¿´Î¶¼Òªµ÷ÓÃrand£¬»¨µÄÊ±¼ä»á±È½Ï¶à£¬ËùÒÔÓÃ¿Õ¼äÀ´»»Ê±¼ä
    int tabu_cnt=1,non_tabu_cnt=1;
    int tabu_move_delt = 100000,non_tabu_move_delt = 100000;//¸øËü¸³Ò»¸ö½Ï´óÖµ£¬·½±ãºóÃæÇó×îÐ¡Öµ
    srand((unsigned)time(NULL));
    for(conf_i=0;conf_i< conf_num ;conf_i++){
    	i = conflict[conf_i];
    	sol_i = sol[p][i];
        if(adj_color_table[i][sol_i]>0){
            for(j=1;j<=k;j++){
            	if(j != sol_i){
            		int temp_delt = adj_color_table[i][j] -adj_color_table[i][sol_i];
	                //cout <<temp_delt<<'\t';
	                if(iter >= tabu_table[i][j]){
	                    if(temp_delt <= non_tabu_move_delt){
	                    	if(temp_delt < non_tabu_move_delt){
	                    		non_tabu_cnt =0;
	                    		non_tabu_move_delt = temp_delt;
							}
	                        non_tabu_move[non_tabu_cnt].u = i;
	                        non_tabu_move[non_tabu_cnt++].vj = j;
	                    }
//	                    else if(temp_delt == non_tabu_move.delt){
//	                    	non_tabu_cnt++;
//	                    	double t = 1.0/non_tabu_cnt;
//	                    	if((rand()/(RAND_MAX+1.0)) < t){//²úÉú0-1µÄËæ»úÊý£¬Èç¹û±ÈtÐ¡ 
//	                    		non_tabu_move.u = i;
//		                        non_tabu_move.vi = sol_i;
//		                        non_tabu_move.vj = j;
//							}
//	                    		
//						}
	                }
	                else{//Âú×ãÕâ¸öÌõ¼þ£¬jÕâÖÖÑÕÉ«´¦ÓÚ½û¼Éµ±ÖÐ
	                    if(temp_delt <= tabu_move_delt){
	                    	if(temp_delt < tabu_move_delt){
	                    		tabu_cnt =0;
	                    		tabu_move_delt = temp_delt;
							}
	                        tabu_move[tabu_cnt].u = i;
	                        tabu_move[tabu_cnt++].vj = j;
	                    }
//	                    else if(temp_delt == tabu_move.delt){
//	                    	tabu_cnt++;
//	                    	double t = 1.0/tabu_cnt,r= rand()/(RAND_MAX+1.0);
//	                    	if(r < t){//²úÉú0-1µÄËæ»úÊý£¬Èç¹û±ÈtÐ¡ 
//	                    		tabu_move.u = i;
//		                        tabu_move.vi = sol_i;
//		                        tabu_move.vj = j;
//							}
//	                    		
//						}
	                    
	                }
				}
                
            }
        }
    }
    int temp1 = f+tabu_move_delt ,temp2 = f+non_tabu_move_delt;
    Move res;
    if(temp1<best_f && temp1<temp2){//±ÈÀúÊ·×îÓÅÒªºÃ£¬ÇÒ±È·Ç½û¼ÉÁÚÓòÖÐµÄ½á¹ûÒªºÃ
        f = best_f = temp1;
        int index = rand()%tabu_cnt;
        res = tabu_move[index];
    }
    else{
        if(temp2<best_f)
            best_f = temp2;
        f = temp2;
        int index = rand()%non_tabu_cnt;
        res = non_tabu_move[index];
    }
    return res;

}
void MakeMove(int u,int vj){
	int vi = sol[p][u];
    sol[p][u] = vj;
    srand((unsigned)time(NULL));
    tabu_table[u][vi] = f+iter+rand()%10+1;
    ArcNode *temp = adjList[u].first;
    
    while(temp){
    	int adjvex = temp->adjvex;
        if((--adj_color_table[adjvex][vi] )==0){
        	if(sol[p][adjvex] == vi){//Õâ¸öµãµÄÑÕÉ«Îªvi£¬µ«ÊÇvi¶ÔÓ¦µÄ³ðÈËÊý±ä³É0ÁË£¬ËùÒÔ²»ÔÙÊÇ³åÍ»µã
        		del_conf(adjvex);
        	}
        }
        if((++adj_color_table[adjvex][vj]) == 1){
        	if(sol[p][adjvex] == vj){//Õâ¸öµãµÄÑÕÉ«Îªvj£¬vj¶ÔÓ¦µÄ³ðÈËÊý±ä³ÉÁË1£¬±ä³ÉÁË³åÍ»µã
        		add_conf(adjvex); 
        	}
        }
        temp=temp->next;
    }
    if(adj_color_table[u][vi]!=0 &&adj_color_table[u][vj]==0)
    	del_conf(u);
    if(adj_color_table[u][vi]==0 &&adj_color_table[u][vj]!=0)
    	add_conf(u);
}

void add_conf(int adjvex){
	conflict[conf_num] = adjvex;
    conf_index[adjvex] = conf_num++;
}

void del_conf(int adjvex){
	int temp_index = conf_index[adjvex];
    conflict[temp_index] = conflict[--conf_num];
    conf_index[conflict[temp_index]] =temp_index;
}
