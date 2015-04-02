
#include "caipiao.h"

void init_global_params(void)
{
	out_flag = 0;
	count_all = 0;
	count_filter = 0;
	memset(&stru_thread_object_queue, 0, sizeof(STRU_THREAD_OBJECT_QUEUE));
	memset(&stru_num, 0, sizeof(STRU_NUM));
	pthread_mutex_init(&mutex_object, NULL);
}

/*记录日志*/
int write_log(FILE *fp, char *sign, THREAD_STRU_NUM *thread_stru_num, char *des)
{
	int ret;
	char str[100];
	sprintf(str, "%7d %2d %2d %2d %2d %2d %2d %2d", thread_stru_num->number, thread_stru_num->red1, thread_stru_num->red2, \
			thread_stru_num->red3, thread_stru_num->red4, thread_stru_num->red5, thread_stru_num->red6, thread_stru_num->blue);
	if((ret = fprintf(fp, "%s | %s | %s\n", sign, str, des)) < 0)
		return -1;
	return 0;
}

/******************************************************************/
/*!
	* @brief  从配置文件里面获得制定参数
	* @param [in]	fp			配置文件
	* @param [in]	param_name	要想获取参数的参数名
	* @param [out]	param_value	返回参数值
	* @return 0：查找成功，-1：查找失败
	*/
int get_param(FILE *fp, char *param_name, char *param_value)
{
	char file_buffer[H_PARAM_MAX_LINE_LEN];
	char *p_name, *p_value;
	
	rewind(fp);

	while(fgets(file_buffer, H_PARAM_MAX_LINE_LEN, fp) != NULL)
	{
		trim_the_str(file_buffer);
		if(strlen(file_buffer)==0)
			continue;
		if(file_buffer[0]=='#')
			continue;
//		printf("CONFIG: %s\n", file_buffer);
		/*****把"="前面的参数名读出来*****/
		p_name = strtok(file_buffer, "=");
		trim_the_str(p_name);
		
		if(strcmp(p_name, param_name) == 0)
		{
			/*****把=后面的值读出来，并把回车符去掉*****/
			p_value = strtok(NULL, "\n");	
			trim_the_str(p_value);
			
			if(strlen(p_value) == 0)
			{
				printf("The value: %s is null\n", param_name);
				return -1;
			}
			
			strcpy(param_value, p_value);
			return 0;
		}
	}

	return -1;
}

/******************************************************************/
/*!
	* @brief  去掉字符串开头和末尾的空格，制表符，回车
	* @param [in]	str		要调整的字符串
	* @param [out]	str		同时返回调整后的字符串
	* @return 无
	*/
void trim_the_str(char *str)
{
	char tmp[100];
	char *pstr;

	strcpy(tmp, str);
	pstr=&tmp[strlen(tmp)-1];
	while(pstr>=tmp && (*pstr==' ' || *pstr=='\t' || *pstr=='\n'))
	{
		*pstr='\0';
		pstr--;
	}
	pstr=tmp;
	while(*pstr!='\0' && (*pstr==' ' || *pstr=='\t' || *pstr=='\n'))
		pstr++;
	strcpy(str,pstr);
}

/******************************************************************/
/*!
	* @brief  根据传入的分隔符拆分字符串
	* @param [in]	str		要拆分的字符串
	* @param [in]	delim		分隔符
	* @param [out]	str		拆分后的字符串存放位置
	* @param [in]	len		拆分后的字符串最大个数
	* @return 返回拆分的实际个数，出错返回-1
	*/
int split_the_str(const char *str, const char delim, char tail_group[][2], int len)
{
	int i = 0, count = 0;
	char *point, *tmp;
	char str1[10];
	
	strcpy(str1, str);
	point = (char *)str1;
	
	if(strlen(str) == 0)
	{
		printf("split error!\n");
		return -1;
	}
		
	for(i = 0; i < len; i++)
	{
		tmp = strchr(point, delim);
		count++;
		/*没有出现分隔符，放入数组后返回*/
		if(tmp == NULL)
		{
			strcpy(tail_group[i], point);
			break;
		}
		*tmp = '\0';
		/*字符串放入数组*/
		strcpy(tail_group[i], point);
		tmp++;
		point = tmp;
	}
	return count;
}

