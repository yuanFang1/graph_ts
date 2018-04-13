// ts_vs.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#define MaxIter 200000000
#define MaxPoint 1002
using namespace std;


int conflict[MaxPoint], conf_index[MaxPoint];
int conf_num = 0;
typedef struct ArcNode {
	int adjvex;
	struct ArcNode *next;
}ArcNode;

typedef struct VerNode {
	ArcNode *first;
}VerNode;

typedef struct Move {
	int u, vi, vj;
}Move;
VerNode *adjList;
int sol[MaxPoint];
int adj_color_table[MaxPoint][MaxPoint], tabu_table[MaxPoint][MaxPoint];
int point_num, edge_num, f, best_f;
long long iter;
int res_iter;//用来保存使用最终结果颜色数经历的迭代数 
double res_time;// 保存时间 

void insert_adjList(int i, int j);//插入邻接表
void dynamic_alloc();
int tabusearch(int k);
Move FindMove(int k);
void MakeMove(int u, int vj);
void add_conf(int adjvex);
void del_conf(int adjvex);
int main(int argc, char *argv[])
{
	int i, j, k;
	char c, s1[100], s2[100], file[100];
	FILE *fp;
	srand((unsigned)time(NULL));
	sprintf(file, ".\\instances\\%s", argv[1]);
	if ((fp = fopen(file, "r")) == NULL) {//从命令行中读取文件名参数 
		cout << "file not open";
		return 0;
	}
	do {
		fgets(s1, 100, fp);
	} while (s1[0] == 'c');
	sscanf(s1, "%c %s %d %d", &c, s2, &point_num, &edge_num);

	dynamic_alloc();
	memset(adjList, 0, sizeof(VerNode)*(point_num + 1));//将first指针全部初始化为NULL
	while (!feof(fp)) {
		fscanf(fp, "%c %d %d\n", &c, &i, &j);
		insert_adjList(i, j);
		insert_adjList(j, i);
	}

	memset(adj_color_table, 0, sizeof(adj_color_table));
	memset(tabu_table, 0, sizeof(tabu_table));
	f = best_f = 0;
	if (tabusearch(atoi(argv[2])) == 0) {
		cout << argv[2] << " cannot match" << endl;
		cout << "end_f= " << f << "  conf_num= " << conf_num << endl;
	}


	else {
		fp = fopen(".\\result.txt", "a+");
		if (fp == NULL)
			cout << "output file open error\n";
		fprintf(fp, "%s %-9s %-12d %-10lf %lf\n", argv[1], argv[2], res_iter, res_time, res_iter / res_time);
	}

	return 0;
}

void insert_adjList(int i, int j) {
	ArcNode *temp1 = (ArcNode *)malloc(sizeof(ArcNode));
	temp1->adjvex = i;
	temp1->next = adjList[j].first;
	adjList[j].first = temp1;
}
void dynamic_alloc() {//动态存储分配内存
	int i;
	adjList = (VerNode *)malloc(sizeof(VerNode)*(point_num + 1));

}
int tabusearch(int k) {
	int i, j, is_conf;
	for (i = 1; i <= point_num; i++) {
		sol[i] = rand() % k + 1;
		//cout << sol[i] <<' ';
	}
	//cout <<endl;

	for (i = 1; i <= point_num; i++) {
		ArcNode *temp = adjList[i].first;
		is_conf = 0;
		while (temp) {//遍历i节点相邻的节点，如果颜色相同的话则将冲突数加1，最后再除以二
			if (sol[temp->adjvex] == sol[i]) {
				is_conf = 1;
				f++;
			}
			adj_color_table[i][sol[temp->adjvex]]++;
			temp = temp->next;
		}
		if (is_conf) {
			conflict[conf_num] = i;
			conf_index[i] = conf_num++;
		}
	}
	f = f / 2;
	//cout <<"initial_f = "<<f<<endl;
	best_f = f;
	iter = 0;
	clock_t start = clock();
	while (iter < MaxIter) {
		if (f == 0)
			break;
		Move mymove = FindMove(k);
		MakeMove(mymove.u, mymove.vj);
		iter++;
	}
	clock_t ends = clock();
	if (iter == MaxIter)
		return 0;

	res_iter = iter;
	res_time = (double)(ends - start) / CLOCKS_PER_SEC;
	return 1;
}

