#include "caipiao.h"


/*���ˣ�����1��ʾ���벻�ϸ񣬷���0��ʾ�ϸ�1��ʾ���ϸ�-1����*/
int thread_filter(sqlite3 *conn, FILE *fp, THREAD_STRU_NUM *thread_stru_num)
{
	int ret;

	/*�жϺ������Ƿ���ֹ�*/
	ret = select_his_record_count(conn, thread_stru_num);
	if(ret != 0)
	{
		#ifdef PRINT_INFO
		printf("The number have found in history!\n");
		#endif
		#ifdef WRITE_LOG
		if(0 != write_log(fp, "0", thread_stru_num, "select_his_record_count"))
		{
			printf("write log failed!\n");
			return -1;
		}
		#endif
		return ret;
	}
	
	/*���˺�ֵ*/
	ret = filter_sum(thread_stru_num);
	if(ret != 0)
	{
		#ifdef PRINT_INFO
				printf("The sum of the number group not in vailid area!\n");
		#endif
		#ifdef WRITE_LOG
		if(0 != write_log(fp, "0", thread_stru_num, "filter_sum"))
		{
			printf("write log failed!\n");
			return -1;
		}
		#endif
		return ret;
	}
	
	/*����β����ֵ*/
	ret = filter_tail_sum(thread_stru_num);
	if(ret != 0)
	{
		#ifdef PRINT_INFO
				printf("The tail sum of the number group not in vailid area!\n");
		#endif
		#ifdef WRITE_LOG
		if(0 != write_log(fp, "0", thread_stru_num, "filter_tail_sum"))
		{
			printf("write log failed!\n");
			return -1;
		}
		#endif
		return ret;
	}
	
	
	/*����β������*/
	ret = filter_tail_group(thread_stru_num);
	if(ret != 0)
	{
		#ifdef PRINT_INFO
			printf("The tail group of the number group not in vailid area!\n");
		#endif
		#ifdef WRITE_LOG
		if(0 != write_log(fp, "0", thread_stru_num, "filter_tail_group"))
		{
			printf("write log failed!\n");
			return -1;
		}
		#endif
		return ret;
	}
	
	/*������ż*/
	ret = filter_odd_even(thread_stru_num);
	if(ret != 0)
	{
		#ifdef PRINT_INFO
		printf("The odd and number not in vailid area!\n");
		#endif
		#ifdef WRITE_LOG
		if(0 != write_log(fp, "0", thread_stru_num, "filter_odd_even"))
		{
			printf("write log failed!\n");
			return -1;
		}
		#endif
		return ret;
	}
	
	/*���˴�Ÿ���*/
	ret = filter_big(thread_stru_num);
	if(ret != 0)
	{
		#ifdef PRINT_INFO
		printf("The big number not in vailid area!\n");
		#endif
		#ifdef WRITE_LOG
		if(0 != write_log(fp, "0", thread_stru_num, "filter_big"))
		{
			printf("write log failed!\n");
			return -1;
		}
		#endif
		return ret;
	}
	
	/*��������*/
	ret = filter_prime(thread_stru_num);
	if(ret != 0)
	{
		#ifdef PRINT_INFO
		printf("The prime number not in vailid area!\n");
		#endif
		#ifdef WRITE_LOG
		if(0 != write_log(fp, "0", thread_stru_num, "filter_prime"))
		{
			printf("write log failed!\n");
			return -1;
		}
		#endif
		return ret;
	}
	
	/*������������ֵ*/
	ret = filter_consecutive(thread_stru_num);
	if(ret != 0)
	{
		#ifdef PRINT_INFO
		printf("The consecutive number not in vailid area!\n");
		#endif
		#ifdef WRITE_LOG
		if(0 != write_log(fp, "0", thread_stru_num, "filter_consecutive"))
		{
			printf("write log failed!\n");
			return -1;
		}
		#endif
		return ret;
	}
	
	/*������β��*/
	ret = filter_max_diff(thread_stru_num);
	if(ret != 0)
	{
		#ifdef PRINT_INFO
		printf("The max diff not in vailid area!\n");
		#endif
		#ifdef WRITE_LOG
		if(0 != write_log(fp, "0", thread_stru_num, "filter_max_diff"))
		{
			printf("write log failed!\n");
			return -1;
		}
		#endif
		return ret;
	}
	
	/*����acֵ*/
	ret = filter_ac_value(thread_stru_num);
	if(ret != 0)
	{
		#ifdef PRINT_INFO
		printf("The AC value not in vailid area!\n");
		#endif
		#ifdef WRITE_LOG
		if(0 != write_log(fp, "0", thread_stru_num, "filter_ac_value"))
		{
			printf("write log failed!\n");
			return -1;
		}
		#endif
		return ret;
	}
	
	/*�������ڹ�*/
	ret = filter_clg(conn, thread_stru_num);
	if(ret != 0)
	{
		#ifdef PRINT_INFO
		printf("The repit&close&lone number not in vailid area!\n");
		#endif
		#ifdef WRITE_LOG
		if(0 != write_log(fp, "0", thread_stru_num, "filter_clg"))
		{
			printf("write log failed!\n");
			return -1;
		}
		#endif
		return ret;
	}
	
	return 0;
}