/*获取配置文件参数*/
int get_conf_param(const char *file)
{
	int i, len, count;
	FILE *fp;
	char issue[10];
	char sum_limit_low[10], sum_limit_up[10];
	char tail_sum_limit_low[10], tail_sum_limit_up[10];
	char tail_group[10], tail_g[3][2];	/*尾数组数，0,1,2三种可能取值*/
	char odd[10], odd_s[4][2];	/*奇数个数，1,2,3,4四种可能取值*/
	char big[10], big_s[4][2];
	char prime[10], prime_s[4][2];
	char cons_number[10], cons_number_s[3][2];
	char cons_group[10], cons_group_s[3][2];
	char max_diff_limit_low[10], max_diff_limit_up[10];
	char ac_value[10], ac_value_s[5][2];
	char repit[10], repit_s[4][2];
	char close[10], close_s[5][2];
	char lone[10], lone_s[5][2];
	char param_name[30];
	
	/*打开配置文件*/
	fp = fopen(file, "r");
	if(NULL == fp)
	{
		printf("open config file %s error!\n", file);
		return -1;
	}
	
	/*变量初始化为0，有问题再补充，暂时没问题*/
	
	strcpy(param_name, "ISSUE");
	if(0 != get_param(fp, param_name, issue))
	{
		printf("%lu: get config %s error\n", pthread_self(), param_name);
		return -1;
	}
	param_node.number = atoi(issue);
	
	strcpy(param_name, "SUM_LIMIT_LOW");
	if(0 != get_param(fp, param_name, sum_limit_low))
	{
		printf("%lu: get config %s error\n", pthread_self(), param_name);
		return -1;
	}
	param_node.sum_limit_low = atoi(sum_limit_low);
	
	strcpy(param_name, "SUM_LIMIT_UP");
	if(0 != get_param(fp, param_name, sum_limit_up))
	{
		printf("%lu: get config %s error\n", pthread_self(), param_name);
		return -1;
	}
	param_node.sum_limit_up = atoi(sum_limit_up);
	
	strcpy(param_name, "TAIL_SUM_LIMIT_LOW");
	if(0 != get_param(fp, param_name, tail_sum_limit_low))
	{
		printf("get config %s error\n", param_name);
		return -1;
	}
	param_node.tail_sum_limit_low = atoi(tail_sum_limit_low);
	
	strcpy(param_name, "TAIL_SUM_LIMIT_UP");
	if(0 != get_param(fp, param_name, tail_sum_limit_up))
	{
		printf("get config %s error\n", param_name);
		return -1;
	}
	param_node.tail_sum_limit_up = atoi(tail_sum_limit_up);

	strcpy(param_name, "TAIL_GROUP");
	if(0 != get_param(fp, param_name, tail_group))
	{
		printf("get config %s error\n", param_name);
		return -1;
	}
//	printf("tail_group = %s\n", tail_group);
	len = sizeof(tail_g) / sizeof(tail_g[0]);	/*获取数组一维长度*/
	/*如果尾号组数有逗号分隔，拆分开放入tail_group数组*/
	if((count = split_the_str(tail_group, ',', tail_g, len)) < 0)
	{
		printf("split TAIL GROUP %s error!\n", tail_group);
		return -1;
	}
	param_node.tail_group_count = count;

//	printf("param_node.tail_group_count = %d\n", param_node.tail_group_count);
	for(i = 0; i < count; i++)
		param_node.tail_group[i] = atoi(tail_g[i]);

	strcpy(param_name, "ODD_NUMBER");
	if(0 != get_param(fp, param_name, odd))
	{
		printf("get config %s error\n", param_name);
		return -1;
	}
	len = sizeof(odd_s) / sizeof(odd_s[0]);	/*获取数组一维长度*/
	/*如果尾号组数有逗号分隔，拆分开放入tail_group数组*/
	if((count = split_the_str(odd, ',', odd_s, len)) < 0)
	{
		printf("split ODD %s error!\n", odd);
		return -1;
	}
	param_node.odd_count = count;
	for(i = 0; i < count; i++)
		param_node.odd_number[i] = atoi(odd_s[i]);
		
	strcpy(param_name, "BIG_NUMBER");
	if(0 != get_param(fp, param_name, big))
	{
		printf("get config %s error\n", param_name);
		return -1;
	}
	len = sizeof(big_s) / sizeof(big_s[0]);	/*获取数组一维长度*/
	/*如果尾号组数有逗号分隔，拆分开放入tail_group数组*/
	if((count = split_the_str(big, ',', big_s, len)) < 0)
	{
		printf("split BIG %s error!\n", big);
		return -1;
	}
	param_node.big_count = count;
	for(i = 0; i < count; i++)
		param_node.big_number[i] = atoi(big_s[i]);
	
	strcpy(param_name, "PRIME_NUMBER");
	if(0 != get_param(fp, param_name, prime))
	{
		printf("get config %s error\n", param_name);
		return -1;
	}
	len = sizeof(prime_s) / sizeof(prime_s[0]);	/*获取数组一维长度*/
	/*如果尾号组数有逗号分隔，拆分开放入tail_group数组*/
	if((count = split_the_str(prime, ',', prime_s, len)) < 0)
	{
		printf("split PRIME NUMBER %s error!\n", prime);
		return -1;
	}
	param_node.prime_count = count;
	for(i = 0; i < count; i++)
		param_node.prime_number[i] = atoi(prime_s[i]);
		
	strcpy(param_name, "CONS_NUMBER");
	if(0 != get_param(fp, param_name, cons_number))
	{
		printf("get config %s error\n", param_name);
		return -1;
	}
	len = sizeof(cons_number_s) / sizeof(cons_number_s[0]);	/*获取数组一维长度*/
	/*如果尾号组数有逗号分隔，拆分开放入tail_group数组*/
	if((count = split_the_str(cons_number, ',', cons_number_s, len)) < 0)
	{
		printf("split PRIME NUMBER %s error!\n", cons_number);
		return -1;
	}
	param_node.cons_number_count = count;
	for(i = 0; i < count; i++)
		param_node.cons_number[i] = atoi(cons_number_s[i]);

	strcpy(param_name, "CONS_GROUP");
	if(0 != get_param(fp, param_name, cons_group))
	{
		printf("get config %s error\n", param_name);
		return -1;
	}
	len = sizeof(cons_group_s) / sizeof(cons_group_s[0]);	/*获取数组一维长度*/
	/*如果尾号组数有逗号分隔，拆分开放入cons_group_s数组*/
	if((count = split_the_str(cons_group, ',', cons_group_s, len)) < 0)
	{
		printf("split PRIME NUMBER %s error!\n", cons_group);
		return -1;
	}
	param_node.cons_group_count = count;
	for(i = 0; i < count; i++)
		param_node.cons_group[i] = atoi(cons_group_s[i]);
	
	strcpy(param_name, "MAX_DIFF_LIMIT_LOW");
	if(0 != get_param(fp, param_name, max_diff_limit_low))
	{
		printf("get config %s error\n", param_name);
		return -1;
	}
	param_node.max_diff_limit_low = atoi(max_diff_limit_low);

	strcpy(param_name, "MAX_DIFF_LIMIT_UP");
	if(0 != get_param(fp, param_name, max_diff_limit_up))
	{
		printf("get config %s error\n", param_name);
		return -1;
	}
	param_node.max_diff_limit_up = atoi(max_diff_limit_up);
	
	strcpy(param_name, "AC_VALUE");
	if(0 != get_param(fp, param_name, ac_value))
	{
		printf("get config %s error\n", param_name);
		return -1;
	}
	len = sizeof(ac_value_s) / sizeof(ac_value_s[0]);	/*获取数组一维长度*/
	/*如果尾号组数有逗号分隔，拆分开放入cons_group_s数组*/
	if((count = split_the_str(ac_value, ',', ac_value_s, len)) < 0)
	{
		printf("split PRIME NUMBER %s error!\n", ac_value);
		return -1;
	}
	param_node.ac_value_count = count;
	for(i = 0; i < count; i++)
		param_node.ac_value[i] = atoi(ac_value_s[i]);
		
	strcpy(param_name, "REPIT_NUMBER");
	if(0 != get_param(fp, param_name, repit))
	{
		printf("get config %s error\n", param_name);
		return -1;
	}
	len = sizeof(repit_s) / sizeof(repit_s[0]);	/*获取数组一维长度*/
	/*如果尾号组数有逗号分隔，拆分开放入cons_group_s数组*/
	if((count = split_the_str(repit, ',', repit_s, len)) < 0)
	{
		printf("split PRIME NUMBER %s error!\n", repit);
		return -1;
	}
	param_node.repit_count = count;
	for(i = 0; i < count; i++)
		param_node.repit[i] = atoi(repit_s[i]);

	strcpy(param_name, "CLOSE_NUMBER");
	if(0 != get_param(fp, param_name, close))
	{
		printf("get config %s error\n", param_name);
		return -1;
	}
	len = sizeof(close_s) / sizeof(close_s[0]);	/*获取数组一维长度*/
	/*如果尾号组数有逗号分隔，拆分开放入cons_group_s数组*/
	if((count = split_the_str(close, ',', close_s, len)) < 0)
	{
		printf("split PRIME NUMBER %s error!\n", close);
		return -1;
	}
	param_node.close_count = count;
	for(i = 0; i < count; i++)
		param_node.close[i] = atoi(close_s[i]);

	strcpy(param_name, "LONE_NUMBER");
	if(0 != get_param(fp, param_name, lone))
	{
		printf("get config %s error\n", param_name);
		return -1;
	}
	len = sizeof(lone_s) / sizeof(lone_s[0]);	/*获取数组一维长度*/
	/*如果尾号组数有逗号分隔，拆分开放入cons_group_s数组*/
	if((count = split_the_str(lone, ',', lone_s, len)) < 0)
	{
		printf("split PRIME NUMBER %s error!\n", lone);
		return -1;
	}
	param_node.lone_count = count;
	for(i = 0; i < count; i++)
		param_node.lone[i] = atoi(lone_s[i]);
		
#if 1
	printf("param_node.number = %d\n", param_node.number);
	printf("param_node.sum_limit_up = %d\n", param_node.sum_limit_up);
	printf("param_node.sum_limit_low = %d\n", param_node.sum_limit_low);
	printf("param_node.tail_sum_limit_up = %d\n", param_node.tail_sum_limit_up);
	printf("param_node.tail_sum_limit_low = %d\n", param_node.tail_sum_limit_low);
	for(i = 0; i < param_node.tail_group_count; i++)
		printf("param_node.tail_group[%d] = %d\n", i, param_node.tail_group[i]);
	for(i = 0; i < param_node.odd_count; i++)
		printf("param_node.odd_number[%d] = %d\n", i, param_node.odd_number[i]);
	for(i = 0; i < param_node.big_count; i++)
		printf("param_node.big_number[%d] = %d\n", i, param_node.big_number[i]);
	for(i = 0; i < param_node.prime_count; i++)
		printf("param_node.prime_number[%d] = %d\n", i, param_node.prime_number[i]);
	for(i = 0; i < param_node.cons_number_count; i++)
		printf("param_node.cons_number[%d] = %d\n", i, param_node.cons_number[i]);
	for(i = 0; i < param_node.cons_group_count; i++)
		printf("param_node.cons_group[%d] = %d\n", i, param_node.cons_group[i]);
	printf("param_node.max_diff_limit_up = %d\n", param_node.max_diff_limit_up);
	printf("param_node.max_diff_limit_low = %d\n", param_node.max_diff_limit_low);
	for(i = 0; i < param_node.ac_value_count; i++)
		printf("param_node.ac_value[%d] = %d\n", i, param_node.ac_value[i]);
	for(i = 0; i < param_node.repit_count; i++)
		printf("param_node.repit[%d] = %d\n", i, param_node.repit[i]);
	for(i = 0; i < param_node.close_count; i++)
		printf("param_node.close[%d] = %d\n", i, param_node.close[i]);
	for(i = 0; i < param_node.lone_count; i++)
		printf("param_node.lone[%d] = %d\n", i, param_node.lone[i]);
#endif
	
	fclose(fp);
	return 0;
}

