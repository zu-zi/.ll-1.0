#include "translate/ir_generator.h"
#include "common/initvalT.h"
void Program::DeclTranslater(DeclAST *decl, BaseIRT *&ir)
{
    if (decl->tp == AstType::ConstDecl)
    {
        ConstDeclTranslater(reinterpret_cast<ConstDeclAST *>(decl->const_or_var_decl), ir);
    }
    else
    {
        VarDeclTranslater(reinterpret_cast<VarDeclAST *>(decl->const_or_var_decl), ir);
    }
    return;
}
void Program::VarDeclTranslater(VarDeclAST *decl, BaseIRT *&ir)
{
    decl->HandleSymbol();
    if (decl->Type == VarType::INT)
    {
        for (auto &it : decl->VarDefVec)
        {
            this->VarDefTranslater(SymType::INT32, reinterpret_cast<VarDefAST *>(it), ir);
        }
    }
    else if (decl->Type == VarType::FLOAT)
    {
        for (auto &it : decl->VarDefVec)
        {
            this->VarDefTranslater(SymType::FLOAT32, reinterpret_cast<VarDefAST *>(it), ir);
        }
    }
    else
    {
        exit(-1);
    }
    return;
}


void Program::InternalConvertExpInitTreeToIR(InitValTree<BaseAST *> *AstTree, const std::vector<int> &dim, std::vector<unsigned int> &trait, ExpIRT *addr, StatementIRT *&ir)
{
    if (AstTree->childs.size())
    {
        for (unsigned int i = 0; i < AstTree->childs.size(); ++i)
        {
            trait.push_back(i);
            InternalConvertExpInitTreeToIR(AstTree->childs[i], dim, trait, addr, ir);
            trait.pop_back();
        }
    }
    else
    {
        int Base = 1, Offset = 0;
        if (trait.size())
        {
            Offset += trait[trait.size() - 1];
            for (int i = trait.size() - 2; i > -1; --i)
            {
                Base *= dim[i + 1];
                Offset += Base * trait[i];
            }
        }
        for (int i = 0; i < AstTree->keys.size(); ++i)
        {
            BaseIRT *initexp;
            this->logic_exp_dealer(AstTree->keys[i], initexp);
            AddStmToTree(ir,
                         new StatementIRT(
                             new MoveIRT(
                                 new MemIRT(
                                     new ExpIRT(
                                         new BinOpIRT(
                                             BinOpKind::plus, addr, new ExpIRT(new ConstIRT((Offset++) << 2))))),
                                 reinterpret_cast<ExpIRT *>(initexp))));
        }
    }
}
void Program::ConvertExpInitTreeToIR(InitValTree<BaseAST *> *AstTree, const std::vector<int> &dim, ExpIRT *addr, StatementIRT *&ir)
{
    ir = NULL;
    if(AstTree==NULL) return;
    if(dim.size()==0)
    {
        if (AstTree&&AstTree->keys.size())
        {
            BaseIRT* initexp;
            this->logic_exp_dealer(AstTree->keys[0],initexp);
            AddStmToTree(ir,
                         new StatementIRT(
                             new MoveIRT(
                                 new MemIRT(addr),reinterpret_cast<ExpIRT *>(initexp))));
        }
    }

    return;
    // get init value tree depth
    unsigned int depth = 0;
    BaseIRT* initexp;
    if (AstTree)
    {
        if (AstTree->childs.size() == 0)
        {
            this->logic_exp_dealer(AstTree->keys[0],initexp);
            AddStmToTree(ir,
                         new StatementIRT(
                             new MoveIRT(
                                 new MemIRT(addr),reinterpret_cast<ExpIRT *>(initexp))));
            return;
        }
        auto p = AstTree;
        while (1)
        {
            if (p->childs.size())
            {
                depth++;
                p = p->childs[0];
            }
            else
            {
                break;
            }
        }

    }
    // if no array
    if (depth == 0)
    {
        // this->logic_exp_dealer(AstTree->FindFirst(), ir);
        ir = NULL;
        return;
    }

    // handle init value
    std::vector<unsigned int> trait;

    // handle if init value depth less than array dimension
    for (unsigned int i = depth; i < dim.size(); ++i)
    {
        trait.push_back(0);
    }

    // handle matched dimension
    this->InternalConvertExpInitTreeToIR(AstTree->childs[0], dim, trait, addr, ir);
}
void Program::VarDefTranslater(SymType type, VarDefAST *decl, BaseIRT *&ir)
{
    Symbol *sym = decl->VarSym;
    
    if (type == SymType::INT32 || type == SymType::FLOAT32)  // 统一处理INT32和FLOAT32类型
    {
        // 判断是否是全局变量
        if (decl->VarSym->GetGlobalFlag())
        {
            // 全局变量处理
            if (decl->DimSizeVec.size())  // 如果是数组
            {
                // 数组处理，支持INT32和FLOAT32
                if (type == SymType::INT32)
                {
                    ir = new GlobalVarIRT(ValueType::INT32, *decl->VarIdent, false, sym->ArrAttributes->ArrayDimVecInt, sym->ArrAttributes->ConstInitValInt);
                }
                else if (type == SymType::FLOAT32)
                {
                    ir = new GlobalVarIRT(ValueType::FLOAT32, *decl->VarIdent, false, sym->ArrAttributes->ArrayDimVecFloat, sym->ArrAttributes->ConstInitValFloat);
                }
            }
            // 非数组变量
            else
            {
                if (type == SymType::INT32)
                {
                    ir = new GlobalVarIRT(ValueType::INT32, *decl->VarIdent, false, sym->VarArrtributes.InitVal);
                }
                else if (type == SymType::FLOAT32)
                {
                    ir = new GlobalVarIRT(ValueType::FLOAT32, *decl->VarIdent, false, sym->VarArrtributes.InitVal);
                }
            }
        }
        // 局部变量
        else
        {
            std::vector<int> dim;
            if (sym->ArrAttributes)
            {
                dim = sym->ArrAttributes->ArrayDimVecInt;
            }
            
            // 分配栈空间
            auto alloc = new ExpIRT(new AllocateIRT(*decl->VarIdent, dim, 4));  // 假设INT32和FLOAT32占用相同的字节数
            auto addr = new TempIRT();
            auto res = new StatementIRT(new MoveIRT(addr, alloc));
            StatementIRT *init = NULL;

            // 将初始化值转换为IR
            this->ConvertExpInitTreeToIR(decl->InitValue, dim, new ExpIRT(addr), init);
            if (init)
            {
                AddStmToTree(res, init);
            }
            
            ir = res;
        }
    }

    return;
}