/*�鿴�����Ƿ�����ʷ�н���¼�г��ֹ������س��´�����������-1*/
int select_his_record_count(sqlite3 *conn, THREAD_STRU_NUM *thread_stru_num)
{
	int row, count;
	sqlite3_stmt *stmt = NULL;
	char sqlstr[LEN] = "";
	
	sprintf(sqlstr, "select count(1) from lottery where red1 = %d and red2 = %d and red3 = %d and red4 = %d and red5 = %d and red6 = %d and blue = %d", \
			thread_stru_num->red1, thread_stru_num->red2, thread_stru_num->red3, thread_stru_num->red4, thread_stru_num->red5,thread_stru_num->red6, thread_stru_num->blue);

#ifdef PRINT_INFO
	printf("sqlstr = [%s]\n", sqlstr);
#endif

	if(SQLITE_OK != sqlite3_prepare_v2(conn, sqlstr, strlen(sqlstr), &stmt, NULL))
	{
		printf("sqlite3_prepare_v2 select failed!\n");
		if(stmt)
			sqlite3_finalize(stmt);
		return -1;
	}
	
	row = sqlite3_step(stmt);
	if(row == SQLITE_ROW)
	{
		count = sqlite3_column_int(stmt, 0);
	}

	sqlite3_finalize(stmt);
		
	return count;
}


/*���˺�ֵ��������Χ����1���ڷ�Χ�ڷ���0*/
int filter_sum(THREAD_STRU_NUM *thread_stru_num)
{
	int sum = 0;

	sum = thread_stru_num->red1 + thread_stru_num->red2 + thread_stru_num->red3 + thread_stru_num->red4 + thread_stru_num->red5 + thread_stru_num->red6;
	#ifdef PRINT_INFO
		printf("sum = %d\n", sum);
	#endif
	if(sum < param_node.sum_limit_low || sum > param_node.sum_limit_up)
		return 1;
		
	return 0;
}

/*����β����ֵ*/
int filter_tail_sum(THREAD_STRU_NUM *thread_stru_num)
{
	int sum = 0;

	sum = thread_stru_num->red1 % 10 + thread_stru_num->red2 % 10 + thread_stru_num->red3 % 10 + \
		thread_stru_num->red4 % 10 + thread_stru_num->red5 % 10 + thread_stru_num->red6 % 10;
	#ifdef PRINT_INFO
		printf("tail sum : %d\n", sum);
	#endif
	
	if(sum < param_node.tail_sum_limit_low || sum > param_node.tail_sum_limit_up)
		return 1;
	
	return 0;
}

