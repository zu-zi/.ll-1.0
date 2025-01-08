#include "common/initvalT.h"

bool ConvertTreeToInt(InitValTree<BaseAST *> *AstTree, InitValTree<int> *&IntTree)
{
    IntTree = new InitValTree<int>();
    for (auto &it : AstTree->childs)
    {
        InitValTree<int> *TempPtr;
        if (ConvertTreeToInt(it, TempPtr))
        {
            // if not const
            delete IntTree;
            IntTree = NULL;
            return true;
        }
        IntTree->childs.push_back(TempPtr);
    }
    for (auto &it : AstTree->keys)
    {
        int temp;
        if (it->GetConstVal(temp))
        {
            delete IntTree;
            IntTree = NULL;
            return true;
        }
        IntTree->keys.push_back(temp);
    }
    return false;
}

bool ConvertTreeToFloat(InitValTree<BaseAST *> *AstTree, InitValTree<float> *&FloatTree)
{
    // 创建一个新的 float 类型的树
    FloatTree = new InitValTree<float>();

    // 遍历子节点
    for (auto &it : AstTree->childs)
    {
        InitValTree<float> *TempPtr;
        // 递归转换每个子节点
        if (ConvertTreeToFloat(it, TempPtr))
        {
            // 如果转换失败（遇到变量或不合法值）
            delete FloatTree;
            FloatTree = NULL;
            return true;
        }
        // 将转换后的子节点加入 FloatTree
        FloatTree->childs.push_back(TempPtr);
    }

    // 遍历树中的键，转换常量值
    for (auto &it : AstTree->keys)
    {
        float temp;
        // 如果不能获取常量值，说明该值无法转换为 float
        if (it->GetConstVal(temp))
        {
            delete FloatTree;
            FloatTree = NULL;
            return true;
        }
        // 将常量值加入 FloatTree
        FloatTree->keys.push_back(temp);
    }

    // 返回转换成功
    return false;
}


void PrintInitialStruct(const std::vector<int> &dim, int level)
{
    if (level == dim.size() || dim[0] == 0)
    {
        std::cout << "i32 ";
        return;
    }
    std::cout << "[ " << dim[level] << " x ";
    PrintInitialStruct(dim, level + 1);
    std::cout << "]";
}
std::string GetInitialStruct(const std::vector<int> &dim, int level)
{
    std::string res("");
    for (int i = level; i < dim.size(); ++i)
    {
        res += "[ ";
        res += std::to_string(dim[i]);
        res += " x";
    }
    res += " i32 ";
    for (int i = level; i < dim.size(); ++i)
    {
        res += "] ";
    }
    return res;
}
std::string InternalInitializerConvert(InitValTree<int> *IntTree, const std::vector<int> &dim,int level)
{
    std::string res("");
    int len;

    if(level>=dim.size())
    {
        len = 1;
    }
    else
    {
        len = dim[level];
    }
    if(len!=1)
    {
        res += "[";
    }
    if(IntTree->childs.size())
    {
        int i;

        for(i = 0;i < IntTree->childs.size();++i)
        {
            res += InternalInitializerConvert(IntTree->childs[i],dim,level+1);
            if(i!=len-1)
            {
                res += ", ";
            }
        }
        if(level==dim.size()-1)
        {
            for( ; i < len; ++i)
            {
                res += "i32 0";
                if(i!=len-1)
                {
                    res += ", ";
                }
            }
        }
        else
        {
            for( ; i < len; ++i)
            {
                res += GetInitialStruct(dim,level);
                if(i!=len-1)
                {
                    res += ", ";
                }
            }
        }
    }
    else 
    {
        res += "i32 "+std::to_string(IntTree->keys[0]);
    }
    if(len!=1)
    {
        res += "]";
    }
    return res;
}