Move FindMove(int k) {
	int i, j, delt, conf_i;
	int sol_i;//用来保存i结点的一些特征，提升速度 
	Move tabu_move[MaxPoint], non_tabu_move[MaxPoint];//采用狗熊掰玉米的方式由于每次都要调用rand，花的时间会比较多，所以用空间来换时间
	int tabu_cnt = 1, non_tabu_cnt = 1;
	int tabu_move_delt = 100000, non_tabu_move_delt = 100000;//给它赋一个较大值，方便后面求最小值
	for (conf_i = 0; conf_i< conf_num; conf_i++) {
		i = conflict[conf_i];
		sol_i = sol[i];
		if (adj_color_table[i][sol_i]>0) {
			for (j = 1; j <= k; j++) {
				if (j != sol_i) {
					int temp_delt = adj_color_table[i][j] - adj_color_table[i][sol_i];
					//cout <<temp_delt<<'\t';
					if (iter >= tabu_table[i][j]) {
						if (temp_delt <= non_tabu_move_delt) {
							if (temp_delt < non_tabu_move_delt) {
								non_tabu_cnt = 0;
								non_tabu_move_delt = temp_delt;
							}
							non_tabu_move[non_tabu_cnt].u = i;
							non_tabu_move[non_tabu_cnt++].vj = j;
						}
						//	                    else if(temp_delt == non_tabu_move.delt){
						//	                    	non_tabu_cnt++;
						//	                    	double t = 1.0/non_tabu_cnt;
						//	                    	if((rand()/(RAND_MAX+1.0)) < t){//产生0-1的随机数，如果比t小 
						//	                    		non_tabu_move.u = i;
						//		                        non_tabu_move.vi = sol_i;
						//		                        non_tabu_move.vj = j;
						//							}
						//	                    		
						//						}
					}
					else {//满足这个条件，j这种颜色处于禁忌当中
						if (temp_delt <= tabu_move_delt) {
							if (temp_delt < tabu_move_delt) {
								tabu_cnt = 0;
								tabu_move_delt = temp_delt;
							}
							tabu_move[tabu_cnt].u = i;
							tabu_move[tabu_cnt++].vj = j;
						}
						//	                    else if(temp_delt == tabu_move.delt){
						//	                    	tabu_cnt++;
						//	                    	double t = 1.0/tabu_cnt,r= rand()/(RAND_MAX+1.0);
						//	                    	if(r < t){//产生0-1的随机数，如果比t小 
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
	int temp1 = f + tabu_move_delt, temp2 = f + non_tabu_move_delt;
	Move res;
	if (temp1<best_f && temp1<temp2) {//比历史最优要好，且比非禁忌邻域中的结果要好
		f = best_f = temp1;
		int index = rand() % tabu_cnt;
		res = tabu_move[index];
	}
	else {
		if (temp2<best_f)
			best_f = temp2;
		f = temp2;
		int index = rand() % non_tabu_cnt;
		res = non_tabu_move[index];
	}
	return res;

}

void MakeMove(int u, int vj) {
	int vi = sol[u];
	sol[u] = vj;
	tabu_table[u][vi] = f + iter + rand() % 10 + 1;
	ArcNode *temp = adjList[u].first;

	while (temp) {
		int adjvex = temp->adjvex;
		if ((--adj_color_table[adjvex][vi]) == 0) {
			if (sol[adjvex] == vi) {//这个点的颜色为vi，但是vi对应的仇人数变成0了，所以不再是冲突点
				del_conf(adjvex);
			}
		}
		if ((++adj_color_table[adjvex][vj]) == 1) {
			if (sol[adjvex] == vj) {//这个点的颜色为vj，vj对应的仇人数变成了1，变成了冲突点
				add_conf(adjvex);
			}
		}
		temp = temp->next;
	}
	if (adj_color_table[u][vi] != 0 && adj_color_table[u][vj] == 0)
		del_conf(u);
	if (adj_color_table[u][vi] == 0 && adj_color_table[u][vj] != 0)
		add_conf(u);
}

void add_conf(int adjvex) {
	conflict[conf_num] = adjvex;
	conf_index[adjvex] = conf_num++;
}

void del_conf(int adjvex) {
	int temp_index = conf_index[adjvex];
	conflict[temp_index] = conflict[--conf_num];
	conf_index[conflict[temp_index]] = temp_index;
}

