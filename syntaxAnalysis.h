#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <stack>

using namespace std;
/*****************************************************全局变量*****************************************************/
//非终结符
vector<string> VN =
{
    "program", "compUnit", "decl", "funcDef", "constDecl",
    "varDecl", "bType", "constDef", "argConst", "constInitval",
    "constExp", "varDef", "argVarDecl", "argVarDef", "initVal",
    "exp", "funcType", "funcFParams", "block", "funcFParam",
    "argFunctionF", "blockItem", "stmt", "argExp", "assignExp",
    "lVal", "primaryExp", "number", "unaryOp", "unaryExp",
    "callFunc", "funcRParams", "funcRParam", "argFunctionR", "mulExp",
    "mulExpAtom", "addExp", "addExpAtom", "relExp", "relExpAtom",
    "eqExp", "eqExpAtom", "assignExpAtom"
};

//终结符
vector<string> VT =
{
    "$", "const", ";", ",", "=",
    "int", "(", ")", "void", "{",
    "}", "return", "INT", "+", "-",
    "!", "*", "/", "%", "<",
    ">", "<=", ">=", "==", "!=",
    "Ident"
};

unordered_map<string, vector<vector<string>>> P;//产生式
unordered_map<string, vector<string>> First;//First集
unordered_map<string, vector<string>> Follow;//Follow集
unordered_map<string, unordered_map<string, vector<string>>> ana_table;//分析表
int len_vt = VT.size();//终结符数量
int len_vn = VN.size();//非终结符数量
/*******************************************************函数*******************************************************/
//读取并存储产生式
void getP(string grammar_path) {
    ifstream fp;
    fp.open(grammar_path, ios::in);

    while (!fp.eof()) {
        string oneP;
        getline(fp, oneP);

        //去除空格
        char* s = new char[oneP.size() + 1];
        strcpy(s, oneP.c_str());
        char* p = strtok(s, " ");

        //将去除空格的符号存入
        vector<string> words;
        while (p) {
            words.push_back(p);
            p = strtok(NULL, " ");
        }

        words.erase(words.begin() + 1);
        string left = words.front();
        words.erase(words.begin());

        P[left].push_back(words);
    }
    fp.close();
}

//打印产生式
void printP() {
    for (auto p : P) {
        for (auto pRight : p.second) {
            cout << p.first + " -> ";
            for (auto once : pRight) {
                cout << once + " ";
            }
            cout << endl;
        }
        cout << endl;
    }

    cout << P.size() << endl;
    cout << endl;
}

//消除左递归
void removeLeftRecursion() {
    for (auto& p : P) {
        if (p.second.back().front() == p.first) {
            string newP = p.first + '\'';
            p.second.front().push_back(newP);
            VN.push_back(newP);
            VT.push_back("$");

            p.second.back().erase(p.second.back().begin());
            p.second.back().push_back(newP);
            vector<string> onceNew;
            onceNew = p.second.back();

            vector<vector<string>> newRight;
            vector<string> empty = { "$" };
            newRight.push_back(onceNew);
            newRight.push_back(empty);

            P[newP] = newRight;
            p.second.pop_back();
        }
    }
}

//判断是否存在左递归
void judgeIfExistLeftRecursion() {
    int cnt = 0;
    for (auto p : P) {
        for (auto onceRight : p.second) {
            if (onceRight.front() == p.first) {
                cnt++;
                cout << p.first + " -> ";
                for (auto word : onceRight) {
                    cout << word + " ";
                }
                cout << "存在左递归" << endl;
            }
        }
    }

    if (!cnt) {
        cout << "无需消除左递归" << endl;
    }
}

//生成非终结符的First集
void getFirst(string p, vector<string>& onceFirst) {
    for (auto onceRight : P[p]) {
        auto tempFirst = onceRight.front();
        int cnt = 0;

        for (auto vt : VT) {
            if (tempFirst == vt) {
                onceFirst.push_back(tempFirst);
            }
            else {
                cnt++;
                if (cnt == VT.size()) {
                    getFirst(tempFirst, onceFirst);
                }
            }
        }
    }
}

//求所有非终结符的First集
void getAllFirst() {
    vector<string> onceFirst;
    int firstCnt = 0;

    for (auto p : P) {
        int cnt = 0;
        cout << "First(" + p.first + ") = { ";

        getFirst(p.first, onceFirst);
        sort(onceFirst.begin(), onceFirst.end());
        onceFirst.erase(unique(onceFirst.begin(), onceFirst.end()), onceFirst.end());
        firstCnt++;

        for (auto first : onceFirst) {
            cnt++;
            cout << first;

            if (cnt != onceFirst.size()) {
                cout << ", ";
            }
            else {
                cout << " }";
            }
        }

        cout << endl;
        First[p.first] = onceFirst;
        onceFirst.clear();
    }
}

