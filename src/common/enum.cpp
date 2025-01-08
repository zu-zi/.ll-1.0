#include "common/enum.h"

// 更新字符串数组以支持新的类型
const char *AstKindStr[] = {"ConstDecl", "VarDecl"};
const char *VarTypeStr[] = {"INT", "FLOAT"};  // 添加 FLOAT 类型
const char *StmtTypeStr[] = {"Assign", "Exp", "Block", "If", "IfElse", "While", "Break", "Continue", "ReturnExp", "ReturnVoid"};
const char *ArgTypeStr[] = {"i32", "i32*", "f32", "f32*", "i8*", "undefined"};  // 添加 f32 和 f32* 类型
const char *IrValTypeStr[] = {"i1", "i8", "i16", "i32", "i64", "void"};
const char* ValueTypeStr[] = {"i32", "f32", "void"};  // 添加 f32 类型

// 保证 EnumToString 函数支持所有类型的枚举值
std::string EnumToString(ValueType type)
{
    return ValueTypeStr[int(type)];
}
std::string EnumToString(AstType type)
{
    return AstKindStr[int(type)];
}
std::string EnumToString(VarType type)
{
    return VarTypeStr[int(type)];
}
std::string EnumToString(StmtType type)
{
    return StmtTypeStr[int(type)];
}
std::string EnumToString(ArgsType type)
{
    return ArgTypeStr[int(type)];
}
std::string EnumToString(IrValType type)
{
    return IrValTypeStr[int(type)];
}
