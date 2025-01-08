#include <cstdio>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include "ast/AST_NODE.hpp"
#include "translate/ir_generator.h"
#include "common/visualize.h"
#include "common/while_frame.h"
#include "symtable/symbol_table.h"
// #include "koopa.h"
// #include "headers/ir_tree.h"
// the header file can be found at https://github.com/pku-minic/koopa/blob/master/crates/libkoopa/include/koopa.h
// a implementation of the KOOPA definition.

class Visualize visual;

using namespace std;

extern FILE *yyin;
extern int yyparse(BaseAST *&ast);

int main(int argc, const char *argv[])
{
  // initialization
  std::cout << "hello world\n";
  visual.init_visualize();

  // compiler 模式 输入文件 -o 输出文件
  assert(argc == 5);
  auto mode = argv[1];
  auto input = argv[2];
  auto output = argv[4];
  cout << "mode: " << mode << endl;
  cout << "output file: " << output << endl;
  // init symbol table
  SymbolTable::InitTable();
  WhileFrame::init_frame();
  // 打开输入文件, 并且指定 lexer 在解析的时候读取这个文件
  yyin = fopen(input, "r");
  assert(yyin);

  // 调用 parser 函数, parser 函数会进一步调用 lexer 解析输入文件的
  BaseAST *ast;
  auto ret = yyparse(ast);
  assert(!ret);

  // 输出解析得到的 AST, 其实就是个字符串
  ast->Dump();
  cout << endl;

  Program p;
  BaseIRT *ir = NULL;

  p.Scan(ast, ir);
  if (ir == nullptr)
    cout << "error";
  SymbolTable::PrintConstStringDeclare();
  ir->Dump();
  cout << endl;

  visual.end_visualize();
  cout << visual.output();

  freopen(output, "w", stdout);
  SymbolTable::PrintConstStringDeclare();
  // std::cout
  //     << "declare void @putint(i32)\n"
  //     << "declare void @putch(i32)\n"
  //     << "declare void @putarray(i32, i32*)\n"
  //     << "declare i32 @getint()\n"
  //     << "declare i32 @getch()\n"
  //     << "declare i32 @getarray(i32*)\n"
  //     << "declare void @starttime()\n"
  //     << "declare void @stoptime()\n"
  //     << "declare void @putf(i8*, ...)\n"
  //     << "declare i32 @printf(i8*, ...)\n\n"
  //     << "declare void @putchar(i32)\n"
  //     << "declare i32 @getchar()\n";
  ir->Dump();
  return 0;
}