int sqlite_conn(sqlite3 **conn)
{
	int ret;
	ret = sqlite3_open("lottery.db", conn);
	if(SQLITE_OK != ret)
	{
		printf("connect lottery.db failed!\n");
		sqlite3_close(*conn);
		return -1;
	}
	return 0;
}

int get_object_proc(void)
{
	int red1, red2, red3, red4, red5, red6;
#ifdef BLUE
	int blue;
#endif
	int flag;

#if 0
	for(i = 0; i < OBJECT_QUEUE_NUM; i++)
	{
		stru_num.number = 2014058;
		stru_num.red1 = 1;
		stru_num.red2 = 2;
		stru_num.red3 = 3;
		stru_num.red4 = 4;
		stru_num.red5 = 5;
		stru_num.red6 = 6;
		stru_num.blue = 7;

		pthread_mutex_lock(&mutex_object);
		flag = thread_insert_queue();
		pthread_mutex_unlock(&mutex_object);

		if(-1 == flag)
		{
			printf("thread queue is full!\n");
			sleep(5);
		}
#ifdef PRINT_INFO
		printf("[%d][%d,%d,%d,%d,%d,%d,%d]\n", i,stru_num.red1,stru_num.red2,stru_num.red3,stru_num.red4,stru_num.red5,stru_num.red6,stru_num.blue);
#endif		
	}
#endif

#if 1
	stru_num.number = 2014133;
	for(red1 = 1; red1 < 17; red1++)
	{
		stru_num.red1 = red1;
		for(red2 = red1 + 1; red2 < 19; red2++)
		{
			stru_num.red2 = red2;
			for(red3 = red2 + 1; red3 < 24; red3++)
			{
				if(red3 < 8)
					continue;
				stru_num.red3 = red3;
				for(red4 = red3 + 1; red4 < 30; red4++)
				{
					if(red4 < 10)
						continue;
					stru_num.red4 = red4;
					for(red5 = red4 + 1; red5 < 33; red5++)
					{
						if(red5 < 13)
							continue;
						stru_num.red5 = red5;
						for(red6 = red5 + 1; red6 < 34; red6++)
						{
							if(red5 < 24)
								continue;
							stru_num.red6 = red6;
#ifdef BLUE
							for(blue = 1; blue < 17; blue++)
							{
								stru_num.blue = blue;
								count_all++;

								pthread_mutex_lock(&mutex_object);
								flag = thread_insert_queue();
								pthread_mutex_unlock(&mutex_object);

								if(-1 == flag)
								{
									printf("queue is full!\n");
									sleep(10);
									//reinsert the number which didn't insert success!
									blue--;
									count_all--;
								}
							}
#else
							count_all++;

							pthread_mutex_lock(&mutex_object);
							flag = thread_insert_queue();
							pthread_mutex_unlock(&mutex_object);

							if(-1 == flag)
							{
								printf("queue is full!\n");
								sleep(10);
								//reinsert the number which didn't insert success!
								red6--;
								count_all--;
							}
#endif
						}
					}
				}
			}
		}
	}
#endif

	out_flag = 1;
	return 0;
}