/*����β������*/
int filter_tail_group(THREAD_STRU_NUM *thread_stru_num)
{
	int i;
	int tail_2_count; /*β���ظ���Ϊ2������*/
	int tail[10];
	
	memset(tail, 0, sizeof(tail));
	
	/*tail�����м�¼β����Ӧ�����ָ���*/
	tail[thread_stru_num->red1 % 10]++;
	tail[thread_stru_num->red2 % 10]++;
	tail[thread_stru_num->red3 % 10]++;
	tail[thread_stru_num->red4 % 10]++;
	tail[thread_stru_num->red5 % 10]++;
	tail[thread_stru_num->red6 % 10]++;
	
	/*tail����6��1��˵��û���ظ�β�ţ�β������Ϊ0��tail�������n��2��˵�����ظ�β��n�飬n��Ҫ���������ļ��е�β������*/
	/*tail�������3��˵����3��β����ͬ��ֱ�ӹ��˵�*/
	tail_2_count = 0;
	for(i = 0; i < 10; i++)
	{
		if(tail[i] == 2)
			tail_2_count++;
		else if(tail[i] > 2)
			return 1;
	}
	for(i = 0; i < param_node.tail_group_count; i++)
	{
		if(tail_2_count == param_node.tail_group[i])
			return 0;
	}

#if 0
	/*��ӡ��β���������˵��ĺ���*/
	printf("%d %d %d %d %d %d\n", thread_stru_num->red1, thread_stru_num->red2, thread_stru_num->red3, \
		thread_stru_num->red4, thread_stru_num->red5, thread_stru_num->red6);
	for(i = 0; i < 10; i++)
		printf("tail[%d] = %d\n", i, tail[i]);
	printf("\n");
#endif	
	
	return 1;
}

/*������ż*/
int filter_odd_even(THREAD_STRU_NUM *thread_stru_num)
{
	int i;
	int odd, even;
	odd = even = 0;
	
	if(thread_stru_num->red1 % 2 == 0)
		even++;

	if(thread_stru_num->red2 % 2 == 0)
		even++;

	if(thread_stru_num->red3 % 2 == 0)
		even++;
		
	if(thread_stru_num->red4 % 2 == 0)
		even++;
		
	if(thread_stru_num->red5 % 2 == 0)
		even++;
		
	if(thread_stru_num->red6 % 2 == 0)
		even++;	
	
	odd = 6 - even;
	
	for(i = 0; i < param_node.odd_count; i++)
	{
		if(odd == param_node.odd_number[i])
			return 0;
	}
	
	return 1;
}

/*���˴�С�ţ�����17����Ϊ����*/
int filter_big(THREAD_STRU_NUM *thread_stru_num)
{
	int i;
	int big = 0;
	if(thread_stru_num->red1 > 17)
		big++;
	if(thread_stru_num->red2 > 17)
		big++;
	if(thread_stru_num->red3 > 17)
		big++;
	if(thread_stru_num->red4 > 17)
		big++;
	if(thread_stru_num->red5 > 17)
		big++;
	if(thread_stru_num->red6 > 17)
		big++;
		
	for(i = 0; i < param_node.big_count; i++)
	{
		if(big == param_node.big_number[i])
			return 0;
	}
	return 1;
}

/*������������*/
int filter_prime(THREAD_STRU_NUM *thread_stru_num)
{
	int i, j;
	int prime = 0;
	
	int prime_list[11] = {2,3,5,7,11,13,17,19,23,29,31};

	for(i = 0; i < param_node.prime_count; i++)
	{
		for(j = 0; j < 11; j++)
		{
			if(prime_list[j] == thread_stru_num->red1)
				prime++;
			if(prime_list[j] == thread_stru_num->red2)
				prime++;
			if(prime_list[j] == thread_stru_num->red3)
				prime++;
			if(prime_list[j] == thread_stru_num->red4)
				prime++;
			if(prime_list[j] == thread_stru_num->red5)
				prime++;
			if(prime_list[j] == thread_stru_num->red6)
				prime++;
		}
		if(prime == param_node.prime_number[i])
			return 0;
	}
	return 1;
}

