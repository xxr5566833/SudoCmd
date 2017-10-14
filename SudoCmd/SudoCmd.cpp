// SudoCmd.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ExceptionHandler.h"
#include <Windows.h>
#include <iostream>
#include <fstream>

using namespace std;


int main(int argc, char* argv[])
{
    request *req = new request();
    memset(req, 0, sizeof(request));
    try {
        paraHandle(argc, argv, req);
    }
    catch (too_few_para &e) {
        e.what();
    }
    catch (format_err &e) {
        e.what();
    }
    catch (combination_conflict &e) {
        e.what();
    }

    HMODULE CoreDLL = LoadLibrary(TEXT("Core/SoduCore.dll"));
    if (!CoreDLL) {
        std::cout << "Core/SudoCore.dll not exist" << std::endl;
        exit(-1);
    }

    char cache[19 * 9 + 2];
    for (int i = 0; i < 19 * 9; i++) {
        if (i % 19 == 18)cache[i] = '\n';
        else cache[i] = ' ';
    }
    cache[19 * 9] = '\n';
    cache[19 * 9 + 1] = 0;

    switch (req->req) {
    case End: {
        typedef void(*GENERATE_C) (int, int**);
        GENERATE_C generate_c = (GENERATE_C)GetProcAddress(CoreDLL, "generate_c");
        int **result = new int*[req->number];
        for (int i = 0; i < req->number; i++)
            result[i] = new int[81];
        generate_c(req->number, result);
        ofstream fout("sudoku.txt");

        for (int i = 0; i < req->number; i++) {
            for (int j = 0; j < 81; j++)
                cache[j / 9 * 19 + j % 9 * 2] = result[i][j] + '0';
            fout << cache;
        }
        fout.close();
        break; 
    }
    case Solve: {
        typedef bool(*SOLVE_S) (int *, int *);
        SOLVE_S solve_s = (SOLVE_S)GetProcAddress(CoreDLL, "solve_s");
        ifstream fin(*(req->filepath));
        ofstream fout("sudoku.txt");
        int n;
        int count = 0;
        int puzzle[81];
        int solution[81];
        while (fin >> n) {
            puzzle[count++] = n;
            if (count == 81) {
                count = 0;
                solve_s(puzzle, solution);
                for (int i = 0; i < 81; i++)
                    cache[i / 9 * 19 + i % 9 * 2] = solution[i] + '0';
                fout << cache;
            }
        }
        fin.close();
        fout.close();
        break; 
    }
    case Mode: {
        typedef void(*GENERATE_M)(int, int, int**);
        GENERATE_M generate_m = (GENERATE_M)GetProcAddress(CoreDLL, "generate_m");
        ofstream fout("sudoku.txt");
        int **result = new int*[req->number];
        for (int i = 0; i < req->number; i++)
            result[i] = new int[81];
        generate_m(req->number, req->mode, result);
        for (int i = 0; i < req->number; i++) {
            for (int j = 0; j < 81; j++)
                cache[j / 9 * 19 + j % 9 * 2] = result[i][j] + '0';
            fout << cache;
        }
        fout.close();
        break;
    }
    case Empty: {
        typedef void(*GENERATE_R)(int, int, int, bool, int**);
        GENERATE_R generate_r = (GENERATE_R)GetProcAddress(CoreDLL, "generate_r");
        ofstream fout("sudoku.txt");
        int **result = new int*[req->number];
        for (int i = 0; i < req->number; i++)
            result[i] = new int[81];
        generate_r(req->number, req->lower,req->upper, req->unique, result);
        for (int i = 0; i < req->number; i++) {
            for (int j = 0; j < 81; j++)
                cache[j / 9 * 19 + j % 9 * 2] = result[i][j] + '0';
            fout << cache;
        }
        fout.close();
        break;
    }
    default:
        std::cout << "unknown error!" << std::endl;
        exit(-1);
    }
    FreeLibrary(CoreDLL);
    return 0;
}