//生成非终结符的Follow集
void getFollow(string pLeft, vector<string>& onceFollow) {
    for (auto p : P) {
        if (p.first == pLeft) {
            continue;
        }
        if (pLeft == "program") {
            onceFollow.push_back("#");
            break;
        }

        for (auto onceRight : p.second) {
            for (auto once : onceRight) {

                //在产生式右侧找到该非终结符
                if (pLeft == once) {
                    vector<string>::iterator it;

                    it = find(onceRight.begin(), onceRight.end(), once);
                    int flag = it - onceRight.begin();

                    //如果不位于产生式末尾
                    if (flag < onceRight.size() - 1) {
                        auto nextone = onceRight[flag + 1];

                        //下一位是终结符
                        if (count(VT.begin(), VT.end(), nextone) == 1) {
                            onceFollow.push_back(nextone);
                        }
                        //下一位是非终结符
                        else {
                            int flag = 0;
                            for (auto addFromFirst : First[nextone]) {
                                if (addFromFirst != "$") {
                                    onceFollow.push_back(addFromFirst);
                                }
                                flag++;
                            }

                            if (flag) {
                                if (Follow[p.first].empty()) {
                                    getFollow(p.first, onceFollow);
                                }
                                else {
                                    for (auto addFromLeftFollow : Follow[p.first]) {
                                        onceFollow.push_back(addFromLeftFollow);
                                    }
                                }
                            }
                        }
                    }
                    //如果位于产生式末尾
                    else if (flag == (onceRight.size() - 1)) {
                        if (Follow[p.first].empty()) {
                            getFollow(p.first, onceFollow);
                        }
                        else {
                            for (auto addFromLeftFollow : Follow[p.first]) {
                                onceFollow.push_back(addFromLeftFollow);
                            }
                        }
                    }
                }
            }
        }
    }
}

//求所有非终结符的Follow集
void getAllFollow() {
    vector<string> onceFollow;
    int followCnt = 0;

    for (auto p : P) {
        int cnt = 0;
        cout << "Follow(" + p.first + ") = { ";

        getFollow(p.first, onceFollow);
        sort(onceFollow.begin(), onceFollow.end());
        onceFollow.erase(unique(onceFollow.begin(), onceFollow.end()), onceFollow.end());
        followCnt++;

        for (auto follow : onceFollow) {
            cnt++;
            cout << follow;

            if (cnt != onceFollow.size()) {
                cout << ", ";
            }
        }
        cout << " }" << endl;
        Follow[p.first] = onceFollow;
        onceFollow.clear();
    }
}

//创建预测表
void create_anaTable() {
    //初始化创建空表
    for (int i = 0; i < len_vn; i++) {
        for (int j = 0; j < len_vt; j++) {
            vector<string> tmp_vec = { "" };
            ana_table[VN[i]][VT[j]] = tmp_vec;
        }
    }
    //遍历产生式
    for (auto p : P) {
        for (auto pRight : p.second) {
            string prFirst = pRight[0];
            //若为终结符，则直接将产生式填入对应表格
            if (count(VT.begin(), VT.end(), prFirst)) {
                ana_table[p.first][prFirst] = pRight;
                if (prFirst == "$") {
                    for (auto meta_fo : Follow[p.first]) {
                        vector<string> tmpv = { "$" };
                        vector<string> cmpv = { "" };
                        //避免对已有产生式的表格进行覆盖
                        if (ana_table[p.first][meta_fo] == cmpv)
                            ana_table[p.first][meta_fo] = tmpv;
                    }
                }
            }
            else {
                //若产生式的First集中包含空串，则可推导至Follow集中的终结符
                if (count(First[prFirst].begin(), First[prFirst].end(), "$")) {
                    for (auto meta_fo : Follow[prFirst]) {
                        vector<string> tmpv = { "$" };
                        vector<string> cmpv = { "" };
                        //避免对已有产生式的表格进行覆盖
                        if (ana_table[p.first][meta_fo] == cmpv)
                            ana_table[p.first][meta_fo] = tmpv;
                    }
                }
                for (auto meta : First[prFirst])
                    ana_table[p.first][meta] = pRight;
            }
        }
    }
}

//打印预测表
void print_table() {
    cout << "=============================预测分析表=============================" << endl;
    for (int i = 0; i < len_vt; i++)
        cout << "\t\t\t\t" << VT[i];
    cout << endl;
    for (auto ta : ana_table) {
        cout
            << "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------"
            << endl;
        cout << ta.first + ":";
        for (int i = 0; i < len_vt; i++) {
            string vt = VT[i];
            auto obj = ta.second[vt];
            for (auto str : obj) {
                if (str.length())
                    cout << str + "\t";
                else
                    cout << "\t\t\t\t";
            }
            cout << "|\t";
        }
        cout << endl;
    }
}

