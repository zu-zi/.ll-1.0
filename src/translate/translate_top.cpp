#include "translate/ir_generator.h"

void Program::block_item_dealer(BlockItemAST *block_item, BaseIRT *&ir)
{
    // std::cout << "in block" << std::endl;/
    if (block_item->decl_or_stmt != nullptr && block_item->decl_or_stmt->type() == "StmtAST")
    {
        StmtAST *stmt_available = dynamic_cast<StmtAST *>(block_item->decl_or_stmt);
        // Deal with Stmt
        stmt_dealer(stmt_available, ir);
        // Further: need to consider: what if no return here in a void function while there are exp or other stmt??
    }
    // deal with decl
    else if (block_item->decl_or_stmt != nullptr && block_item->decl_or_stmt->type() == "DeclAST")
    {
        DeclTranslater(reinterpret_cast<DeclAST *>(block_item->decl_or_stmt), ir);
    }
}

void Program::block_dealer(BlockAST *block, BaseIRT *&ir)
{
    // Deal with Block
    BlockAST *block_available = nullptr;
    BaseAST *block_true = nullptr;
    if (block != nullptr)
    {
        block_available = dynamic_cast<BlockAST *>(block);
        if (block_available != nullptr)
        {
            block_true = block_available->block;
        }
    }
    if(block_true == nullptr)
        return;
    // enter scope
    SymbolTable::EnterScope();
    // deal with blocks
    BaseIRT *ir1 = nullptr, *ir2 = nullptr;
    blockAST *block_true_available = nullptr;
    std::list<std::string> ret_stmt;
    if (block_true != nullptr)
    {
        block_true_available = dynamic_cast<blockAST *>(block_true);
        // in function, there would be :blockitem -- blockitem -- ..
        if (block_true_available != nullptr)
        {
            assert(block_true_available->block_item[0] != nullptr && block_true_available->block_item[0]->type() == "BlockItemAST");
            block_item_dealer(dynamic_cast<BlockItemAST *>(block_true_available->block_item[0]), ir1);
            for (int i = 1; i < block_true_available->block_item.size(); i++)
            {
                block_item_dealer(dynamic_cast<BlockItemAST *>(block_true_available->block_item[i]), ir2);
                ir1 = new StatementIRT(StmKind::Sequence, new SequenceIRT(reinterpret_cast<StatementIRT *>(ir1), reinterpret_cast<StatementIRT *>(ir2)));
            }
            ir = ir1;
        }
    }
    // leave scope
    SymbolTable::ExitScope();
}

