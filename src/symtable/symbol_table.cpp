#include"symtable/symbol_table.h"
int SymbolTable::UnNamedStringCounter;
std::vector<std::map<std::string,Symbol*>> SymbolTable::TableVec;
std::map<std::string,std::string> SymbolTable::ConstStringMap;
void replace_str(std::string& str, const std::string& before, const std::string& after)
{
	for (std::string::size_type pos(0); pos != std::string::npos; pos += after.length())
	{
		pos = str.find(before, pos);
		if (pos != std::string::npos)
			str.replace(pos, before.length(), after);
		else
			break;
	}
}
void SymbolTable::PrintConstStringDeclare()
{
    for(auto &it:ConstStringMap)
    {
        // it.first.replace(it.first.find("\n"),1,"\\0A");
        std::string str(it.first);
        replace_str(str,"\n","\\0A");
        std::cout<<it.second<<" = private unnamed_addr constant [ "<<it.first.length() + 1<<" x i8 ]c\""<<str<<"\\00\", align 1\n";
    }
}
bool SymbolTable::AddSymbol(std::string name,Symbol* sym){
    std::string SymName("");
    int LastTableIndex = TableVec.size()-1;
    if(LastTableIndex!=0){
        sym->GlobalFlag = false;
    }else{
        sym->GlobalFlag = true;
    }
    if(TableVec[LastTableIndex].find(name)!=TableVec[LastTableIndex].end()) {
        delete sym;
        return true;
    }
    TableVec[LastTableIndex].insert(std::pair<std::string,Symbol*>(name,sym));
    return false;
}
Symbol* SymbolTable::FindSymbol(std::string name){
    for(auto it = TableVec.rbegin();it!=TableVec.rend();it++){
        auto FindIt = it->find(name);
        if(FindIt!=it->end()){
            return FindIt->second;
        }
    }
    return NULL;
}
Symbol* SymbolTable::FindLocalScopeSymbol(std::string name)
{
    auto FindIt = TableVec[TableVec.size()-1].find(name);
    if(FindIt != TableVec[TableVec.size()-1].end())
    {
        return FindIt->second;
    }
    return NULL;
}
std::string SymbolTable::AddConstString(std::string &str)
{
    // remove the '"' at the head and tail of the str.
    if (str.front() == '"') {
        str.erase(0, 1);
    }
    if (str.back() == '"') {
        str.pop_back();
    }
    replace_str(str,"\\n","\n");
    auto it = ConstStringMap.find(str);
    if(ConstStringMap.find(str)!=ConstStringMap.end())
    {
        return it->second;
    }
    int TempId = UnNamedStringCounter++;
    std::string label("");
    if(TempId==0)
    {
        label = "@.str";
    }
    else
    {
        label = "@.str."+std::to_string(TempId);
    }
    ConstStringMap.insert(std::pair<std::string,std::string>(str,label));
    return label;
}
void SymbolTable::EnterScope(){
    TableVec.push_back(std::map<std::string,Symbol*>());
}
void SymbolTable::ExitScope(){
    for(auto &it:TableVec[TableVec.size()-1]){
        delete it.second;
    }
    TableVec.pop_back();
}
void SymbolTable::InitTable() {
    if (TableVec.size() == 0) {
        TableVec.push_back(std::map<std::string, Symbol*>());

        // add function
        std::vector<ArgsType> args;

        // get int
        args.clear();
        Symbol* sym = new Symbol(ValueType::INT32, args);
        TableVec[0].insert(std::pair<std::string, Symbol*>("getint", sym));

        // getch
        args.clear();
        sym = new Symbol(ValueType::INT32, args);
        TableVec[0].insert(std::pair<std::string, Symbol*>("getch", sym));

        // getchar
        sym = new Symbol(ValueType::INT32, args);
        TableVec[0].insert(std::pair<std::string, Symbol*>("getchar", sym));

        // get array (int32 array)
        args.clear();
        args.push_back(ArgsType::Int32Array);
        sym = new Symbol(ValueType::INT32, args);
        TableVec[0].insert(std::pair<std::string, Symbol*>("getarray", sym));

        // get array (float32 array)
        args.clear();
        args.push_back(ArgsType::Float32Array);
        sym = new Symbol(ValueType::FLOAT32, args);
        TableVec[0].insert(std::pair<std::string, Symbol*>("getfarray", sym));

        // putint
        args.clear();
        args.push_back(ArgsType::Int32);
        sym = new Symbol(ValueType::VOID, args);
        TableVec[0].insert(std::pair<std::string, Symbol*>("putint", sym));

        // putch
        args.clear();
        args.push_back(ArgsType::Int32);
        sym = new Symbol(ValueType::VOID, args);
        TableVec[0].insert(std::pair<std::string, Symbol*>("putch", sym));

        // putchar
        TableVec[0].insert(std::pair<std::string, Symbol*>("putchar", sym));

        // putarray (int32 array)
        args.clear();
        args.push_back(ArgsType::Int32);
        args.push_back(ArgsType::Int32Array);
        sym = new Symbol(ValueType::VOID, args);
        TableVec[0].insert(std::pair<std::string, Symbol*>("putarray", sym));

        // putarray (float32 array)
        args.clear();
        args.push_back(ArgsType::Int32);
        args.push_back(ArgsType::Float32Array);
        sym = new Symbol(ValueType::VOID, args);
        TableVec[0].insert(std::pair<std::string, Symbol*>("putfarray", sym));

        // putfloat
        args.clear();
        args.push_back(ArgsType::Float32);
        sym = new Symbol(ValueType::VOID, args);
        TableVec[0].insert(std::pair<std::string, Symbol*>("putfloat", sym));

        // printf
        args.clear();
        args.push_back(ArgsType::String);
        args.push_back(ArgsType::VarsPacket);
        sym = new Symbol(ValueType::INT32, args);
        TableVec[0].insert(std::pair<std::string, Symbol*>("printf", sym));

        // putf
        args.clear();
        args.push_back(ArgsType::String);
        args.push_back(ArgsType::VarsPacket);
        sym = new Symbol(ValueType::VOID, args);
        TableVec[0].insert(std::pair<std::string, Symbol*>("putf", sym));
    }
}

bool Symbol::GetGlobalFlag() const{
    return this->GlobalFlag;
}
std::string Symbol:: GetLabelStr(std::string SymName) const{
    if(this->GlobalFlag){
        return "@" + SymName;
    }else{
        return "%" + SymName;
    }
}

void SymbolTable::AddGlobalSym (std::string name,Symbol* sym)
{
    TableVec[0].insert(std::pair<std::string,Symbol*>(name,sym));
}