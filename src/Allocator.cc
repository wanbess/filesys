#include"Allocator.h"
#include"string.h"
std::vector<unsigned long> BlockAlloc::alloc_BaseMap(unsigned long begin,unsigned long end,size_t leng,int num){
    char * temp=new char[(end-begin)*block_size];
    ch->read(begin,end-begin,(std::intptr_t)temp);
    std::vector<unsigned long> target;
    for(unsigned long i=0;i<leng;i+=8){
        char now=temp[i/8];
        for(int j=0;j<8;++j){
            if((now&(1<<j))==0){
                temp[i/8]|=(1<<j);
                target.push_back(i+j);
            }
            if(target.size()==num){
                ch->write(begin,(std::intptr_t)temp,(end-begin)*block_size);
                delete[] temp;        
                return target;
            }
        }
    }
    LOG("-- can't allocate enoufe space \r\n")
    delete[] temp;
    return target;
}
unsigned long BlockAlloc::alloc_Inode(){
   if(ch->getSpuerBlock()->inode_fn==0) return 0;
    unsigned long re=alloc_BaseMap(ch->inodeMapStart(),ch->blockMapStart(),ch->inodenum(), 1)[0];
    ch->getSpuerBlock()->inode_fn--; 
    LOG("inode no of : %lu allocate \r\n", re)
     return re;
}

std::vector<unsigned long> BlockAlloc::alloc_Block(size_t need){
    if(ch->getSpuerBlock()->block_fn<need)  return {};
    std::vector<unsigned long> re=alloc_BaseMap(ch->blockMapStart(),ch->inodeEntryStart(),ch->blocknum(), need);
      LOG("block no :")
     for(int i=0;i<re.size();++i) {
        LOG("%lu ",re[i])
        re[i]+=ch->blockEntryStart();
     }
     LOG("allocate \r\n")
     ch->getSpuerBlock()->block_fn-=need;
    
     return re;
}
bool BlockAlloc::release_Inode(unsigned long no_block){
   char * temp = new char[ (ch->blockMapStart()-ch->inodeMapStart())*block_size];
   ch->read(ch->inodeMapStart(),ch->blockMapStart()-ch->inodeMapStart(),(std::intptr_t)temp);
   temp[(no_block-ch->inodeEntryStart())/8] |= (1<<((no_block-ch->inodeEntryStart())%8));
   ch->write(ch->inodeMapStart(),(std::intptr_t)temp,ch->blockMapStart()-ch->inodeMapStart());
   char back[block_size];
   memset(back,0,block_size);
   ch->write(no_block,(std::intptr_t)back,block_size);
   LOG("-- inode %lu has been release \r\n",no_block)
   delete[] temp;
   return 1;
}

bool BlockAlloc::release_Block(unsigned long no_block){
    char * temp = new char[( ch->inodeEntryStart()-ch->blockMapStart())*block_size];
    ch-> read(ch->blockMapStart(),ch->inodeEntryStart()-ch->blockMapStart(),(std::intptr_t)temp);
    temp[((no_block-ch->blockEntryStart())/8)] |= (1<<((no_block-ch->blockEntryStart())%8));
    ch->write(ch->blockMapStart(),(std::intptr_t)temp,ch->inodeEntryStart()-ch->blockMapStart());
    char back[block_size];
    memset(back,0xff,block_size);
    ch->write(no_block,(std::intptr_t)back,block_size);
    LOG("-- data block %lu has been release \r\n",no_block)
    delete[] temp;
    return 1;
}
