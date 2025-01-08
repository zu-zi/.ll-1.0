#ifndef _SYMBOL_TABLE_
#define _SYMBOL_TABLE_
#include<map>
#include<vector>
#include"common/enum.h"
#include"common/initvalT.h"
#include"common/base_ast.hpp"
class VarInfo{
public:
    bool IsInited;
    int InitVal;
    float FloatInitVal;
    BaseAST* ExpVal;
    VarInfo(int val) : IsInited(true), InitVal(val), FloatInitVal(0.0f), ExpVal(NULL) {}
    VarInfo(float val) : IsInited(true), InitVal(0), FloatInitVal(val), ExpVal(NULL) {}
    VarInfo():IsInited(false),InitVal(0),ExpVal(NULL){}
};
class FunctionInfo{
public:
    ValueType RetValType;
    std::vector<ArgsType> ArgsTypeVec;
    FunctionInfo(){}
    FunctionInfo(ValueType ret,std::vector<ArgsType> args):RetValType(ret),ArgsTypeVec(args){}
};
class ArrayInfo {
public:
    bool IsInited;
    bool IsConstInit;

    // 用于保存数组维度（支持 int 和 float）
    std::vector<int> ArrayDimVecInt;    // 用于保存 int 类型维度
    std::vector<float> ArrayDimVecFloat; // 用于保存 float 类型维度

    // 用于初始化值（分别为 int 类型和 float 类型的初始化树）
    InitValTree<BaseAST*> *VarInitVal;
    InitValTree<int> *ConstInitValInt; 
    InitValTree<float> *ConstInitValFloat;

    // 默认构造函数
    ArrayInfo() 
        : IsInited(false), IsConstInit(false), VarInitVal(NULL), ConstInitValInt(NULL), ConstInitValFloat(NULL) {}

    // 支持 int 类型的维度初始化
    ArrayInfo(std::vector<int> vec)
        : IsInited(false), IsConstInit(false), ArrayDimVecInt(vec), VarInitVal(NULL), ConstInitValInt(NULL), ConstInitValFloat(NULL) {}

    // 支持 int 类型数组和变量初始化表达式
    ArrayInfo(std::vector<int> vec, InitValTree<BaseAST*> *VarInit)
        : IsInited(true), IsConstInit(false), ArrayDimVecInt(vec), VarInitVal(VarInit), ConstInitValInt(NULL), ConstInitValFloat(NULL) {}

    // 支持 int 类型数组和常量初始化值
    ArrayInfo(std::vector<int> vec, InitValTree<int> *ConstInit)
        : IsInited(true), IsConstInit(true), ArrayDimVecInt(vec), VarInitVal(NULL), ConstInitValInt(ConstInit), ConstInitValFloat(NULL) {}

    // 支持 float 类型的维度初始化
    ArrayInfo(std::vector<float> vec)
        : IsInited(false), IsConstInit(false), ArrayDimVecFloat(vec), VarInitVal(NULL), ConstInitValInt(NULL), ConstInitValFloat(NULL) {}

    // 支持 float 类型数组和变量初始化表达式
    ArrayInfo(std::vector<float> vec, InitValTree<BaseAST*> *VarInit)
        : IsInited(true), IsConstInit(false), ArrayDimVecFloat(vec), VarInitVal(VarInit), ConstInitValInt(NULL), ConstInitValFloat(NULL) {}

    // 支持 float 类型数组和常量初始化值
    ArrayInfo(std::vector<float> vec, InitValTree<float> *ConstInit)
        : IsInited(true), IsConstInit(true), ArrayDimVecFloat(vec), VarInitVal(NULL), ConstInitValInt(NULL), ConstInitValFloat(ConstInit) {}

    // 析构函数
    ~ArrayInfo() {
        delete VarInitVal;
        delete ConstInitValInt;
        delete ConstInitValFloat;
    }
};

class Symbol {
    friend class SymbolTable;
private:
    bool GlobalFlag;
public:
    int TempId;
    SymType SymbolType;
    bool ConstFlag;
    bool MemoryFlag;
    VarInfo VarArrtributes; // For scalar variables
    ArrayInfo* ArrAttributes;
    FunctionInfo* FunctionAttributes; // For functions

    /**
     * @brief Constructor for scalar INT32 variable
    */
    Symbol(bool IsConst, bool IsInMemory, int val)
        : SymbolType(SymType::INT32), ConstFlag(IsConst), MemoryFlag(IsInMemory), VarArrtributes(val) {
        FunctionAttributes = NULL;
    }