/*�������Ÿ�������������*/
int filter_consecutive(THREAD_STRU_NUM *thread_stru_num)
{
	int i = 0;
	int c21 = 0, c32 = 0, c43 = 0, c54 = 0;
	int cons_number = 0, cons_group = 0;
	int cons_number_flag = 0, cons_group_flag = 0;
	
	if(1 == thread_stru_num->red2 - thread_stru_num->red1)
	{
		c21 = 1;	/*�ڶ�������͵�һ������֮������*/
		cons_number++;
		cons_group++;
	}

	if(1 == thread_stru_num->red3 - thread_stru_num->red2)
	{
		c32 = 1;
		cons_number++;
		if(c21 != 1)	/*�����һ������͵ڶ������벻��������������+1*/
			cons_group++;
	}
	
	if(1 == thread_stru_num->red4 - thread_stru_num->red3)
	{
		c43 = 1;
		cons_number++;
		if(c32 != 1)
			cons_group++;
	}
	
	if(1 == thread_stru_num->red5 - thread_stru_num->red4)
	{
		c54 = 1;
		cons_number++;
		if(c43 != 1)
			cons_group++;
	}
	
	if(1 == thread_stru_num->red6 - thread_stru_num->red5)
	{
		cons_number++;
		if(c54 != 1)
			cons_group++;
	}
	
	for(i = 0; i < param_node.cons_number_count; i++)
	{
		if(cons_number == param_node.cons_number[i])
		{
			cons_number_flag = 1;
			break;
		}
	}
	
	for(i = 0; i < param_node.cons_group_count; i++)
	{
		if(cons_group == param_node.cons_group[i])
		{
			cons_group_flag = 1;
			break;
		}
	}
	
	if((cons_number_flag == 1) && (cons_group_flag == 1))
		return 0;
		
	return 1;
}

/*������β��*/
int filter_max_diff(THREAD_STRU_NUM *thread_stru_num)
{
	int diff;
	diff = thread_stru_num->red6 - thread_stru_num->red1;
	if((diff > param_node.max_diff_limit_up) || (diff < param_node.max_diff_limit_low))
		return 1;
	
	return 0;
}

/*����ACֵ*/
int filter_ac_value(THREAD_STRU_NUM *thread_stru_num)
{
	int i, j;
	int ac_value = 0;
	int ac[32];
	int arr[6] = {thread_stru_num->red1, thread_stru_num->red2, thread_stru_num->red3, thread_stru_num->red4, \
			thread_stru_num->red5, thread_stru_num->red6};
	
	memset(ac, 0, sizeof(ac));
	
	for(i = 0; i < 5; i++)
		for(j = i + 1; j < 6; j++)
		{
			ac[arr[j] - arr[i]] = 1;
		}
	
	for(i = 0; i < 32; i++)
		if(ac[i] == 1)
			ac_value++;
	
	ac_value -= 5;
	
	for(i = 0; i < param_node.ac_value_count; i++)
		if(ac_value == param_node.ac_value[i])
			return 0;
	
	return 1;
}

