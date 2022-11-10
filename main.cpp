#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <algorithm>
#include "lexicalAnalysis.h"
#include "syntaxAnalysis.h"
#include "global.h"
using namespace std;
int main() {
    //--------------------词法分析-------------------------
    string resultPath = RES_PATH;
    string filename = SOURCE_PATH;
    string grammarPath = GRAMMAR_PATH;
    FILE* fp;                //文件指针
    //将结果输出至txt
    fp = fopen(filename.c_str(), "r");
    ifstream existPath(resultPath.c_str());
    if (existPath.good())
        remove(resultPath.c_str());
    ofstream ResultPath(resultPath);
    ResultPath.close();
    existPath.close();

    cout << "结果如下：" << endl;

    lexicalAnalysis(fp);        //词法分析
    fclose(fp);                    //关闭文件
    //--------------------语法分析-------------------------
    getP(grammarPath);//读取产生式
    //removeLeftRecursion();
    //VT.erase(unique(VT.begin(), VT.end()), VT.end());
    printP();
    judgeIfExistLeftRecursion();
    getAllFirst();
    cout << endl;
    getAllFollow();
    create_anaTable();
    //print_table();
    analyze();
}