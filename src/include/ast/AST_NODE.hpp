#ifndef __AST_H__
#define __AST_H__

#include <iostream>
#include <memory>
#include <string>
#include <list>
#include <vector>
#include "symtable/symbol_table.h"
#include "common/initvalT.h"
#include "common/base_ast.hpp"
#include "common/enum.h"
#include "common/visualize.h"

extern class Visualize visual;
class NumberAST;

class NumberAST : public BaseAST
{
public:
  enum class NumberType
  {
    Int,
    Float
  } numType;
  int intNum;
  float floatNum;

  void Dump() const override
  {
    std::cout << "NumberAST {\n";
    if (numType == NumberType::Int)
    {
      std::cout << intNum;
    }
    else
    {
      std::cout << floatNum;
    }
    std::cout << "\n}(NumberAST ends) ";
  }

  std::string type(void) const override
  {
    std::unique_ptr<std::string> TypeStr(new std::string("NumberAST"));
    return *TypeStr;
  }

  bool GetConstVal(int &val) const override
  {
    if (numType == NumberType::Int)
    {
      val = this->intNum;
      return false;
    }
    return true; // Float type does not directly return an int value
  }

  bool GetConstVal(float &val) const override
  {
    if (numType == NumberType::Float)
    {
      val = this->floatNum;
      return true;
    }
    return false;
  }
};

class CompUnitAST : public BaseAST
{
public:
  BaseAST *comp_unit;
  void Dump() const override
  {
    std::cout << "CompUnitAST {\n";
    visual.add_pair(std::string("CompUnitAST"), std::string("CompunitAST"), false);
    comp_unit->Dump();
    std::cout << "\n}(CompUnitAST ends) ";
  }
  std::string type(void) const override
  {
    std::unique_ptr<std::string> TypeStr(new std::string("CompUnitAST"));
    return *TypeStr;
  }
};

class CompunitAST : public BaseAST
{
public:
  std::vector<BaseAST *> DeclList;
  void Dump() const override
  {
    int idx_visual = 0;
    for (auto &it : DeclList)
    {
      visual.add_pair(std::string("CompunitAST"), std::string("FuncDefAST_instance_") + std::to_string(idx_visual), true);
      it->Dump();
      idx_visual++;
    }
  }
  std::string type(void) const override
  {
    std::unique_ptr<std::string> TypeStr(new std::string("CompunitAST"));
    return *TypeStr;
  }
};

class DeclAST : public BaseAST
{
public:
  AstType tp;
  BaseAST *const_or_var_decl;
  void Dump() const override
  {
    std::cout << "DeclAST {\n";
    const_or_var_decl->Dump();
    std::cout << "\n}(DeclAST ends) ";
  }
  std::string type(void) const override
  {
    std::unique_ptr<std::string> TypeStr(new std::string("DeclAST"));
    return *TypeStr;
  }
};
class VarDeclAST : public BaseAST
{
public:
  VarType Type;
  std::vector<BaseAST *> VarDefVec;
  void Dump() const override
  {
    std::cout << "VarDeclAst {\n";
    std::cout << "vartype:" << EnumToString(Type);
    for (auto &it : VarDefVec)
    {
      it->Dump();
    }
    std::cout << "\n}(VarDeclAST ends) ";
  }

  void HandleSymbol() const;
};

class VarDefAST : public BaseAST
{
public:
  std::string *VarIdent;
  std::vector<BaseAST *> DimSizeVec;
  bool IsInited;
  InitValTree<BaseAST *> *InitValue;
  Symbol *VarSym;
  void Dump() const override
  {
    std::cout << *VarIdent;
    for (auto &it : DimSizeVec)
    {
      std::cout << "[";
      it->Dump();
      std::cout << "]";
    }
    std::cout << " = "
              << "{ ";
    if (InitValue != NULL)
      InitValue->Dump();
    std::cout << "}";
  }
};
class ConstDeclAST : public BaseAST
{
public:
  VarType Type;
  std::vector<BaseAST *> ConstDefVec;
  void Dump() const override
  {
    std::cout << "DeclAST {\n"
              << "vartype:" << EnumToString(Type);
    for (auto &it : ConstDefVec)
    {
      it->Dump();
    }
    std::cout << "\n}(DeclAST ends) ";
  }
  std::string type(void) const override
  {
    std::unique_ptr<std::string> TypeStr(new std::string("ConstDeclAST"));
    return *TypeStr;
  }

