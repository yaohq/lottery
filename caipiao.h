#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "sqlite3.h"

//#define PRINT_INFO
#define WRITE_LOG

#define LEN 500		/*拼接sql的数组长度*/
#define THREAD_NUM 3
#define OBJECT_QUEUE_NUM	100000		/*存放号码组队列的个数*/
#define THREAD_QUEUE_MIN_NUM 100	/*队列内号码组小于此值开始入队列*/
#define H_PARAM_MAX_LINE_LEN 100	/*配置文件参数名和参数值长度*/

/*主线程生成号码存放结构体*/
typedef struct _stru_num{
	int number;
	int red1;
	int red2;
	int red3;
	int red4;
	int red5;
	int red6;
	int blue;
}STRU_NUM;

/*子线程读取队列获取号码存放结构体*/
typedef struct _thread_stru_num{
	int number;
	int red1;
	int red2;
	int red3;
	int red4;
	int red5;
	int red6;
	int blue;
}THREAD_STRU_NUM;

typedef struct _cs_thread_node
{
    int thread_suffix;      /*!<  线程下标 */
    pthread_t thread_id;    /*!<  线程ID */
    int waiting_i;          /*!<  等待标志,1：处于等待状态 0：处理  2：退出*/
    int return_stat;        /*!<  线程返回状态 */
    struct _cs_thread_node *next;   /*!<  下个线程信息链表节点 */
}STRU_THREAD_NODE;

typedef struct _cs_thread_object_queue
{
	STRU_NUM arr[OBJECT_QUEUE_NUM];
	int head;
	int tail;
}STRU_THREAD_OBJECT_QUEUE;

typedef struct _param_node
{
	int number;
	int sum_limit_low;
	int sum_limit_up;
	int tail_sum_limit_low;
	int tail_sum_limit_up;
	int tail_group_count;	/*记录尾数组数的实际组数*/
	int tail_group[3];	/*尾数组最多判断三种情况，0,1,2*/
	int odd_count;			/*奇数实际个数*/
	int odd_number[4];	/*奇数个数，最多可以有四个值*/
	int big_count;			/*大号实际个数*/
	int big_number[4];	/*大号个数，最多可以有四个值*/
	int prime_count;
	int prime_number[4];
	int cons_number_count;
	int cons_number[3];
	int cons_group_count;
	int cons_group[3];
	int max_diff_limit_low;
	int max_diff_limit_up;
	int ac_value_count;
	int ac_value[5];
	int repit_count;
	int repit[4];
	int close_count;
	int close[5];
	int lone_count;
	int lone[5];
	/*继续添加*/
}PARAM_NODE;

PARAM_NODE param_node; /*配置文件参数结构体，全局共享*/
int out_flag;
STRU_NUM stru_num;
STRU_THREAD_OBJECT_QUEUE stru_thread_object_queue;
pthread_mutex_t mutex_object;
double count_all;	/*所有号码组合总数*/
double count_filter;	/*过滤后剩下的号码总数*/


void init_global_params(void);
int sqlite_conn(sqlite3 **conn);
int get_object_proc(void);
int thread_get_queue_num(void);
int thread_insert_queue(void);
int thread_out_queue(THREAD_STRU_NUM *thread_stru_num);
void *cs_thread_process(void *thread_cur);

int thread_filter(sqlite3 *conn, FILE *fp, THREAD_STRU_NUM *thread_stru_num);
int select_his_record_count(sqlite3 *conn, THREAD_STRU_NUM *thread_stru_num);
int filter_sum(THREAD_STRU_NUM *thread_stru_num);
int filter_tail_sum(THREAD_STRU_NUM *thread_stru_num);
int filter_tail_group(THREAD_STRU_NUM *thread_stru_num);
int filter_odd_even(THREAD_STRU_NUM *thread_stru_num);
int filter_big(THREAD_STRU_NUM *thread_stru_num);
int filter_prime(THREAD_STRU_NUM *thread_stru_num);
int filter_consecutive(THREAD_STRU_NUM *thread_stru_num);
int filter_max_diff(THREAD_STRU_NUM *thread_stru_num);
int filter_ac_value(THREAD_STRU_NUM *thread_stru_num);
int filter_clg(sqlite3 *conn, THREAD_STRU_NUM *thread_stru_num);


int get_conf_param(const char *file);
int get_param(FILE *fp, char *param_name, char *param_value);
void trim_the_str(char *str);
int split_the_str(const char *str, const char delim, char tail_group[][2], int len);
int write_log(FILE *fp, char *sign, THREAD_STRU_NUM *thread_stru_num, char *des);
int get_lottery_num(sqlite3 *conn, int number, THREAD_STRU_NUM *thread_last_num);