/*获取队列中号码组数目*/
int thread_get_queue_num(void)
{
	int num;
	num = stru_thread_object_queue.tail - stru_thread_object_queue.head;
	if(num < 0)
		num += OBJECT_QUEUE_NUM;

	return num;
}

/*入队列，-1：队列满，入队失败，1：入队后队列满，0：成功*/
int thread_insert_queue()
{
	if((stru_thread_object_queue.tail + 1) % OBJECT_QUEUE_NUM == stru_thread_object_queue.head)
		return -1;

	stru_thread_object_queue.arr[stru_thread_object_queue.tail].number = stru_num.number;
	stru_thread_object_queue.arr[stru_thread_object_queue.tail].red1 = stru_num.red1;
	stru_thread_object_queue.arr[stru_thread_object_queue.tail].red2 = stru_num.red2;
	stru_thread_object_queue.arr[stru_thread_object_queue.tail].red3 = stru_num.red3;
	stru_thread_object_queue.arr[stru_thread_object_queue.tail].red4 = stru_num.red4;
	stru_thread_object_queue.arr[stru_thread_object_queue.tail].red5 = stru_num.red5;
	stru_thread_object_queue.arr[stru_thread_object_queue.tail].red6 = stru_num.red6;
	stru_thread_object_queue.arr[stru_thread_object_queue.tail].blue = stru_num.blue;

	stru_thread_object_queue.tail = (stru_thread_object_queue.tail + 1) % OBJECT_QUEUE_NUM;

	if((stru_thread_object_queue.tail + 1) % OBJECT_QUEUE_NUM == stru_thread_object_queue.head)
		return 1;
	else
		return 0;
}

