#include "ir/ir_tree.h"
static StmKind LastKind = StmKind::Cjump;
std::map<std::string,int> IrTreeStackVarTable::StackTable;
void StatementIRT::Dump() const{
    if(LastKind==StmKind::Jump && this->ContentKind == StmKind::Jump)
    {
        return;
    }
    LastKind = this->ContentKind;
    StmContent->Dump();
}
void IrTreeStackVarTable::Init()
{
    StackTable.clear();
}
int IrTreeStackVarTable::AddVarieble(std::string ident)
{
    auto it = StackTable.find(ident);
    if(it!=StackTable.end())
    {
        return it->second;
    }
    int TempId = TempIdAllocater::GetId();
    StackTable.insert(std::pair<std::string,int>(ident,TempId));
    return TempId;
}
int IrTreeStackVarTable::FindVarieble(std::string ident)
{

    auto it = StackTable.find(ident);
    if(it==StackTable.end())
    {
//error
    }
    return it->second;
}
void CjumpIRT::Dump() const
{
    BinOpIRT CondExp(RelationOp, LeftExp, RightExp);
    auto ConValue = CondExp.ExpDump();
    std::cout << "\n";
    std::string condition_type = ConValue.TypeToString();
    std::string condition_label = ConValue.LabelToString();
    if (ConValue.TypeToString() == std::string("i32"))
    {
        // add a statement to convert.
        condition_type = std::string("i1");
        condition_label = "%" + std::to_string(TempIdAllocater::GetId());
        std::cout << condition_label << " = "
                  << "icmp ne i32 " << ConValue.LabelToString() << ", 0" << std::endl;
    }
    std::cout << "br " << condition_type << " " << condition_label << ", label %" << LabelTrue->LableName << ", label %" << LableFalse->LableName << "\n";
}
void MoveIRT::Dump() const
{
    auto SrcExpVal = SrcExp->ExpDump();
    if (MoveKind == ToTemp)
    {
        // std::cout << "%" << DstTemp->TempVarId << " = " << SrcExpVal.LabelToString();
        // std::cout << "\n";
        this->DstTemp->TempValue = SrcExpVal;
    }
    else
    {
        auto DstAddrVal = DstMem->ExpDump();
        // BitCast(DstAddrVal,SrcExpVal.ExpType,true);
        ConvertMemToTemp(SrcExpVal);
        std::cout << "store " << SrcExpVal.TypeToString() << " " << SrcExpVal.LabelToString() << ", " << DstAddrVal.TypeToString() << " " << DstAddrVal.LabelToString() << "\n";
    }
}
ExpValue ExpIRT::ExpDump() const
{
    BinOpIRT *BinOpContent = reinterpret_cast<BinOpIRT *>(ExpContent);
    MemIRT *MemContent = reinterpret_cast<MemIRT *>(ExpContent);
    TempIRT *TempContent = reinterpret_cast<TempIRT *>(ExpContent);
    ESeqIRT *EseqContent = reinterpret_cast<ESeqIRT *>(ExpContent);
    NameIRT *NameContent = reinterpret_cast<NameIRT *>(ExpContent);
    ConstIRT *ConstContent = reinterpret_cast<ConstIRT *>(ExpContent);
    CallIRT *CallContent = reinterpret_cast<CallIRT *>(ExpContent);
    AllocateIRT *AllocateContent = reinterpret_cast<AllocateIRT *>(ExpContent);
    switch (ContentKind)
    {
    case ExpKind::BinOp:

        return BinOpContent->ExpDump();
    case ExpKind::Mem:

        return MemContent->ExpDump();
    case ExpKind::Temp:

        return TempContent->ExpDump();
    case ExpKind::ESeq:

        return EseqContent->ExpDump();
    case ExpKind::Name:

        return NameContent->ExpDump();
    case ExpKind::Const:

        return ConstContent->ExpDump();
    case ExpKind::Call:

        return CallContent->ExpDump();
    case ExpKind::Allocate:
        return AllocateContent->ExpDump();
    }
}
ExpValue ConvertExpToBinOprand(ExpIRT *exp)
{
    ExpValue res = exp->ExpDump();
    if (exp->ContentKind == ExpKind::Mem)
    {
        if (res.IsPtr)
        {
            BitCast(res, IrValType::i32, true);
            int TempId = TempIdAllocater::GetId();
            std::cout << "%" << std::to_string(TempId) << " = "
                      << "load i32,i32* " << res.LabelToString() << "\n";
            res.IsPtr = false;
            res.TempId = TempId;
            res.VarName = "%%%%";
        }
    }
    else if (exp->ContentKind == ExpKind::Name || res.IsPtr)
    {
        ConvertPtrToInt(res);
    }

    return res;
}
ExpValue BinOpIRT::ExpDump() const
{
    int temp1, temp2, res, temp;
    std::string ResString(""), TempString;
    // ExpValue ResVal;
    ExpValue ResVal, LeftValue, RightValue;
    // auto LeftValue = LeftExp->ExpDump(), RightValue = RightExp->ExpDump();
    // OpBitSignedExtension(LeftValue,RightValue);
    // ConvertMemToTemp(LeftValue);
    // ConvertMemToTemp(RightValue);
    LeftValue = ConvertExpToBinOprand(LeftExp);
    RightValue = ConvertExpToBinOprand(RightExp);
    OpBitSignedExtension(LeftValue, RightValue);
    ResVal.ExpType = LeftValue.ExpType;
    switch (OpKind)
    {
    case BinOpKind::plus:
        res = TempIdAllocater::GetId();
        ResString += "%" + std::to_string(res);

        std::cout << ResString << " = "
                  << "add " << LeftValue.TypeToString() << " " << LeftValue.LabelToString() << ", " << RightValue.LabelToString() << "\n";
        break;
    case BinOpKind::minus:
        res = TempIdAllocater::GetId();
        ResString += "%" + std::to_string(res);

        std::cout << ResString << " = "
                  << "sub " << LeftValue.TypeToString() << " " << LeftValue.LabelToString() << ", " << RightValue.LabelToString() << "\n";
        break;
    case BinOpKind::mul:
        res = TempIdAllocater::GetId();
        ResString += "%" + std::to_string(res);

        std::cout << ResString << " = "
                  << "mul " << LeftValue.TypeToString() << " " << LeftValue.LabelToString() << ", " << RightValue.LabelToString() << "\n";
        break;
    case BinOpKind::_div:
        res = TempIdAllocater::GetId();
        ResString += "%" + std::to_string(res);
        std::cout << ResString << " = "
                  << "sdiv " << LeftValue.TypeToString() << " " << LeftValue.LabelToString() << ", " << RightValue.LabelToString() << "\n";
        break;
    case BinOpKind::IsEqual:
        temp = TempIdAllocater::GetId();
        TempString = "%" + std::to_string(temp);
        res = TempIdAllocater::GetId();
        ResString += "%" + std::to_string(res);
        std::cout << TempString << " = "
                  << "icmp eq " << LeftValue.TypeToString() << " " << LeftValue.LabelToString() << ", " << RightValue.LabelToString() << "\n";
        std::cout << ResString << " = "
                  << "zext i1 " << TempString << " to i32\n";
        ResVal.ExpType = IrValType::i32;
        break;
    case BinOpKind::IsNe:
        temp = TempIdAllocater::GetId();
        TempString = "%" + std::to_string(temp);
        res = TempIdAllocater::GetId();
        ResString += "%" + std::to_string(res);
        std::cout << TempString << " = "
                  << "icmp ne " << LeftValue.TypeToString() << " " << LeftValue.LabelToString() << ", " << RightValue.LabelToString() << "\n";
        std::cout << ResString << " = "
                  << "zext i1 " << TempString << " to i32\n";
        ResVal.ExpType = IrValType::i32;
        break;
    case BinOpKind::IsLt:
        temp = TempIdAllocater::GetId();
        TempString = "%" + std::to_string(temp);
        res = TempIdAllocater::GetId();
        ResString += "%" + std::to_string(res);
        std::cout << TempString << " = "
                  << "icmp slt " << LeftValue.TypeToString() << " " << LeftValue.LabelToString() << ", " << RightValue.LabelToString() << "\n";
        std::cout << ResString << " = "
                  << "zext i1 " << TempString << " to i32\n";
        ResVal.ExpType = IrValType::i32;
        break;
    case BinOpKind::IsLe:
        temp = TempIdAllocater::GetId();
        TempString = "%" + std::to_string(temp);
        res = TempIdAllocater::GetId();
        ResString += "%" + std::to_string(res);
        std::cout << TempString << " = "
                  << "icmp sle " << LeftValue.TypeToString() << " " << LeftValue.LabelToString() << ", " << RightValue.LabelToString() << "\n";
        std::cout << ResString << " = "
                  << "zext i1 " << TempString << " to i32\n";
        ResVal.ExpType = IrValType::i32;
        break;
    case BinOpKind::IsGt:
        temp = TempIdAllocater::GetId();
        TempString = "%" + std::to_string(temp);
        res = TempIdAllocater::GetId();
        ResString += "%" + std::to_string(res);
        std::cout << TempString << " = "
                  << "icmp sgt " << LeftValue.TypeToString() << " " << LeftValue.LabelToString() << ", " << RightValue.LabelToString() << "\n";
        std::cout << ResString << " = "
                  << "zext i1 " << TempString << " to i32\n";
        ResVal.ExpType = IrValType::i32;
        break;
    case BinOpKind::IsGe:
        temp = TempIdAllocater::GetId();
        TempString = "%" + std::to_string(temp);
        res = TempIdAllocater::GetId();
        ResString += "%" + std::to_string(res);
        std::cout << TempString << " = "
                  << "icmp sge " << LeftValue.TypeToString() << " " << LeftValue.LabelToString() << ", " << RightValue.LabelToString() << "\n";
        std::cout << ResString << " = "
                  << "zext i1 " << TempString << " to i32\n";
        ResVal.ExpType = IrValType::i32;
        break;
    case BinOpKind::rem:
        res = TempIdAllocater::GetId();
        ResString += "%" + std::to_string(res);
        std::cout << ResString << " = "
                  << "srem " << LeftValue.TypeToString() << " " << LeftValue.LabelToString() << ", " << RightValue.LabelToString() << "\n";
        break;
    case BinOpKind::LogicAnd:
        temp1 = TempIdAllocater::GetId();
        temp2 = TempIdAllocater::GetId();
        res = TempIdAllocater::GetId();
        std::cout << "%" << temp1 << " = "
                  << "icmp ne i32 0, " << LeftValue.LabelToString() << "\n";
        std::cout << "%" << temp2 << " = "
                  << "icmp ne i32 0, " << RightValue.LabelToString() << "\n";
        std::cout << "%" << std::to_string(res) << " = "
                  << "and i1 %" << std::to_string(temp1) << ", %" << std::to_string(temp2)<<"\n";
        ResString = "%" + std::to_string(res);
        res = TempIdAllocater::GetId();
        std::cout<<"%"<<res<<" = sext i1 "<<ResString<<" to i32\n";
        ResVal.ExpType = IrValType::i32;
        break;
    case BinOpKind::LogicOr:
        temp1 = TempIdAllocater::GetId();
        temp2 = TempIdAllocater::GetId();
        res = TempIdAllocater::GetId();
        std::cout << "%" << temp1 << " = "
                  << "icmp ne i32 0, " << LeftValue.LabelToString() << "\n";
        std::cout << "%" << temp2 << " = "
                  << "icmp ne i32 0, " << RightValue.LabelToString() << "\n";
        std::cout << "%" << std::to_string(res) << " = "
                  << "or i1 %" << std::to_string(temp1) << ", %" << std::to_string(temp2)<<"\n";
        ResString = "%" + std::to_string(res);
        res = TempIdAllocater::GetId();
        std::cout<<"%"<<res<<" = sext i1 "<<ResString<<" to i32\n";
        ResVal.ExpType = IrValType::i32;
        break;
    case BinOpKind::LogicNot:
        res = TempIdAllocater::GetId();
        ResString = "%" + std::to_string(res);
        std::cout << ResString << " = "
                  << "icmp eq i32 0, " << LeftValue.LabelToString();
        ResString = "%" + std::to_string(res);
        res = TempIdAllocater::GetId();
        std::cout<<"%"<<res<<" = sext i1 "<<ResString<<" to i32\n";
        ResVal.ExpType = IrValType::i32;
        break;
    case BinOpKind::Neg:
        res = TempIdAllocater::GetId();
        ResString = "%" + std::to_string(res);
        std::cout << ResString << " = "
                  << "sub " << LeftValue.TypeToString() << " 0, " << LeftValue.LabelToString();
        break;
    default:
        res = 0;
        break;
    }
    ResVal.TempId = res;
    return ResVal;
}