  void HandleSymbol() const;
};

class FuncDefAST : public BaseAST
{
public:
  VarType func_type;
  std::string *ident;
  BaseAST *func_fparams;
  BaseAST *block;
  void Dump() const override
  {
    std::cout << "FuncDefAST {\n";
    std::string instance_name;
    if (int t = visual.find_map(std::string("CompunitAST"), instance_name))
    {
      std::string id = std::string("FunctionId_") + *ident;
      visual.add_pair(std::string(instance_name), std::string("FuncTypeAST_") + std::string(*ident), false);
      visual.add_pair(std::string(instance_name), std::string(id), false);
      visual.add_pair(std::string(instance_name), std::string("FuncFParamsAST_") + std::string(*ident), false);
      visual.add_pair(std::string(instance_name), std::string("Block_") + std::string(*ident), false);
      visual.remove_map(std::string("CompunitAST"), instance_name);
    }
    std::cout << ", " << *ident << ", ";
    if (func_fparams != nullptr)
      func_fparams->Dump();
    block->Dump();
    std::cout << "\n}(FuncDefAST ends) ";
  }
  std::string type(void) const override
  {
    std::unique_ptr<std::string> TypeStr(new std::string("FuncDefAST"));
    return *TypeStr;
  }
};

class FuncTypeAST : public BaseAST
{
public:
  std::string type_ret;
  void Dump() const override
  {
    std::cout << "FuncTypeAST {\n";
    std::cout << type_ret;
    std::cout << "\n}(FuncTypeAST ends) ";
  }
  std::string type(void) const override
  {
    std::unique_ptr<std::string> TypeStr(new std::string("FuncTypeAST"));
    return *TypeStr;
  }
};
class FuncFParamsAST : public BaseAST
{
public:
  std::vector<BaseAST *> func_fparam;
  void Dump() const override
  {
    std::cout << "FuncFParamsAST {\n";
    for (auto &it : func_fparam)
    {
      it->Dump();
    }
    std::cout << "\n}(FuncFParamsAST ends) ";
  }
  std::string type(void) const override
  {
    std::unique_ptr<std::string> TypeStr(new std::string("FuncFParamsAST"));
    return *TypeStr;
  }
};

class FuncFParamAST : public BaseAST
{
public:
  ArgsType tp;
  VarType Btype;
  std::string *ident;
  std::vector<int> dimension;
  bool EmitHighestDimFlag;

  void Dump() const override
  {
    std::cout << "FuncFParamAST {\n";
    if (tp == ArgsType::Int32)
    {
      std::cout << "Btype:" << EnumToString(Btype) << ", " << *ident;
    }
    else if (tp == ArgsType::Int32Array)
    {
      // 打印 Int32 数组类型的逻辑
      std::cout << "Btype:Int32Array, " << *ident << ", dimensions: [";
      for (size_t i = 0; i < dimension.size(); ++i)
      {
        std::cout << dimension[i];
        if (i < dimension.size() - 1)
          std::cout << ", ";
      }
      std::cout << "]";
    }
    else if (tp == ArgsType::Float32)
    {
      std::cout << "Btype:Float32, " << *ident;
    }
    else if (tp == ArgsType::Float32Array)
    {
      // 打印 Float32 数组类型的逻辑
      std::cout << "Btype:Float32Array, " << *ident << ", dimensions: [";
      for (size_t i = 0; i < dimension.size(); ++i)
      {
        std::cout << dimension[i];
        if (i < dimension.size() - 1)
          std::cout << ", ";
      }
      std::cout << "]";
    }
    std::cout << "\n}(FuncFParamAST ends) ";
  }

  std::string type(void) const override
  {
    std::unique_ptr<std::string> TypeStr(new std::string("FuncFParamAST"));
    return *TypeStr;
  }
};

