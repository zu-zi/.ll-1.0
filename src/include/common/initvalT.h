#ifndef _INIT_VAL_TREE_
#include"common/base_ast.hpp"
#include"common/type_traits.h"
#include<vector>
/**
 * @brief this template class store the struct of init val,type may be BaseAST* or int
*/
template <typename T> 
class InitValTree
{
public:
  std::vector<InitValTree<T> *> childs;
  std::vector<T> keys;
  ~InitValTree(){
    for(auto &it:childs){
      delete it;
    }
    if constexpr(TypeTrait<T>::isPointer){
      for(auto &it:keys){
        delete it;
      }
    }
  }
  void Dump(){
    std::cout<<"{";
    for(auto &it:childs){
      it->Dump();
      std::cout<<" ";
    }
    for(auto &it:keys){
      if constexpr(std::is_same<T,int>::value){
        std::cout<<it << " ";
      }
      else{
        // BaseAST*
        it->Dump();
        std::cout<<" ";
      }
    }
    std::cout<<"}";
  }
  T FindFirst()
  {
    if(this->childs.size())
    {
      return this->childs[0]->FindFirst();
    }
    else
    {
      return this->keys[0];
    }
  }
  bool find(const std::vector<int> &index,T& result){
    return internal_find(index,result,0);
  }
private:
  bool internal_find(const std::vector<int> &index,T& result,int level){
    int size = this->childs.size();
    if(size){
      if(index[level]<0 || index[level]>=size){
        // return true;
        result = 0;
        return true;
      }
      return internal_find(index,result,level+1);
    }
    else{
      size = this->keys.size();
      if(index[level]<0 || index[level]>=size){
        result = 0;
        return true;
      }
      result = keys[index[level]];
      return false;
    }
  }
};
/**
 * @brief this function convert ast type tree to int val, used when const variable initialization
 * @return true indicate not const ast tree, otherwise false
 * @param AstTree target ast tree ptr
 * @param IntTree result int tree pointer referrence, will point to the result tree
*/
bool ConvertTreeToInt(InitValTree<BaseAST*> *AstTree,InitValTree<int>* &IntTree);
bool ConvertTreeToFloat(InitValTree<BaseAST *> *AstTree, InitValTree<float> *&FloatTree);
void PrintInitialStruct(const std::vector<int> &dim,int level);
void ConvertIntTreeToInitializer(InitValTree<int>* IntTree,const std::vector<int> &dim);
void ConvertFloatTreeToInitializer(InitValTree<float> *FloatTree, const std::vector<int> &dim);
void AdjustTree(InitValTree<BaseAST*>* &tree);

#define _INIT_VAL_TREE_
#endif