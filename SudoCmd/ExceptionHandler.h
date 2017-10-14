#ifndef EH_H_
#define EH_H_

#include <exception>
#include <string>
#include <iostream>
#pragma once
//使用枚举变量表示不同类型，End 表示是要生成数独终局 Solve表示要解决数独 Mode表示三种不同数独生成模式 Empty表示按照挖空数来生成数独游戏 
//Nothing 是默认值什么都不做
enum requests { End, Solve, Mode, Empty, Nothing };
typedef struct request
{
	//最后生成的数量
	int number;		
	//数独问题的文件路径
	std::string *filepath;
	//存是哪种模式
	int mode;
	//对于挖空的情况，最大空的数量
	int upper;
	//最小
	int lower;
	//是不是要求唯一解
	bool unique;
	//是哪种类型（这里默认有四种类型：1.生成终局-c 2.解决数独 -s 3.生成不同模式 -m -n 4.生成不同挖空数 -r -n （-u））
	requests req;
} request;
//具体函数的定义
/*
require: req 必须已经申请好了空间，而且已经初始化为0
*/
void paraHandle(int argc, char *argv[], request *req);
//参数太少的异常
class too_few_para :public std::exception
{
public :
	void what() {
		std::cout<<"two few parameters\n";
		std::cout << "only support: -c num / -s filepath /-r num1~num2 -n num (-u) / -m 1/2/3 -n num\n";
		exit(1);
	}
};
//格式错误的异常
class format_err :public std::exception
{
public:
	void what() {
		std::cout << "format err\n";
		std::cout << "only support: -c num / -s filepath /-r num1~num2 -n num (-u) / -m 1/2/3 -n num\n";
		exit(1);
	}
};
//最低值比最高值还大的异常
class lower_bigger_than_upper :public std::exception
{
public:
	void what() {
		std::cout << "lower must less than upper\n";

		exit(1);
	}
};
//参数组合错误异常
class combination_conflict :public std::exception {
public:
	void what() {
		std::cout<<"parameters combination error\n"; 
		std::cout << "only support: -c num / -s filepath /-r num1~num2 -n num (-u) / -m 1/2/3 -n num\n";
		exit(1);
	}
};
//数字超出范围的异常
class out_of_range :public std::exception
{
public:
	out_of_range(int upper, int lower)
	{
		this->upper = upper;
		this->lower = lower;
	}
	void what() 
	{
		std::cout << "out of range\n";
		std::cout << "please input a number from " << lower << " to " << upper << std::endl;
		exit(1);
	}
private:
	int upper;
	int lower;
};

#endif EH_H_