/*出队列，-1：队列空，0：成功*/
int thread_out_queue(THREAD_STRU_NUM *thread_stru_num)
{
	if(stru_thread_object_queue.head == stru_thread_object_queue.tail)
		return -1;

	thread_stru_num->number = stru_thread_object_queue.arr[stru_thread_object_queue.head].number;
	thread_stru_num->red1 = stru_thread_object_queue.arr[stru_thread_object_queue.head].red1;
	thread_stru_num->red2 = stru_thread_object_queue.arr[stru_thread_object_queue.head].red2;
	thread_stru_num->red3 = stru_thread_object_queue.arr[stru_thread_object_queue.head].red3;
	thread_stru_num->red4 = stru_thread_object_queue.arr[stru_thread_object_queue.head].red4;
	thread_stru_num->red5 = stru_thread_object_queue.arr[stru_thread_object_queue.head].red5;
	thread_stru_num->red6 = stru_thread_object_queue.arr[stru_thread_object_queue.head].red6;
	thread_stru_num->blue = stru_thread_object_queue.arr[stru_thread_object_queue.head].blue;

	stru_thread_object_queue.head = (stru_thread_object_queue.head + 1) % OBJECT_QUEUE_NUM;

	return 0;
}

void *cs_thread_process(void *thread_cur)
{

	int i, f_obj, flag;
	FILE *fp = NULL;
	char filename[20] = "";
	STRU_THREAD_NODE *thread_node;
	THREAD_STRU_NUM thread_stru_num;
	sqlite3 *conn = NULL;


	thread_node = (STRU_THREAD_NODE *)thread_cur;

	flag = sqlite_conn(&conn);
	if(0 != flag)
	{
		printf("sqlite_conn failed!\n");
		pthread_exit(NULL);
	}

	sprintf(filename, "thread-[%d].txt", thread_node->thread_suffix);
	if(NULL == (fp = fopen(filename, "w+")))
	{
		printf("open file [%s] failed!\n", filename);
		pthread_exit(NULL);
	}

	i = 0;
	while(1)
	{
		pthread_mutex_lock(&mutex_object);
		f_obj = thread_out_queue(&thread_stru_num);
		pthread_mutex_unlock(&mutex_object); 

		/*queue is empty*/
		if(-1 == f_obj)
		{
			if(1 == out_flag)
			{
				break;
			}
			printf("queue is empty!\n");
			sleep(3);
		}

#ifdef PRINT_INFO
		printf("thread:[%d][%d]--[%d,%d,%d,%d,%d,%d,%d]\n", thread_node->thread_suffix,i++,thread_stru_num.red1,thread_stru_num.red2,\
				thread_stru_num.red3,thread_stru_num.red4,thread_stru_num.red5,thread_stru_num.red6,thread_stru_num.blue);
#endif		

		flag = thread_filter(conn, fp, &thread_stru_num);
		/*过滤掉，继续下一个*/
		if(1 == flag)
			continue;
		/*出错退出*/
		else if(-1 == flag)
			break;

		count_filter++;
		/*成功的号码组写入文件*/
		if(0 != write_log(fp, "1", &thread_stru_num, "success"))
		{
			printf("write log failed!\n");
			break;
		}
		fflush(fp);
	}


	sqlite3_close(conn);

	fclose(fp);
	
	return NULL;
}

