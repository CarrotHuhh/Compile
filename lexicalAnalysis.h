#include <iostream>
#include <string>
#include <stdio.h>
#include <fstream>
#include "symbol.h"
#include "tokens.h"
#include "global.h"

using namespace std;
/*****************************************************全局变量*****************************************************/
int code = 0;        //记录单词的种别码
const int MAX = 5;    //关键字数量
string token = "";    //用于存储单词
char keyWord[][10] = {"int", "void", "return", "const", "main"};
int NEXT = 0;//标志位
string lex_res[100][3];
int num_lex = 0;
/*******************************************************函数*******************************************************/
void print(tokens t) {
  string a = "";
  switch (t.kind) {
    case KW:a = "KW";
      break;
    case OP:a = "OP";
      break;
    case SE:a = "SE";
      break;
    case IDN:a = "IDN";
      break;
    case INT:a = "INT";
      break;
    default:break;
  }
  cout << t.symbol << "\t<" << a << "," << t.content << ">" << endl;
  lex_res[num_lex][0] = t.symbol;
  lex_res[num_lex][1] = a;
  lex_res[num_lex++][2] = t.content;
  ofstream f(RES_PATH, ios::app);
  if (f.good()) {
    f << t.symbol << "\t<" << a << "," << t.content << ">" << endl;
  }
  f.close();
}
tokens gettokens(string token, int code) {
  tokens t;
  string symbol = "";
  symbol.swap(token);
  int kind = 0;
  string content;
  switch (code) {
    case KW_INT:kind = KW;
      content = "1";
      break;
    case KW_VOID:kind = KW;
      content = "2";
      break;
    case KW_RETURN:kind = KW;
      content = "3";
      break;
    case KW_CONST:kind = KW;
      content = "4";
      break;
    case KW_MAIN:kind = KW;
      content = "5";
      break;
    case OP_ADD:kind = OP;
      content = "6";
      break;
    case OP_SUB:kind = OP;
      content = "7";
      break;
    case OP_MUL:kind = OP;
      content = "8";
      break;
    case OP_DIV:kind = OP;
      content = "9";
      break;
    case OP_MOD:kind = OP;
      content = "10";
      break;
    case OP_EONE:kind = OP;
      content = "11";
      break;
    case OP_GREATER:kind = OP;
      content = "12";
      break;
    case OP_LESS:kind = OP;
      content = "13";
      break;
    case OP_ETWO:kind = OP;
      content = "14";
      break;
    case OP_LOE:kind = OP;
      content = "15";
      break;
    case OP_GOE:kind = OP;
      content = "16";
      break;
    case OP_NE:kind = OP;
      content = "17";
      break;
    case OP_AND:kind = OP;
      content = "18";
      break;
    case OP_OR:kind = OP;
      content = "19";
      break;
    case SE_LB:kind = SE;
      content = "20";
      break;
    case SE_RB:kind = SE;
      content = "21";
      break;
    case SE_LBRACE:kind = SE;
      content = "22";
      break;
    case SE_RBRACE:kind = SE;
      content = "23";
      break;
    case SE_SEM:kind = SE;
      content = "24";
      break;
    case SE_COM:kind = SE;
      content = "25";
      break;
    case IDN:kind = IDN;
      content = symbol;
      break;
    case INT:kind = INT;
      content = symbol;
      break;
    default:kind = 0;
      content = "";
      break;
  }
  t.content = content;
  t.kind = kind;
  t.symbol = symbol;
  print(t);
  return t;
}

bool isKey(string token) {
  for (int i = 0; i < MAX; i++) {
    if (token.compare(keyWord[i]) == 0)
      return true;
  }
  return false;
}
int getKeyID(string token) {
  for (int i = 0; i < MAX; i++) {
    if (token.compare(keyWord[i]) == 0)
      return i + 1;
  }
  return -1;
}
bool isLetter(char letter) {
  if ((letter >= 'a' && letter <= 'z') || (letter >= 'A' && letter <= 'Z'))
    return true;
  return false;

}
bool isDigit(char digit) {
  if (digit >= '0' && digit <= '9')
    return true;
  return false;
}

void lexicalAnalysis(FILE *fp) {
  char ch = fgetc(fp);
  NEXT = 1;
  while (ch != EOF) {
    if (!NEXT) {
      ch = fgetc(fp);
    }
    if (ch == EOF)
      break;
    NEXT = 0;
    token = ch;
    if (ch == ' ' || ch == '\t' || ch == '\n')
      continue;

    else if (isLetter(ch)) {
      token = "";
      while (isLetter(ch) || isDigit(ch)) {
        token.push_back(ch);
        ch = fgetc(fp);
      }
      NEXT = 1;
      //fseek(fp, -1L, SEEK_CUR);//文件指针后退一个字节，即重新读取上述单词后的第一个字符
      if (isKey(token))
        code = getKeyID(token);//关键字
      else
        code = IDN;    //单词为标识符
    } else if (isDigit(ch)) {
      token = "";
      while (isDigit(ch)) {
        token.push_back(ch);
        ch = fgetc(fp);
      }
      code = INT;                //单词为整型
      NEXT = 1;
      //fseek(fp, -1L, SEEK_CUR);//文件指针后退一个字节，即重新读取常数后的第一个字符
    } else
      switch (ch) {
        /*运算符*/
        case '+': code = OP_ADD;
          break;
        case '-': code = OP_SUB;
          break;
        case '*': code = OP_MUL;
          break;
        case '/': code = OP_DIV;
          break;
        case '%': code = OP_MOD;
          break;
        case '=': ch = fgetc(fp);
          if (ch == '=') {
            token.push_back(ch);
            code = OP_EONE;
          } else {
            code = OP_ETWO;
            NEXT = 1;
          }
          break;
        case '|': ch = fgetc(fp);
          if (ch == '|') {
            token.push_back(ch);
            code = OP_OR;
          }
          break;
        case '&': ch = fgetc(fp);
          if (ch == '&') {
            token.push_back(ch);
            code = OP_AND;
          }
          break;
        case '>': ch = fgetc(fp);
          if (ch == '=') {
            token.push_back(ch);
            code = OP_GOE;
          } else {
            code = OP_GREATER;
            NEXT = 1;
          }
          break;
        case '<': ch = fgetc(fp);
          if (ch == '=') {
            token.push_back(ch);
            code = OP_LOE;
          } else {
            code = OP_LESS;
            NEXT = 1;
          }
          break;
        case '!': ch = fgetc(fp);
          if (ch == '=') {
            token.push_back(ch);
            code = OP_NE;
          }
          break;
        case '(': code = SE_LB;
          break;
        case ')': code = SE_RB;
          break;
        case '{': code = SE_LBRACE;
          break;
        case '}': code = SE_RBRACE;
          break;
        case ';': code = SE_SEM;
          break;
        case ',': code = SE_COM;
          break;
        default: code = 0;//未识别符号
      }
    gettokens(token, code);
  }
}