class StringAST : public BaseAST
{
public:
  std::string StringLabel;

  void Dump() const override
  {
    std::cout << StringLabel;
  }
  std::string type(void) const override
  {
    std::unique_ptr<std::string> TypeStr(new std::string("StringAST"));
    return *TypeStr;
  }
};

class CallParamsAST : public BaseAST
{
public:
  std::vector<BaseAST *> exp;
  void Dump() const override
  {
    std::cout << "CallParamsAST {\n";
    for (auto &it : exp)
    {
      it->Dump();
    }
    std::cout << "\n}(CallParamsAST ends) ";
  }
  std::string type(void) const override
  {
    std::unique_ptr<std::string> TypeStr(new std::string("CallParamsAST"));
    return *TypeStr;
  }
};

class BlockAST : public BaseAST
{
public:
  BaseAST *block;
  void Dump() const override
  {
    std::cout << "BlockAST {\n";
    if (block != nullptr)
      block->Dump();
    std::cout << "\n}(BlockAST ends) ";
  }
  std::string type(void) const override
  {
    std::unique_ptr<std::string> TypeStr(new std::string("BlockAST"));
    return *TypeStr;
  }
};

class blockAST : public BaseAST
{
public:
  std::vector<BaseAST *> block_item;
  void Dump() const override
  {
    for (auto &it : block_item)
    {
      it->Dump();
    }
  }
  std::string type(void) const override
  {
    std::unique_ptr<std::string> TypeStr(new std::string("blockAST"));
    return *TypeStr;
  }
};

class BlockItemAST : public BaseAST
{
public:
  std::string tp;
  BaseAST *decl_or_stmt;
  void Dump() const override
  {
    std::cout << "BlockItemAST {\n";
    decl_or_stmt->Dump();
    std::cout << "\n}(BlockItemAST ends) ";
  }
  std::string type(void) const override
  {
    std::unique_ptr<std::string> TypeStr(new std::string("BlockItemAST"));
    return *TypeStr;
  }
};
class AssignAST : public BaseAST
{
public:
  BaseAST *LVal;
  BaseAST *ValueExp;
  void Dump() const override
  {
    std::cout << "AssignAST {\n";
    LVal->Dump();
    std::cout << ", ";
    ValueExp->Dump();
    std::cout << "\n}(AssignAST ends) ";
  }
  std::string type(void) const override
  {
    std::unique_ptr<std::string> TypeStr(new std::string("AssignAST"));
    return *TypeStr;
  }
};
class LValAST : public BaseAST
{
public:
  std::string *VarIdent;
  std::vector<BaseAST *> IndexVec;
  void Dump() const override
  {
    std::cout << "LValAST {\n";
    std::cout << *VarIdent;
    for (auto &it : IndexVec)
    {
      std::cout << "[";
      it->Dump();
      std::cout << "]";
    }
    std::cout << "\n}(LValAST ends) ";
  }
  std::string type(void) const override
  {
    std::unique_ptr<std::string> TypeStr(new std::string("LValAST"));
    return *TypeStr;
  }

  bool GetConstVal(int &val) const override;
  bool GetConstVal(float &val) const override;
};

class StmtAST : public BaseAST
{
public:
  StmtType tp;
  BaseAST *ret_exp;
  BaseAST *cond_exp;
  BaseAST *stmt_if;
  BaseAST *stmt_else;
  BaseAST *stmt_while;
  void Dump() const override
  {
    std::cout << "StmtAST {\n";
    if (tp == StmtType::ReturnExp)
    {
      std::cout << EnumToString(tp);
      ret_exp->Dump();
    }
    else if (tp == StmtType::If)
    {
      std::cout << EnumToString(tp);
      std::cout << EnumToString(tp);
      std::cout << std::endl
                << "if (" << std::endl;
      cond_exp->Dump();
      std::cout << std::endl
                << ") {" << std::endl;
      stmt_if->Dump();
      std::cout << std::endl
                << "}" << std::endl;
    }
    else if (tp == StmtType::IfElse)
    {
      std::cout << EnumToString(tp);
      std::cout << EnumToString(tp);
      std::cout << std::endl
                << "if (" << std::endl;
      cond_exp->Dump();
      std::cout << std::endl
                << ") {" << std::endl;
      stmt_if->Dump();
      std::cout << std::endl
                << "}" << std::endl;
      std::cout << std::endl
                << "else {" << std::endl;
      stmt_else->Dump();
      std::cout << std::endl
                << "}" << std::endl;
    }
    std::cout << "\n}(StmtAST ends) ";
  }
  std::string type(void) const override
  {
    std::unique_ptr<std::string> TypeStr(new std::string("StmtAST"));
    return *TypeStr;
  }
};

