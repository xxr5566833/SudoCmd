#include "stdafx.h"
#include "ExceptionHandler.h"
#include <iostream>
#include <string>
#include <fstream>
/*
require:	req空间已经申请好，且地址上的值都初始化为了0
*/

/*
提前设置好不同的参数形式
*/
const char *kS_End = "-c";
const char *kS_Solve = "-s";
const char *kS_Mode = "-m";
const char *kS_Empty = "-r";
const char *kS_Unique = "-u";
const char *kS_Number = "-n";

//这是一些常用的常量
const int kLower = 1;
const int kUpper = 100*10000;
const int kEmptyUpper = 55;
const int kEmptyLower = 20;
const int kGenerateGame = 10000;
const int kModeLower = 1;
const int kModeUpper = 3;
//这个是表示最大文件路径字符串的长度
const int kPathLength = 100;
//string转int  
int string2int(char *s, int upper, int lower)
{
	int sum = 0;
	int p = 0;
	if (s[p] == '\0')
	{
		throw format_err();
	}
	while (s[p] != '\0')
	{
		//非数字字符直接报错
		if (s[p] < '0' || s[p] > '9')
		{
			throw (format_err());
		}
		/*this if canot be ignored*/
		if (sum >= upper)
		{
			throw (out_of_range(upper, lower));
		}
		sum = sum * 10 + s[p] - '0';
		++p;
	}
	if (sum < lower) {
		throw (out_of_range(upper, lower));
	}
	if (sum > upper)
	{
		throw (out_of_range(upper, lower));
	}
	return sum;
}
//处理xx~xx 这种字符串的
void string2range(char* emptystr, int *lower, int *upper) {
	//先把字符串分成两部分，然后分别调用string2int
	char *temp1 = new char[strlen(emptystr)+1];
	char *temp2 = new char[strlen(emptystr)+1];
	int i = 0;
	while (emptystr[i] != '~')
	{
		if (emptystr[i] == '\0')
		{
			throw format_err();
		}
		temp1[i] = emptystr[i];
		++i;
	}
	temp1[i++] = '\0';
	int j = 0;
	while (emptystr[i] != '\0')
	{
		temp2[j] = emptystr[i];
		++i;
		++j;
	}
	temp2[j] = '\0';
	try
	{
		*lower = string2int(temp1, kEmptyUpper, kEmptyLower);
		*upper = string2int(temp2, kEmptyUpper, kEmptyLower);
	}
	catch (out_of_range &e)
	{
		e.what();
	}
	catch (format_err &e)
	{
		e.what();
	}
	if (*lower > *upper)
	{
		throw (lower_bigger_than_upper());
	}
	delete[]temp1;
	delete[]temp2;

}
/*
具体的检查思路：
错误可以分为①功能性参数（-c -m -n等）的组合错误②内容型参数（-n后面的数字，-m后面的123等）的内容错误
对于①，采用的检查错误的办法是，每个功能性参数对应一个flag，
	1.可以检测是否重复输入了这个功能性参数，具体办法就是在检测到是这个参数比如-c后，在设置c_flag之前，检查一下c_flag是不是已经被设置了
	2.可以检测是否组合错误，具体办法是：目前仅有四种功能性组合①-c代表的产生终局②-s代表的解决③-m和-n代表的产生不同模式的游戏④-r-n（和-u）代表的
	不同挖空数游戏，那么我在遍历完所有命令行参数后，检测这四种组合是否出现，如果都没出现，那么说明这个组合有问题，具体可以看下面
对于②，就是定义各种具体的异常，一旦检测到了，那么抛出异常就好
*/
void paraHandle(int argc, char *argv[], request *req) {

	req->req = Nothing;
	//表示每种参数是否出现
	bool c_flag = false;
	bool s_flag = false;
	bool n_flag = false;
	bool m_flag = false;
	bool r_flag = false;
	bool u_flag = false;
	if (argc == 1)
	{
		throw (too_few_para ());
	}
	//注意要从1开始，0默认是exe文件的路径
	for (int i = 1; i < argc; i++)
	{
		char *s = argv[i];
		//先检查是不是-u 因为它后面没有具体的内容
		if (strcmp(s, kS_Unique) == 0) {
			req->unique = true;
			if (u_flag)
				throw format_err();
			u_flag = true;
			continue;
		}
		++i;
		if ( argc <= i)
		{
			throw(too_few_para());
		}
		if (strcmp(s, kS_End) == 0)
		{
			char *numstr = argv[i];
			int number = 0;
			try {
				number = string2int(numstr, kUpper, kLower);
			}
			catch (format_err &e)
			{
				e.what();
			}
			catch (out_of_range &e) {
				e.what();
			}
			//设置好对应的内容
			req->number = number;
			//防止重复的-c
			if (c_flag)
				throw format_err();
			c_flag = true;
			req->req = End;
			continue;
		}
		if (strcmp(s, kS_Solve) == 0)
		{
			char *filepath = argv[i];
			std::fstream file(filepath, std::ios::in);
			if (!file)
			{
				throw FileNotExist(filepath);
			}
			req->filepath = new std::string(filepath);
			if (s_flag)
				throw format_err();
			s_flag = true;
			req->req = Solve;
			continue;
		}
		if (strcmp(s, kS_Number) == 0)
		{
			char *numstr = argv[i];
			int number = 0;
			try {
				number = string2int(numstr, kGenerateGame, kLower);
			}
			catch (format_err &e)
			{
				e.what() ;
			}
			catch (out_of_range &e) {
				e.what();
			}
			req->number = number;
			if (n_flag)
				throw format_err();
			n_flag = true;
			continue;
		}
		if (strcmp(s, kS_Mode) == 0)
		{
			char *modestr = argv[i];
			int level = 0;
			try
			{
				level = string2int(modestr, kModeUpper, kModeLower);
			}
			catch (format_err &e)
			{
				e.what();
			}
			catch (out_of_range &e) {
				e.what();
			}
			if (m_flag)
				throw format_err();
			m_flag = true;
			req->req = Mode;
			req->mode = level;
			continue;
		}
		if (strcmp(s, kS_Empty) == 0)
		{
			char *emptystr = argv[i];
			int upper = 0;
			int lower = 0;
			try
			{
				string2range(emptystr, &lower, &upper);
			}
			catch (lower_bigger_than_upper &e)
			{
				e.what();
			}
			req->upper = upper;
			req->lower = lower;
			if (r_flag)
				throw format_err();
			r_flag = true;
			req->req = Empty;
			continue;

		}
		else
		{
			throw format_err();
		}

	}
	/*1. -c -s only single,cannot with other para*/
	/*2. -m only with -n*/
	/*3. -r only with -n -u*/
	//只有可能是下面四种组合模式，这四种如果都不是，那么就说明这个组合有问题
	bool c_func = c_flag && !(s_flag || m_flag || r_flag || u_flag || n_flag);
	bool s_func = s_flag && !(c_flag || m_flag || r_flag || u_flag || n_flag);
	bool m_func = m_flag&&n_flag && !(c_flag || s_flag || r_flag || u_flag);
	bool r_func = r_flag&&n_flag && !(c_flag || s_flag || m_flag);
	if (!(c_func || s_func || m_func || r_func))
		throw combination_conflict();
	
}