/*�������ڹº�*/
int filter_clg(sqlite3 *conn, THREAD_STRU_NUM *thread_stru_num)
{
	int i, j, ret;
	int repit =0, close = 0, lone = 0;
	int repit_flag = 0, close_flag = 0, lone_flag = 0;
	THREAD_STRU_NUM thread_last_num;
	int cur[6] = {thread_stru_num->red1, thread_stru_num->red2, thread_stru_num->red3, thread_stru_num->red4, \
			thread_stru_num->red5, thread_stru_num->red6};
	int last[6];
	
	ret = get_lottery_num(conn, param_node.number - 1, &thread_last_num);
	if(0 != ret)
	{
		printf("get lottery number: %d failed!\n", param_node.number - 1);
		return -1;
	}
	
	last[0] = thread_last_num.red1;
	last[1] = thread_last_num.red2;
	last[2] = thread_last_num.red3;
	last[3] = thread_last_num.red4;
	last[4] = thread_last_num.red5;
	last[5] = thread_last_num.red6;

	/*ͳ���غ�*/
	for(i = 0; i< 6; i++)
		for(j = 0; j < 6; j++)
		{
			if(cur[i] == last[j])
			{
				repit++;
				break;
			}
		}
	
	/*ͳ���ںţ�����Ѿ����غţ��Ͳ��ڵ����ں���*/
	for(i = 0; i< 6; i++)
		for(j = 0; j < 6; j++)
		{
			if(cur[i] == last[j])
				break;
			if(cur[i] - last[j] == 1)
			{
				if(cur[i] != last[j+1])
					close++;
				break;
			}
			if(cur[i] - last[j] == -1)
			{
				close++;
				break;
			}
		}

	/*�º�*/
	lone = 6 - repit - close;

	for(i = 0; i < param_node.repit_count; i++)
		if(repit == param_node.repit[i])
			repit_flag = 1;
	
	for(i = 0; i < param_node.close_count; i++)
		if(close == param_node.close[i])
			close_flag = 1;
	
	for(i = 0; i < param_node.lone_count; i++)
		if(lone == param_node.lone[i])
			lone_flag = 1;
			
#if 0
printf("%7d: %2d %2d %2d %2d %2d %2d %2d\n", thread_last_num.number, thread_last_num.red1, thread_last_num.red2, \
	thread_last_num.red3, thread_last_num.red4, thread_last_num.red5, thread_last_num.red6, thread_last_num.blue);

printf("%7d: %2d %2d %2d %2d %2d %2d %2d\n", thread_stru_num->number, thread_stru_num->red1, thread_stru_num->red2, \
	thread_stru_num->red3, thread_stru_num->red4, thread_stru_num->red5, thread_stru_num->red6, thread_stru_num->blue);

printf("repit: %d\t\tclose: %d\t\tlone:%d\n", repit, close, lone);
#endif	
	if((repit_flag == 1) && (close_flag == 1) && (lone_flag == 1))
		return 0;
	
	return 1;
}

/*��ȡָ��number��ŵĺ����飬�ҵ�����0��δ�ҵ�����1��������-1*/
int get_lottery_num(sqlite3 *conn, int number, THREAD_STRU_NUM *thread_last_num)
{
	int row;
	sqlite3_stmt *stmt = NULL;
	char sqlstr[LEN] = "";
	
	sprintf(sqlstr, "select red1,red2,red3,red4,red5,red6,blue from lottery where number = %d", number);
	
#ifdef PRINT_INFO
	printf("sqlstr = [%s]\n", sqlstr);
#endif


	if(SQLITE_OK != sqlite3_prepare_v2(conn, sqlstr, strlen(sqlstr), &stmt, NULL))
	{
		printf("sqlite3_prepare_v2 select failed!\n");
		if(stmt)
			sqlite3_finalize(stmt);
		return -1;
	}
	
	row = sqlite3_step(stmt);
	if(row == SQLITE_ROW)/*ִ�гɹ������з���ֵ*/
	{
//		printf("select %d ok!\n", number);
		thread_last_num->number = number;
		thread_last_num->red1 = sqlite3_column_int(stmt, 0);
		thread_last_num->red2 = sqlite3_column_int(stmt, 1);
		thread_last_num->red3 = sqlite3_column_int(stmt, 2);
		thread_last_num->red4 = sqlite3_column_int(stmt, 3);
		thread_last_num->red5 = sqlite3_column_int(stmt, 4);
		thread_last_num->red6 = sqlite3_column_int(stmt, 5);
		thread_last_num->blue = sqlite3_column_int(stmt, 6);
		sqlite3_finalize(stmt);
		return 0;
	}
	else
		return 1;
}