ExpValue MemIRT::ExpDump() const
{
    auto res = AddressExp->ExpDump();
    // BitCast(res,IrValType::i32,true);
    if(this->AddressExp->ContentKind==ExpKind::Name && reinterpret_cast<NameIRT*>(this->AddressExp->ExpContent)->IsPtr == false)
    {
        return res;
    }
    ConvertIntToPtr(res);
    return res;
}
ExpValue TempIRT::ExpDump() const
{
    return this->TempValue;
}
ExpValue ESeqIRT::ExpDump() const
{
    SideEffectStm->Dump();
    return EstimatedExp->ExpDump();
}
ExpValue NameIRT::ExpDump() const
{
    ExpValue res;
    res.VarName = this->AsmLableName;
    if(this->AsmLableName[0] != '@')
    {
        res.TempId = IrTreeStackVarTable::FindVarieble(&this->AsmLableName[1]);
    }
    if (this->ArrayDim.size())
    {
        res.ExpDim = this->ArrayDim;
    }
    res.IsPtr = this->IsPtr;
    return res;
}
ExpValue ConstIRT::ExpDump() const
{
    ExpValue res;
    res.IsConst = true;
    res.ConstValue = this->ConstVal;
    return res;
}
ExpValue CallIRT::ExpDump() const 
{
    ExpValue res;
    std::string ResString("");
    std::string RetTypeString("");
    std::vector<ExpValue> ArgsVal;
    int i = 0;
    for (auto &it : this->ArgsExpList)
    {
        auto TempVal = it->ExpDump();
        if (this->ArgsTypeList[i] == ArgsType::Int32)
        {
            ConvertMemToTemp(TempVal);
        }
        else if (this->ArgsTypeList[i] == ArgsType::Int32Array)
        {
            BitCast(TempVal, IrValType::i32, true);
        }
        else if (this->ArgsTypeList[i] == ArgsType::Float32)
        {
            ConvertMemToTemp(TempVal); // 对浮点数参数进行必要的临时转换
        }
        else if (this->ArgsTypeList[i] == ArgsType::Float32Array)
        {
            BitCast(TempVal, IrValType::f32, true); // 对浮点数组进行类型转换
        }
        else if (this->ArgsTypeList[i] == ArgsType::String)
        {
            TempVal.ExpType = IrValType::i8;
            BitCast(TempVal, IrValType::i8, true);
        }
        ArgsVal.push_back(TempVal);
        i++;
    }

    if (this->IsVarParaCount)
    {
        RetTypeString = EnumToString(this->RetValType) + "(" + EnumToString(this->ArgsTypeList[0]) + ",...) "; 
        for (unsigned int i = 1; i < ArgsVal.size(); ++i)
        {
            ConvertMemToTemp(ArgsVal[i]);
        }
    }
    else
    {
        RetTypeString = EnumToString(this->RetValType);
    }

    if (RetValType == ValueType::INT32)
    {
        int TempId = TempIdAllocater::GetId();
        res.ExpType = IrValType::i32;
        std::cout << "%" << TempId << " = "
                  << "call " << RetTypeString << " @" << FuncLable->LableName + "(";
        res.TempId = TempId;
        for (int i = 0; i < ArgsVal.size(); ++i)
        {
            std::cout << ArgsVal[i].TypeToString() << " " << ArgsVal[i].LabelToString();
            if (i != ArgsVal.size() - 1)
            {
                std::cout << ", ";
            }
        }
        std::cout << ")\n";
        return res;
    }
    else if (RetValType == ValueType::FLOAT32)
    {
        int TempId = TempIdAllocater::GetId();
        res.ExpType = IrValType::f32;
        std::cout << "%" << TempId << " = "
                  << "call " << RetTypeString << " @" << FuncLable->LableName + "(";
        res.TempId = TempId;
        for (int i = 0; i < ArgsVal.size(); ++i)
        {
            std::cout << ArgsVal[i].TypeToString() << " " << ArgsVal[i].LabelToString();
            if (i != ArgsVal.size() - 1)
            {
                std::cout << ", ";
            }
        }
        std::cout << ")\n";
        return res;
    }
    else if (RetValType == ValueType::VOID)
    {
        res.ExpType = IrValType::_void_;
        std::cout << "call " << RetTypeString << " @" << FuncLable->LableName + "(";

        for (int i = 0; i < ArgsVal.size(); ++i)
        {
            std::cout << ArgsVal[i].TypeToString() << " " << ArgsVal[i].LabelToString();

            if (i != ArgsVal.size() - 1)
            {
                std::cout << ", ";
            }
        }
        std::cout << ")\n";
        return res;
    }
    else
    {
        exit(-1);
    }
}