void Program::ConstDefTranslater(SymType type, VarDefAST *decl, BaseIRT *&ir)
{
    Symbol *sym = decl->VarSym;
    
    if (type == SymType::INT32 || type == SymType::FLOAT32)  // 统一处理INT32和FLOAT32类型
    {
        // 判断是否是全局常量
        if (decl->VarSym->GetGlobalFlag())
        {
            // 如果是数组
            if (decl->DimSizeVec.size())
            {
                if (type == SymType::INT32)
                {
                    ir = new GlobalVarIRT(ValueType::INT32, *decl->VarIdent, true, sym->ArrAttributes->ArrayDimVecInt, sym->ArrAttributes->ConstInitValInt);
                }
                else if (type == SymType::FLOAT32)
                {
                    ir = new GlobalVarIRT(ValueType::FLOAT32, *decl->VarIdent, true, sym->ArrAttributes->ArrayDimVecFloat, sym->ArrAttributes->ConstInitValFloat);
                }
            }
            // 非数组常量
            else
            {
                if (type == SymType::INT32)
                {
                    ir = new GlobalVarIRT(ValueType::INT32, *decl->VarIdent, true, sym->VarArrtributes.InitVal);
                }
                else if (type == SymType::FLOAT32)
                {
                    ir = new GlobalVarIRT(ValueType::FLOAT32, *decl->VarIdent, true, sym->VarArrtributes.InitVal);
                }
            }
        }
        // 局部常量
        else
        {
            // 局部常量处理逻辑
            ir = NULL;  // 暂时没有具体的局部常量处理
        }
    }

    return;
}

void Program::ConstDeclTranslater(ConstDeclAST *decl, BaseIRT *&ir)
{
    decl->HandleSymbol();
    if (decl->Type == VarType::INT)
    {
        for (auto &it : decl->ConstDefVec)
        {
            this->ConstDefTranslater(SymType::INT32, reinterpret_cast<VarDefAST *>(it), ir);
        }
    }
    else if (decl->Type == VarType::FLOAT)
    {
        for (auto &it : decl->ConstDefVec)
        {
            this->ConstDefTranslater(SymType::FLOAT32, reinterpret_cast<VarDefAST *>(it), ir);
        }
    }
    else
    {
        exit(-1);
    }
    return;
}
