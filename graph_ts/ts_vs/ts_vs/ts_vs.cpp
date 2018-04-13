// ts_vs.cpp : �������̨Ӧ�ó������ڵ㡣
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
int res_iter;//��������ʹ�����ս����ɫ�������ĵ����� 
double res_time;// ����ʱ�� 

void insert_adjList(int i, int j);//�����ڽӱ�
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
	if ((fp = fopen(file, "r")) == NULL) {//���������ж�ȡ�ļ������� 
		cout << "file not open";
		return 0;
	}
	do {
		fgets(s1, 100, fp);
	} while (s1[0] == 'c');
	sscanf(s1, "%c %s %d %d", &c, s2, &point_num, &edge_num);

	dynamic_alloc();
	memset(adjList, 0, sizeof(VerNode)*(point_num + 1));//��firstָ��ȫ����ʼ��ΪNULL
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
void dynamic_alloc() {//��̬�洢�����ڴ�
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
		while (temp) {//����i�ڵ����ڵĽڵ㣬�����ɫ��ͬ�Ļ��򽫳�ͻ����1������ٳ��Զ�
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
	int sol_i;//��������i����һЩ�����������ٶ� 
	Move tabu_move[MaxPoint], non_tabu_move[MaxPoint];//���ù��������׵ķ�ʽ����ÿ�ζ�Ҫ����rand������ʱ���Ƚ϶࣬�����ÿռ�����ʱ��
	int tabu_cnt = 1, non_tabu_cnt = 1;
	int tabu_move_delt = 100000, non_tabu_move_delt = 100000;//������һ���ϴ�ֵ�������������Сֵ
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
						//	                    	if((rand()/(RAND_MAX+1.0)) < t){//����0-1��������������tС 
						//	                    		non_tabu_move.u = i;
						//		                        non_tabu_move.vi = sol_i;
						//		                        non_tabu_move.vj = j;
						//							}
						//	                    		
						//						}
					}
					else {//�������������j������ɫ���ڽ��ɵ���
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
						//	                    	if(r < t){//����0-1��������������tС 
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
	if (temp1<best_f && temp1<temp2) {//����ʷ����Ҫ�ã��ұȷǽ��������еĽ��Ҫ��
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
			if (sol[adjvex] == vi) {//��������ɫΪvi������vi��Ӧ�ĳ��������0�ˣ����Բ����ǳ�ͻ��
				del_conf(adjvex);
			}
		}
		if ((++adj_color_table[adjvex][vj]) == 1) {
			if (sol[adjvex] == vj) {//��������ɫΪvj��vj��Ӧ�ĳ����������1������˳�ͻ��
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