std::string InternalInitializerConvertf(InitValTree<float> *FloatTree, const std::vector<int> &dim, int level) 
{
    std::string res("");
    int len;

    // 判断当前维度级别是否超过数组的维度
    if(level >= dim.size()) {
        len = 1;
    } else {
        len = dim[level];
    }

    // 如果当前维度不止一维，添加 '[' 来表示数组
    if(len != 1) {
        res += "[";
    }

    // 如果当前层级有子节点，递归处理子节点
    if(FloatTree->childs.size()) {
        int i;

        // 处理子节点
        for(i = 0; i < FloatTree->childs.size(); ++i) {
            res += InternalInitializerConvertf(FloatTree->childs[i], dim, level + 1);
            if(i != len - 1) {
                res += ", ";
            }
        }

        // 如果当前维度是最后一维，填充0
        if(level == dim.size() - 1) {
            for( ; i < len; ++i) {
                res += "float 0.0";
                if(i != len - 1) {
                    res += ", ";
                }
            }
        } else {
            // 如果当前维度不是最后一维，递归处理并继续填充
            for( ; i < len; ++i) {
                res += GetInitialStruct(dim, level);  // 递归调用初始化结构函数
                if(i != len - 1) {
                    res += ", ";
                }
            }
        }
    }
    // 如果没有子节点，直接输出值
    else {
        res += "float " + std::to_string(FloatTree->keys[0]);
    }

    // 如果当前维度不止一维，添加 "]" 关闭数组表示
    if(len != 1) {
        res += "]";
    }

    return res;
}


void ConvertIntTreeToInitializer(InitValTree<int> *IntTree, const std::vector<int> &dim)
{
    std::string res (""),head(""),tail(""),temp("");
    // get init value tree depth
    int depth = 0;
    auto p = IntTree->childs[0];
    while(1)
    {
        if(p->childs.size())
        {
            depth++;
            p = p->childs[0];
        }
        else
        {
            break;
        }
    }
    // if no array
    if(depth==0)
    {
        std::cout<<" i32 "<<IntTree->FindFirst();
        return;
    }
    // if array
    // handle the case init value dimension less than the array dimension
    if(depth<dim.size())
    {

        for(int i = 0;i<dim.size()-depth;++i)
        {
            head += "[ ";
            temp = "";
            for(int j=1;j<dim[i];++j)
            {
                temp +=  GetInitialStruct(dim,i+1) + "zeroinitializer" ;
                if(j!=dim[i]-1)
                {
                    temp += ", ";
                }
            }
            tail = temp + "]" + tail;
        }
        tail = ", " + tail;
    }
    // handle init value
    res = head + InternalInitializerConvert(IntTree->childs[0],dim,dim.size()-depth) + tail;
    // return res;
    std::cout<<res;
}

void ConvertFloatTreeToInitializer(InitValTree<float> *FloatTree, const std::vector<int> &dim) 
{
    std::string res(""), head(""), tail(""), temp("");
    
    // 获取初始化值树的深度
    int depth = 0;
    auto p = FloatTree;
    while (!p->childs.empty()) 
    {
        depth++;
        p = p->childs[0];
    }

    // 如果没有数组（标量情况）
    if (depth == 0) 
    {
        std::cout << " float " << FloatTree->FindFirst();
        return;
    }

    // 如果是数组，处理初始化值维度小于数组维度的情况
    if (depth < dim.size()) 
    {
        for (int i = 0; i < dim.size() - depth; ++i) 
        {
            head += "[ ";
            temp = "";
            for (int j = 1; j < dim[i]; ++j) 
            {
                temp += GetInitialStruct(dim, i + 1) + "zeroinitializer";
                if (j != dim[i] - 1) 
                {
                    temp += ", ";
                }
            }
            tail = temp + "]" + tail;
        }
        tail = ", " + tail;
    }

    // 递归处理初始化值树，并生成初始化字符串
    res = head + InternalInitializerConvertf(FloatTree, dim, dim.size() - depth) + tail;

    // 输出最终的结果
    std::cout << res;
}



void AdjustTree(InitValTree<BaseAST *> *&tree)
{
    for (auto &it : tree->childs)
    {
        AdjustTree(it);
    }
    if (tree->childs.size() == 1)
    {
        InitValTree<BaseAST *> *temp = tree->childs[0];
        free(tree);
        tree = temp;
    }
}