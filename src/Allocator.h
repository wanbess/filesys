#pragma once
#include"channel.h"
class TraversalPolicy;
class BlockAlloc{
public:
BlockAlloc()=default;
explicit BlockAlloc( Channel* ptr):ch(ptr){}
   
    unsigned long alloc_Inode();
    std::vector<unsigned long> alloc_Block(size_t need);
    bool release_Inode(unsigned long );
    bool release_Block(unsigned long );
private:
 void init_all();
 std::vector<unsigned long> alloc_BaseMap(unsigned long begin,unsigned long end,size_t leng,int num);
 size_t need;
 Channel * ch;
 static TraversalPolicy *policy;
};