class ExpAST : public BaseAST
{
public:
  BaseAST *lor_exp;
  void Dump() const override
  {
    std::cout << "ExpAST {\n";
    lor_exp->Dump();
    std::cout << "\n}(ExpAST ends) ";
  }
  std::string type(void) const override
  {
    std::unique_ptr<std::string> TypeStr(new std::string("ExpAST"));
    return *TypeStr;
  }
  bool GetConstVal(int &val) const override;
  bool GetConstVal(float &val) const override;
};

class LOrExpAST : public BaseAST
{
public:
  std::vector<BaseAST *> land_exp;
  std::vector<std::string> op;
  void Dump() const override
  {
    std::cout << "LOrExpAST {\n";
    for (int i = 0; i < land_exp.size(); i++)
    {
      if (i != 0)
      {
        std::cout << " " << op[i - 1] << " ";
      }
      land_exp[i]->Dump();
    }

    std::cout << "\n}(LOrExpAST ends) ";
  }
  std::string type(void) const override
  {
    std::unique_ptr<std::string> TypeStr(new std::string("LOrExpAST"));
    return *TypeStr;
  }
  bool GetConstVal(int &val) const override;
  bool GetConstVal(float &val) const override;
};

class LAndExpAST : public BaseAST
{
public:
  std::vector<BaseAST *> eq_exp;
  std::vector<std::string> op;
  void Dump() const override
  {
    std::cout << "LAndExpAST {\n";
    for (int i = 0; i < eq_exp.size(); i++)
    {
      if (i != 0)
      {
        std::cout << " " << op[i - 1] << " ";
      }
      eq_exp[i]->Dump();
    }

    std::cout << "\n}(LAndExpAST ends) ";
  }
  std::string type(void) const override
  {
    std::unique_ptr<std::string> TypeStr(new std::string("LAndExpAST"));
    return *TypeStr;
  }
  bool GetConstVal(int &val) const override;
  bool GetConstVal(float &val) const override;
};

class EqExpAST : public BaseAST
{
public:
  std::vector<BaseAST *> rel_exp;
  std::vector<std::string> op;
  void Dump() const override
  {
    std::cout << "EqExpAST {\n";
    for (int i = 0; i < rel_exp.size(); i++)
    {
      if (i != 0)
      {
        std::cout << " " << op[i - 1] << " ";
      }
      rel_exp[i]->Dump();
    }

    std::cout << "\n}(EqExpAST ends) ";
  }
  std::string type(void) const override
  {
    std::unique_ptr<std::string> TypeStr(new std::string("EqExpAST"));
    return *TypeStr;
  }
  bool GetConstVal(int &val) const override;
  bool GetConstVal(float &val) const override;
};

class RelExpAST : public BaseAST
{
public:
  std::vector<BaseAST *> add_exp;
  std::vector<std::string> op;
  void Dump() const override
  {
    std::cout << "RelExpAST {\n";
    for (int i = 0; i < add_exp.size(); i++)
    {
      if (i != 0)
      {
        std::cout << " " << op[i - 1] << " ";
      }
      add_exp[i]->Dump();
    }

    std::cout << "\n}(RelExpAST ends) ";
  }
  std::string type(void) const override
  {
    std::unique_ptr<std::string> TypeStr(new std::string("RelExpAST"));
    return *TypeStr;
  }
  bool GetConstVal(int &val) const override;
  bool GetConstVal(float &val) const override;
};
class AddExpAST : public BaseAST
{
public:
  std::vector<BaseAST *> mul_exp;
  std::vector<std::string> op;
  void Dump() const override
  {
    std::cout << "AddExpAST {\n";
    for (int i = 0; i < mul_exp.size(); i++)
    {
      if (i != 0)
      {
        std::cout << " " << op[i - 1] << " ";
      }
      mul_exp[i]->Dump();
    }

    std::cout << "\n}(AddExpAST ends) ";
  }
  std::string type(void) const override
  {
    std::unique_ptr<std::string> TypeStr(new std::string("AddExpAST"));
    return *TypeStr;
  }