ExpValue AllocateIRT::ExpDump() const
{
    ExpValue res;
    res.TempId = IrTreeStackVarTable::AddVarieble(this->ident);
    std::cout << "%" << res.TempId << " = "
              << "alloca ";
    PrintInitialStruct(this->ArrDim, 0);
    if (AlignSize > 1)
    {
        std::cout << ", align " << std::to_string(AlignSize);
    }
    std::cout << "\n";
    res.IsPtr = true;
    res.ExpDim = this->ArrDim;
    res.VarName = "%" + this->ident;
    // BitCast(res,IrValType::i64,false);
    // ConvertPtrToInt(res);
    return res;
}
void FuncIRT::Dump() const 
{
    IrTreeStackVarTable::Init();

    std::string FuncTypeStr("");
    if (RetValType == ValueType::INT32)
    {
        FuncTypeStr = "i32";
    }
    else if (RetValType == ValueType::FLOAT32)
    {
        FuncTypeStr = "f32";
    }
    else if (RetValType == ValueType::VOID)
    {
        FuncTypeStr = "void";
    }

    std::cout << "define " << FuncTypeStr << " @" << this->FuncLable->LableName << "(";
    TempIdAllocater::Rewind();

    for (int i = 0; i < this->ArgsVec.size(); ++i)
    {
        std::cout << EnumToString(ArgsVec[i]);
        std::cout << " %";
        int TempId =  IrTreeStackVarTable::AddVarieble(this->ParameterNameVec[i]);
        std::cout << TempId;
        if (i != this->ArgsVec.size() - 1)
        {
            std::cout << ", ";
        }
    }
    std::cout << ")";
    std::cout << "{\n";
    TempIdAllocater::Increase();
    if (this->FuncStm != NULL)
        this->FuncStm->Dump();

    std::cout << "}\n";
}