void Program::func_dealer(FuncDefAST *func_def, BaseIRT *&ir) 
{
    std::string result(""), ident("");
    VarType func_type;
    FuncFParamsAST *func_fparams;
    BaseAST *block;

    // Start with FuncDefAST, we translate it to IR
    func_type = func_def->func_type;
    ident += *func_def->ident;
    func_fparams = dynamic_cast<FuncFParamsAST *>(func_def->func_fparams);
    block = func_def->block;

    // Check symbol table
    SymbolTable::EnterScope();
    std::vector<std::string> names;
    std::vector<ArgsType> types;
    if (func_fparams)
    {
        for (auto &it : reinterpret_cast<FuncFParamsAST *>(func_fparams)->func_fparam)
        {
            auto param = reinterpret_cast<FuncFParamAST *>(it);
            types.push_back(param->tp);
            names.push_back(*param->ident);
            Symbol *sym = SymbolTable::FindLocalScopeSymbol(*param->ident);
            if (sym != NULL)
            {
                exit(-1);
            }
            if (param->tp == ArgsType::Int32)
            {
                sym = new Symbol(false, false, 0);
            }
            else if (param->tp == ArgsType::Int32Array)
            {
                sym = new Symbol(false, param->dimension);
            }
            else if (param->tp == ArgsType::Float32)
            {
                sym = new Symbol(false, true, 0.0f); // 对于浮点数参数的符号
            }
            else if (param->tp == ArgsType::Float32Array)
            {
                sym = new Symbol(false, param->dimension); // 对于浮点数组参数的符号
            }
            SymbolTable::AddSymbol(*param->ident, sym);
        }
    }

    ValueType ret;
    if (func_type == VarType::INT)
        ret = ValueType::INT32;
    else if (func_type == VarType::FLOAT)
        ret = ValueType::FLOAT32;
    else if (func_type == VarType::VOID)
        ret = ValueType::VOID;
    else
    {
        std::cerr << "Unsupported function return type" << std::endl;
        exit(-1);
    }

    // Deal with block
    SymbolTable::AddGlobalSym(ident, new Symbol(ret, types));
    block_dealer(dynamic_cast<BlockAST *>(block), ir);
    SymbolTable::ExitScope();

    // Construct func_ir tree
    if (func_type == VarType::INT)
    {
        ir = new StatementIRT(StmKind::Func, new FuncIRT(ValueType::INT32, new LabelIRT(ident), names, types, reinterpret_cast<StatementIRT *>(ir)));
    }
    else if (func_type == VarType::FLOAT)
    {
        ir = new StatementIRT(StmKind::Func, new FuncIRT(ValueType::FLOAT32, new LabelIRT(ident), names, types, reinterpret_cast<StatementIRT *>(ir)));
    }
    else if (func_type == VarType::VOID)
    {
        ir = new StatementIRT(StmKind::Func, new FuncIRT(ValueType::VOID, new LabelIRT(ident), names, types, reinterpret_cast<StatementIRT *>(ir)));
    }
}


void Program::Scan(BaseAST *root, BaseIRT *&ir)
{
    std::string comp_const("CompUnitAST");
    CompUnitAST *comp_unit = nullptr;
    BaseAST *root_raw_ptr = root;
    BaseAST *comp_unit_true = nullptr;
    if (root->type() == comp_const)
    {
        comp_unit = dynamic_cast<CompUnitAST *>(root_raw_ptr);
        if (comp_unit != nullptr)
        {
            comp_unit_true = comp_unit->comp_unit;
        }
    } 

    BaseIRT *ir1 = nullptr, *ir2 = nullptr;
    CompunitAST *CompUnitt = dynamic_cast<CompunitAST *>(comp_unit_true);
    if (CompUnitt != nullptr)
    {
        if (CompUnitt->DeclList[0] != nullptr && CompUnitt->DeclList[0]->type() == "FuncDefAST")
        {
            func_dealer(dynamic_cast<FuncDefAST *>(CompUnitt->DeclList[0]), ir1);
        }
        else if (CompUnitt->DeclList[0] != nullptr && CompUnitt->DeclList[0]->type() == "DeclAST")
        {
            this->DeclTranslater(reinterpret_cast<DeclAST *>(CompUnitt->DeclList[0]), ir1);
        }
        for (int i = 1; i < CompUnitt->DeclList.size(); i++)
        {
            // By order, we deal with different functions and declarations
            if (CompUnitt->DeclList[i] != nullptr && CompUnitt->DeclList[i]->type() == "FuncDefAST")
            {
                func_dealer(dynamic_cast<FuncDefAST *>(CompUnitt->DeclList[i]), ir2);
                ir1 = new StatementIRT(StmKind::Sequence, new SequenceIRT(reinterpret_cast<StatementIRT *>(ir1), reinterpret_cast<StatementIRT *>(ir2)));
            }
            else if (CompUnitt->DeclList[i] != nullptr && CompUnitt->DeclList[i]->type() == "DeclAST")
            {
                this->DeclTranslater(reinterpret_cast<DeclAST *>(CompUnitt->DeclList[i]), ir2);
                ir1 = new StatementIRT(StmKind::Sequence, new SequenceIRT(reinterpret_cast<StatementIRT *>(ir1), reinterpret_cast<StatementIRT *>(ir2)));
            }
        }
        ir = ir1;
    }
}