int main(int argc, char * argv[])
{
	int i, ret, obj_flag;
	STRU_THREAD_NODE thread_node, *thread_cur, *thread_new;
	pthread_attr_t thread_attr;
	
	char *file = "caipiao.cfg";

	init_global_params();
	
	
	/*配置文件参数读入param_node结构体*/
	if((ret = get_conf_param(file)) != 0)
	{
		printf("get config param error!\n");
		return -1;
	}

	/*thread create*/
	thread_cur = &thread_node;
	thread_new = NULL;
	for(i = 0; i < THREAD_NUM; i++)
	{
		thread_new = (STRU_THREAD_NODE *)malloc(sizeof(STRU_THREAD_NODE));
		thread_new->thread_suffix = i;
		pthread_attr_init(&thread_attr);
		pthread_attr_setscope(&thread_attr, PTHREAD_SCOPE_SYSTEM);
		if(0 != pthread_create(&(thread_new->thread_id), (pthread_attr_t *)&thread_attr, cs_thread_process, (void *)thread_new))
		{
			printf("create thread fail [%d]\n", __LINE__);
			free(thread_new);
			exit(1);
		}
		thread_new->return_stat = 0;
		thread_new->waiting_i = 0;
		thread_new->next = NULL;
		thread_cur->next = thread_new;
		thread_cur = thread_new;
	}


	/*号码组放入队列*/
	if(0 != get_object_proc())
	{
		obj_flag = 1;
	}


	thread_cur = thread_node.next;
	while(thread_cur)
	{
		pthread_join(thread_cur->thread_id, NULL);
		thread_cur = thread_cur->next;
	}


#if 0
	/*thread control*/
	thread_cur = thread_node.next;
	active_num=0;
	while(thread_cur)
	{
		if(1 != thread_cur->waiting_i)
		{
			active_num = 1;
			break;
		}
		thread_cur = thread_cur->next;
	}

	if(1 == active_num)
	{
		printf("queue is empty!\n");
	}


	while(1)
	{
		/*线程监控，如果所有线程都停止运行，记录日志*/
		/*如果部分退出，则等待全部退出之后，主线程退出*/
		thread_cur = thread_node.next;
		active_num = 0;
		while(thread_cur)
		{   	
			if(thread_cur->return_stat == 0 || thread_cur->return_stat == 1)
			{
				active_num++;
			}
			thread_cur = thread_cur->next;
		}
		printf("active_num = %d\n", active_num);
		if(0 == active_num)
		{
			printf("all the thread have exited, now quit!\n");
			exit(1);
		}
		else if(1 == get_out || active_num < THREAD_NUM)  /*个别线程异常退出，其余暂时没有退出*/
		{
			sleep(2);
		}

		/*队列内号码组数量小于等于THREAD_QUEUE_MIN_NUM时开始入队列，否则继续处理*/
		if(THREAD_QUEUE_MIN_NUM < thread_get_queue_num())
		{
			sleep(2);
		}

	}/*while(1)*/
#endif	

	printf("count_all = %.0f\n", count_all);
	printf("count_after_filter = %.0f\n", count_filter);

	return 0;
}