    /**
     * @brief Constructor for scalar FLOAT32 variable
    */
    Symbol(bool IsConst, bool IsInMemory, float val)
        : SymbolType(SymType::FLOAT32), ConstFlag(IsConst), MemoryFlag(IsInMemory),VarArrtributes(val) {
        FunctionAttributes = NULL;
    }

Symbol(bool IsConst, std::vector<int> dim, InitValTree<int>* initval = NULL, InitValTree<BaseAST*>* initexp = NULL)
    : SymbolType(SymType::Int32Array), ConstFlag(IsConst), MemoryFlag(true) 
{
    if (initexp != NULL) {
        // 如果传入了初始化表达式
        ArrAttributes = new ArrayInfo(dim, initexp); // 创建一个 int 数组类型的 ArrayInfo
    } else {
        // 如果没有初始化表达式，使用 int 常量初始化值
        ArrAttributes = new ArrayInfo(dim, initval); // 创建一个 int 常量初始化的 ArrayInfo
    }
    FunctionAttributes = NULL;
}

// 添加 float 类型支持
Symbol(bool IsConst, std::vector<float> dim, InitValTree<float>* initval = NULL, InitValTree<BaseAST*>* initexp = NULL)
    : SymbolType(SymType::Float32Array), ConstFlag(IsConst), MemoryFlag(true) 
{
    if (initexp != NULL) {
        // 如果传入了初始化表达式
        ArrAttributes = new ArrayInfo(dim, initexp); // 创建一个 float 数组类型的 ArrayInfo
    } else {
        // 如果没有初始化表达式，使用 float 常量初始化值
        ArrAttributes = new ArrayInfo(dim, initval); // 创建一个 float 常量初始化的 ArrayInfo
    }
    FunctionAttributes = NULL;
}

    /**
     * @brief Constructor for functions
    */
    Symbol(ValueType ret, std::vector<ArgsType> args)
        : SymbolType(SymType::FuncName) {
        ArrAttributes = NULL;
        FunctionAttributes = new FunctionInfo(ret, args);
    }

    /**
     * @brief Get a bool indicating whether this symbol is global
    */
    bool GetGlobalFlag() const;

    /**
     * @brief Process the symbol name to label (add '%' or '@' depending on global flag)
    */
    std::string GetLabelStr(std::string SymName) const;

    /**
     * @brief Destructor
    */
    ~Symbol() {
        delete FunctionAttributes;
    }
};


class SymbolTable{
public:
    /**
     * @brief default ctor
    */
    // SymbolTable(){
    //     // init the basic table which contains global symbol
    //     TableVec.push_back(std::map<std::string,Symbol*>());
    // }
    /**
     * @brief add a new symbol
     * @author zhang xin
     * @param sym a symbol ptr
     * @return return bool, true indicate duplicate symbol, false otherwise
     * @example: AddSymbol(new Symbol(SymType::INT32,"result"))
    */
    static bool AddSymbol(std::string name,Symbol* sym);
    /**
     * @brief get a symbol by name
     * @param name: symbol name string
     * @return return a symbol ptr, if not find , return NULL
     * @example Symbol* sym = FindSymbol("name");
    */
    static Symbol* FindSymbol(std::string name);
    /**
     * @brief this function find symbol from local scope
     * 
     * @param name 
     * @return Symbol* 
     */
    static Symbol* FindLocalScopeSymbol(std::string name);
    /**
     * @brief call when you enter a new scope
    */
    static void EnterScope();
    /**
     * @brief call when you exit a  scope
    */
    static void ExitScope();
    /**
     * @brief Call only once in main, then you can use symbol table
    */
    static void InitTable();
    /**
     * @brief add const string for putf
     * @param str: const string
     * @return : lable points to the string
    */
    static std::string AddConstString(std::string &str);
    /**
     * @brief print const string declare in llvm style
    */
    static void PrintConstStringDeclare();
    /**
     * @brief this function add global symbol to the map, for function use, and this function is not responsible for check duplicate symbol or symbol is inited or not
     * 
     * @param name :function name
     * @param sym :function symbol
     */
    static void AddGlobalSym(std::string name,Symbol* sym);

private:
    static std::vector<std::map<std::string,Symbol*>> TableVec;
    static std::map<std::string, std::string> ConstStringMap;
    static int UnNamedStringCounter;
};
#endif