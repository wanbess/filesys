#include"channel.h"
#include"assert.h"
#include<stdlib.h>
#include<string.h>
void init_Superblock(Superblock* s_block){
    LOG("-- Read to inti dist space \r\n")
    s_block->transaction_map_ptr=sizeof(Superblock)/block_size+((sizeof(Superblock)%block_size)>0);
    LOG("-- transaction data begin in BLock: %lu \r\n",s_block->transaction_map_ptr)
    s_block->inode_map_ptr=s_block->transaction_map_ptr+s_block->transaction/block_size+((s_block->transaction%block_size)>0);
    LOG("-- inode map begin in BLock: %lu \r\n",s_block->inode_map_ptr) 
    s_block->block_map_ptr=s_block->inode_n/block_size+(((s_block->inode_n)%block_size)>0)+s_block->inode_map_ptr ;
    LOG(" --data block map begin in Block: %lu \r\n",s_block->block_map_ptr)
    s_block->inode_entry_ptr = s_block->block_map_ptr+s_block->block_n/block_size+((s_block->block_n%block_size)>0) ;
    LOG("-- inode entry begin in Block: %lu \r\n",s_block->inode_entry_ptr)
    s_block->block_entry_ptr = s_block->inode_entry_ptr+ s_block->inode_n*inode_size/block_size + (((s_block->inode_n*inode_size)%block_size)>0);
    LOG("-- data block entry begin in Block: %lu \r\n",s_block->block_entry_ptr)
    s_block->end_block=s_block->block_entry_ptr+s_block->block_n;
    LOG("-- all dists spaces end in Block: %lu \r\n",s_block->end_block) 
}
 Channel::Channel(size_t c,size_t block,size_t transaction,const char* file):capacity(c),filename(file){
    f=fopen(file,"w+");
    s_block.inode_n=c;
    s_block.block_n=block;
    s_block.block_fn=s_block.block_n;
    s_block.inode_fn=s_block.inode_n;
    s_block.transaction=transaction;
    init_Superblock(&s_block);
    size_t re=fwrite(&s_block,sizeof(s_block),1,f);
    if(re!=1) LOG("fail to write superblock %s \r\n",filename) 
   {
    char maptemp[block_size*(inodeEntryStart()-inodeMapStart())]={0};
     write(inodeMapStart(),(std::intptr_t)maptemp,(inodeEntryStart()-inodeMapStart())*block_size);
   }  //Inode 一定位block的整数倍，不需要额外对齐
    LOG("-- init map block \r\n")
    Inode nodes[s_block.inode_n];
    for(unsigned long i=0;i<s_block.inode_n;++i){
        nodes[i].n_inode=i;
    }
    write(inodeEntryStart(),(std::intptr_t)nodes,(blockEntryStart()-inodeEntryStart())*block_size);
    LOG("-- init inode entry \r\n")
    for(unsigned long start=blockEntryStart();start<allNodeNum();start+=format_block){
        char temp[format_block*block_size];
        memset(temp,0xff,format_block*block_size);
        write(start,(std::intptr_t)temp,(MIN(format_block,allNodeNum()-start))*block_size);     
    }
    LOG("-- finish format dist \r\n")
}
Channel::~Channel(){
    int r=fclose(f);
   // if(f) LOG("fail to close channel file \r\n")
}
 void Channel::read(unsigned long offset,int read_b,std::intptr_t ptr){
    fseek(f,offset*block_size,SEEK_SET);
    fread((char*)ptr,read_b,block_size,f);
   // LOG("%lu bytes data read from block %lu \r\n",block_size*read_b,offset)
}
 bool Channel::write(unsigned long offset, std::intptr_t ptr, size_t buf){
    fseek(f,offset*block_size,SEEK_SET);
    fwrite((char*)ptr,buf,1,f);
    fflush(f);
   // LOG("%lu bytes data write to  block %lu \r\n",buf,offset)
    return 1;
}
 void Channel::getDirBlock(unsigned long offset,Block *block){
   assert(block!=nullptr);
   read(offset,1,(std::intptr_t)block->dir_entry);
}
 void Channel::putDirBlock(unsigned long offset,Block* block){
    write(offset,(std::intptr_t)block->dir_entry,block_size);
}
 void Channel::getInode(unsigned long offset,Inode *node){
    unsigned long position=offset*inode_size+inodeEntryStart()*block_size;
    fseek(f,position,SEEK_SET);
    fread(node,sizeof(Inode),1,f);

}
 void Channel::putInode(unsigned long offset, Inode* node){
    unsigned long position=offset*inode_size+inodeEntryStart()*block_size;
    fseek(f,position,SEEK_SET);
    fwrite(node,sizeof(Inode),1,f);
    fflush(f);
}
 void Channel::getOrderBlock(unsigned long offset, Indirect_block *b){
    read(offset,1,(std::intptr_t)b->offset);
}
 void Channel::putOrderBlock(unsigned long offset,Indirect_block *b){
    write(offset,(std::intptr_t)b->offset,block_size);
}
 unsigned long Channel::inodeMapStart(){
    return s_block.inode_map_ptr;
}
 unsigned long Channel::blockMapStart(){
    return s_block.block_map_ptr;
}
 unsigned long Channel::inodeEntryStart(){
    return s_block.inode_entry_ptr;
}
 unsigned long Channel::blockEntryStart(){
    return s_block.block_entry_ptr;
}
 unsigned long Channel::allNodeNum(){
    return s_block.end_block;
}
 Superblock* Channel::getSpuerBlock(){
    return &s_block;
}
  unsigned long Channel::inodenum(){
    return s_block.inode_n;
}
 unsigned long Channel::blocknum(){
    return s_block.block_n;
}