//对词法分析结果进行预处理
void lex_res_process() {

    for (int i = 0; i < num_lex; i++) {
        if (count(VT.begin(), VT.end(), lex_res[i][0]) == 0)
            lex_res[i][0] = lex_res[i][1];
        if (lex_res[i][1] == "IDN")
            lex_res[i][0] = "Ident";
    }
}

//根据预测表进行分析
void analyze() {
    //初始化
    ofstream f(RES_PATH, ios::app);
    if (f.good()) {
        f << "\n\n\n";
    }
    f.close();
    lex_res_process();
    int index = 1; //语法分析输出序号
    stack<string> op_stack;//符号栈
    stack<string> input_stack;//输入串栈
    op_stack.push("#");
    op_stack.push(VN[0]);
    input_stack.push("#");
    //倒序将词法分析结果压入栈中
    for (int i = num_lex - 1; i >= 0; i--) {
        input_stack.push(lex_res[i][0]);
    }

    while (!input_stack.empty()) {
        string origin = input_stack.top();
        string top_input = origin;
        //空串直接推出
        if (op_stack.top() == "$")
            op_stack.pop();
        string top_op = op_stack.top();

        //分析
        //输入串已空
        if (top_input == "#") {
            //若符号栈也空，则分析完成，确认接收
            if (top_op == "#") {
                ofstream f(RES_PATH, ios::app);
                if (f.good()) {
                    f << to_string(index) << "\tEOF#EOF\taccept\n";
                }
                f.close();
                cout << to_string(index) << "\tEOF#EOF\taccept\n";
                input_stack.pop();
                op_stack.pop();
                break;
            }
            //若符号栈未空
            else {
                //若栈顶符号为终结符，则分析失败
                if (count(VT.begin(), VT.end(), top_op)) {
                    ofstream f(RES_PATH, ios::app);
                    if (f.good()) {
                        f << to_string(index) + "\t" + top_op + "#" + origin + "\terror\n";
                    }
                    f.close();
                    cout << to_string(index) + "\t" + top_op + "#" + origin + "\terror\n";
                    break;
                }
                //栈顶符号无法推导至空串，则分析失败
                else if (ana_table[top_op]["$"][0] == "") {
                    ofstream f(RES_PATH, ios::app);
                    if (f.good()) {
                        f << to_string(index) + "\t" + top_op + "#" + origin + "\terror\n";
                    }
                    f.close();
                    cout << to_string(index) + "\t" + top_op + "#" + origin + "\terror\n";
                    break;
                }
                //栈顶符号可推导至空串，则进行规约
                else {
                    ofstream f(RES_PATH, ios::app);
                    if (f.good()) {
                        f << to_string(index) + "\t" + top_op + "#" + origin + "\treduction\n";
                    }
                    f.close();
                    cout << to_string(index++) + "\t" + top_op + "#" + origin + "\treduction\n";
                    vector<string> temp_vec(ana_table[top_op]["$"]);
                    op_stack.pop();
                    if (temp_vec.size() > 1) {
                        for (auto it = temp_vec.rbegin(); it != temp_vec.rend(); ++it) {
                            op_stack.push(*it);
                        }
                    }
                    else {
                        op_stack.push(temp_vec[0]);
                    }
                }
            }
        }
        else if ((count(VT.begin(), VT.end(), top_op) == 1) && (top_op != top_input)) {
            ofstream f(RES_PATH, ios::app);
            if (f.good()) {
                f << to_string(index) + "\t" + top_op + "#" + origin + "\terror\n";
            }
            f.close();
            cout << to_string(index) + "\t" + top_op + "#" + origin + "\terror\n";
            break;
        }
        else if (top_op == top_input) {
            ofstream f(RES_PATH, ios::app);
            if (f.good()) {
                f << to_string(index) + "\t" + top_op + "#" + origin + "\tmove\n";
            }
            f.close();
            cout << to_string(index++) + "\t" + top_op + "#" + origin + "\tmove\n";
            op_stack.pop();
            input_stack.pop();
        }
        else if (ana_table[top_op][top_input][0] == "") {
            ofstream f(RES_PATH, ios::app);
            if (f.good()) {
                f << to_string(index) + "\t" + top_op + "#" + origin + "\terror\n";
            }
            f.close();
            cout << to_string(index) + "\t" + top_op + "#" + origin + "\terror\n";
            break;
        }
        else {
            ofstream f(RES_PATH, ios::app);
            if (f.good()) {
                f << to_string(index) + "\t" + top_op + "#" + origin + "\treduction\n";
            }
            f.close();
            cout << to_string(index++) + "\t" + top_op + "#" + origin + "\treduction\n";
            vector<string> temp_vec(ana_table[top_op][top_input]);
            op_stack.pop();
            if (temp_vec.size() > 1) {
                for (auto it = temp_vec.rbegin(); it != temp_vec.rend(); ++it) {
                    op_stack.push(*it);
                }
            }
            else {
                op_stack.push(temp_vec[0]);
            }
        }
    }
}