void RetIRT::Dump() const
{
    ExpValue Value;
    if (this->RetExp != NULL)
    {
        Value = this->RetExp->ExpDump();
        ConvertMemToTemp(Value);
        std::cout << "ret " << Value.TypeToString() << " " << Value.LabelToString();
        std::cout << "\n";
    }
    else
    {
        std::cout << "ret void\n";
    }
    TempIdAllocater::Increase();
}
void GlobalVarIRT::Dump() const 
{
    if (this->GlobalVarType == ValueType::INT32)
    {
        std::cout << "\n"
                  << "@" << this->GlobalVarName << " = ";
        if (this->IsConstant)
        {
            std::cout << "constant ";
        }
        else
        {
            std::cout << "global ";
        }
        PrintInitialStruct(this->DimVec, 0);
        if (this->InitVal != NULL)
        {
            ConvertIntTreeToInitializer(this->InitVal, this->DimVec);
        }
        else
        {
            if(this->Int32Val){
                std::cout << this->Int32Val;
            }
            else std::cout << " zeroinitializer";
        }
        std::cout << ", align 4\n";
    }
    else if (this->GlobalVarType == ValueType::FLOAT32)
    {
        std::cout << "\n"
                  << "@" << this->GlobalVarName << " = ";
        if (this->IsConstant)
        {
            std::cout << "constant ";
        }
        else
        {
            std::cout << "global ";
        }
        PrintInitialStruct(this->DimVec, 0);
        if (this->FloatInitVal != NULL)
        {
            ConvertFloatTreeToInitializer(this->FloatInitVal, this->DimVec);
        }
        else
        {
            if(this->Float32Val){
                std::cout << this->Float32Val;
            }
            else std::cout << " zeroinitializer";
        }
        std::cout << ", align 4\n";
    }
    else
    {
        // to be implemented
    }
}

void ExpIRT::Dump() const
{
    if (this->ContentKind == ExpKind::Call)
    {
        this->ExpContent->ExpDump();
    }
}