  bool GetConstVal(int &val) const override;
  bool GetConstVal(float &val) const override;
};

class MulExpAST : public BaseAST
{
public:
  std::vector<BaseAST *> unary_exp;
  std::vector<std::string> op;
  void Dump() const override
  {
    std::cout << "MulExpAST {\n";
    for (int i = 0; i < unary_exp.size(); i++)
    {
      if (i != 0)
      {
        std::cout << " " << op[i - 1] << " ";
      }
      unary_exp[i]->Dump();
    }

    std::cout << "\n}(MulExpAST ends) ";
  }
  std::string type(void) const override
  {
    std::unique_ptr<std::string> TypeStr(new std::string("MulExpAST"));
    return *TypeStr;
  }
  bool GetConstVal(int &val) const override;
  bool GetConstVal(float &val) const override;
};

class UnaryExpAST : public BaseAST
{
public:
  ExpType tp;
  BaseAST *primary_exp;
  BaseAST *unary_op;
  BaseAST *unary_exp;
  std::string *ident;
  BaseAST *func_rparam;
  void Dump() const override
  {
    std::cout << "UnaryExpAST {\n";
    if (tp == ExpType::Primary)
      primary_exp->Dump();
    else if (tp == ExpType::OpExp)
    {
      unary_op->Dump();
      std::cout << ", ";
      unary_exp->Dump();
    }
    else if (tp == ExpType::Call)
    {
      std::cout << *ident << ", ";
      if (func_rparam != nullptr)
        func_rparam->Dump();
    }
    std::cout << "\n}(UnaryExpAST ends) ";
  }
  std::string type(void) const override
  {
    std::unique_ptr<std::string> TypeStr(new std::string("UnaryExpAST"));
    return *TypeStr;
  }
  bool GetConstVal(int &val) const override;
  bool GetConstVal(float &val) const override;
};

class PrimaryExpAST : public BaseAST
{
public:
  PrimaryType tp;    // PrimaryType: Exp, num_INT, num_FLOAT, Lval
  BaseAST *exp;      // 表达式节点
  BaseAST *lval;     // 左值节点
  NumberAST *number; // 存储数字（指向 NumberAST 对象）

  void Dump() const override
  {
    std::cout << "PrimaryExpAST {\n";
    if (tp == PrimaryType::Exp)
    {
      exp->Dump();
    }
    else if (tp == PrimaryType::NumInt)
    {
      std::cout << "Integer: " << number->intNum; // 直接访问 intNum
    }
    else if (tp == PrimaryType::NumFloat)
    {
      std::cout << "Float: " << number->floatNum; // 直接访问 floatNum
    }
    else
    { // Lval 类型
      lval->Dump();
    }
    std::cout << "\n}(PrimaryExpAST ends) ";
  }

  std::string type(void) const override
  {
    std::unique_ptr<std::string> TypeStr(new std::string("PrimaryExpAST"));
    return *TypeStr;
  }

  bool GetConstVal(int &val) const override;
  bool GetConstVal(float &val) const override;
};

class UnaryOpAST : public BaseAST
{
public:
  std::string op;
  void Dump() const override
  {
    std::cout << "UnaryOpAST {\n";
    std::cout << op;
    std::cout << "\n}(UnaryOpAST ends) ";
  }
  std::string type(void) const override
  {
    std::unique_ptr<std::string> TypeStr(new std::string("UnaryOpAST"));
    return *TypeStr;
  }
};

#endif