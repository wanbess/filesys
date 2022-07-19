#include"FileManeger.h"
#include"string.h"
#include"assert.h"
FileManeger::FileManeger(){
    ch = std::move(std::unique_ptr<Channel>(new Channel(1024,1024,0,"default.fs")) );
    allocator= std::move(std::unique_ptr<BlockAlloc>(new BlockAlloc(ch.get())));
    InitRootNode();
}
FileManeger::FileManeger(std::unique_ptr<Channel>&& ch,std::unique_ptr<BlockAlloc>&& allocator): ch(std::move(ch)),allocator(std::move(allocator)){
}

//Ϊ�µ�inode��ʼ��
void FileManeger::InitDirInode(unsigned long last_no,Inode* target,FileType type){
    target->block_use=1;
    target->i_count.i_node_cnt=2;
    target->i_type=type;
    target->last_re =std::chrono::system_clock::now();
    target->i_block[0]=allocator->alloc_Block(1)[0];
    Block block;
    ch->getDirBlock(target->i_block[0],&block);
    const char* now_path=".";
    const char* last_path="..";
    strcpy(block.dir_entry[0].dir_name,now_path);
    strcpy(block.dir_entry[1].dir_name,last_path);
    block.dir_entry[0].offset=target->n_inode;
    block.dir_entry[1].offset=last_no;
    ch->putInode(target->n_inode,target);
    ch->putDirBlock(target->i_block[0],&block);
}
bool FileManeger::findfileInBlock(const char* str,unsigned long no_block,Inode* target,FileType type){
    Block now_block;
    Inode temp;
    ch->getDirBlock(no_block,&now_block);
    for(int i=0;i<dir_entry_num;i++){
        const char * buf =now_block.dir_entry[i].dir_name;
        unsigned long offset=now_block.dir_entry[i].offset;
        if(offset==0xffffffffffffffff) continue;
        if(strcmp(buf,str)==0){
            ch->getInode(offset,&temp);
            if(temp.i_type==type){
                *target =std::move(temp);
                LOG("-- find inode in block %lu name %s \r\n",now_block.dir_entry[i].offset,str)
                return 1;
            }
        }
    }
    return 0;
}
void FileManeger::InitRootNode(){
    root_node=std::move(std::unique_ptr<Inode>(new Inode()));
    unsigned long offset=allocator->alloc_Inode();
    InitDirInode(offset,root_node.get(),FileType::Directory);
}
 std::unique_ptr<Inode> FileManeger::findFile(const char* str,Inode* root,FileType type){
  
    if(root->i_type==FileType::Directory){
        std::unique_ptr<Inode> node(new Inode());
        for(short i=0;i<MIN(i_data_block,root->block_use);++i){
           if(findfileInBlock(str,root->i_block[i],node.get(),type)) 
              return node;
        }
        for(short i=i_data_block;i<MIN(root->block_use,i_data_block+i_first_order_block);++i){
            if(findFileInOrderBlock(str,root->i_block[i],node.get(),1,type)) return node;
        }
        for( short i=i_data_block+i_first_order_block;i<MIN(root->block_use,i_first_order_block+i_data_block+i_second_order_block);++i){
            if(findFileInOrderBlock(str,root->i_block[i],node.get(),2,type)) return node;
        }
        for(short i=i_data_block+i_first_order_block+i_second_order_block;
        i<MIN(root->block_use,i_data_block+i_first_order_block+i_second_order_block+i_third_order_block);++i){
            if(findFileInOrderBlock(str,root->i_block[i],node.get(),3,type)) return node;
        }     
    }
    
    LOG("can't find target file: %s in node dir \r\n",str)
        return nullptr;
 }
 bool FileManeger::findFileInOrderBlock(const char * str,unsigned long no_block,Inode* target,int order,FileType type){
    if(order==0){
        return findfileInBlock(str,no_block,target,type);
    }
    Indirect_block now_block;
    ch->getOrderBlock(no_block,&now_block);
    for(int i=0;i<order_entry_num;++i){
        if(findFileInOrderBlock(str,now_block.offset[i],target,order-1,type)) 
           return 1;
    }
    return 0;
}
/***  ������block���ҵ�һ����е�dir entry���������µ�inode��
*/
bool FileManeger::findSpaceInBlock(const char* str,unsigned long no_block,Inode *target,FileType type){
    Block now_block;
    Inode temp;
    ch->getDirBlock(no_block,&now_block);
    for(int i=0;i<dir_entry_num;++i){
        const char * buf =now_block.dir_entry[i].dir_name;
        unsigned long offset = now_block.dir_entry[i].offset;
        if(offset==(unsigned long) 0xffffffffffffffff){
            now_block.dir_entry[i].offset = allocator->alloc_Inode();
            strcpy(now_block.dir_entry[i].dir_name,str);
            ch->putDirBlock(no_block,&now_block);
            target->n_inode=now_block.dir_entry[i].offset;
            return 1;
        }
    }
    return 0;
}

bool FileManeger::findSpaceInOrderBlock(const char * str,unsigned long no_block,Inode *target,short order,FileType type){
   if(order==0){
    return findSpaceInBlock(str,no_block,target,type);
   }
   Indirect_block now_block;
   ch->getOrderBlock(no_block,&now_block);
   for(int i=0;i<order_entry_num;++i){
    if(now_block.offset[i]){
        if(findSpaceInOrderBlock(str,now_block.offset[i],target,order-1,type))
          return 1;
    }else{
        now_block.offset[i]= allocator->alloc_Block(1)[0];
        bool finded=findFileInOrderBlock(str,now_block.offset[i],target,order-1,type);
        if(!finded) LOG("-- error in alloct Order block:  %lu \r\n",now_block.offset[i])
        return 1;
    }
   }
   return 0;
}
std::unique_ptr<Inode> FileManeger::creatDir(const char* str,Inode* root,FileType type){
  
    if(root->i_type==FileType::Directory){
        std::unique_ptr<Inode> node(new Inode());
        if(findFile(str,root,type)) return node;
        bool finded=0;
        unsigned short i=0;
        for(;i<MIN(root->block_use,i_data_block);++i){
            if(findSpaceInBlock(str,root->i_block[i],node.get(),type)){
                finded=true;
                root->block_use=MAX(root->block_use,i+1);
                InitDirInode(root->n_inode,node.get(),type);
                LOG("creat new dir in  root inode no %d entry \r\n",i)
                return node;
            }
        }
        if(root->block_use<i_data_block){
            root->i_block[i]=allocator->alloc_Block(1)[0];
            root->block_use++;
            bool r=findSpaceInBlock(str,root->i_block[i],node.get(),type);
            assert(r);
            InitDirInode(root->n_inode,node.get(),type);
            return node;
        }
        for(;i<MIN(root->block_use,i_data_block+i_first_order_block);++i){
            if(findSpaceInOrderBlock(str,root->i_block[i],node.get(),1,type)) return node;    
        }
        if(!finded&&root->block_use<i_data_block+i_first_order_block){
               
        }
        for(;i<MIN(root->block_use,i_data_block+i_first_order_block+i_second_order_block);++i){
             if(findSpaceInOrderBlock(str,root->i_block[i],node.get(),2,type)){
                root->block_use=MAX(root->block_use,i+1);
                root->i_count.i_block_cnt++;
                InitDirInode(root->n_inode,node.get(),type);
                return node;
             }
        }
       
    }
    LOG("there is no enough space for creat dir %s \r\n",str)
    return nullptr;
}
Inode* FileManeger:: getrootInode(){  
    return root_node.get();
}

void FileManeger:: mkdir(const char* str){
    Inode* node=getrootInode();
    LOG("root node number is %lu \r\n",node->n_inode);
    creatDir(str,node,FileType::Directory);
}
bool FileManeger::find(const char * str,FileType type){
    Inode* node=getrootInode();
  std::unique_ptr<Inode> re=  findFile(str,node,FileType::Directory);
  if(re) return 1;
  else return 0;
}
// 清除目录块中所有的子内容
void FileManeger::removeAllInBlock(unsigned long no_block,FileType type){
    assert(type!=FileType::Normal);
    if(type==FileType::Directory){
        Block block;
        ch->getDirBlock(no_block,&block);
        for(int i=0;i<dir_entry_num;++i){
           if( block.dir_entry->offset ==0xffffffffffffffff) continue;
           Inode node;
           unsigned long offset=block.dir_entry[i].offset;
           ch->getInode(offset,&node);
           removeInode(&node);
        }
    }else{

    }
    allocator->release_Block(no_block);
}

void FileManeger::removeOrderBlock(unsigned long no_block,short order,FileType type){
   //此时block为正常块，若要删除的是文件，直接将该块删去；若删除的目录，此时block是目录键值对，需要调用
    if(order==0){
       if(type==FileType::Normal) allocator->release_Block(no_block);
       else  removeAllInBlock(no_block,type);
    }
    Indirect_block in_block;
    ch->getOrderBlock(no_block,&in_block);
    for(int i=0;i<order_entry_num;++i){
        if(in_block.offset[i]==0xffffffffffffffff) continue;
        removeOrderBlock(in_block.offset[i],order-1,type);
    }
    allocator->release_Block(no_block);
}
void FileManeger::removeInode(Inode *node){
    allocator->release_Inode(node->n_inode);
    short i=0;
    for(;i<MIN(i_data_block,node->block_use);++i){
        if(node->i_block[i]==0xffffffffffffffff) continue;
        if(node->i_type==FileType::Normal)
          allocator->release_Block(node->i_block[i]);
        else  removeAllInBlock(node->i_block[i],node->i_type);
    }
    if(node->block_use<=i_data_block){
        LOG("release %d block in no:%lu inode, belong data block \r\n",node->block_use,node->n_inode);
        return ;
    }
    for(;i<MIN(node->block_use,i_data_block+i_first_order_block);++i){
          if(node->i_block[i]!=0xffffffffffffffff) 
          removeOrderBlock(node->i_block[i],1,node->i_type);
    }
    if(node->block_use<=i_data_block+i_first_order_block){
        LOG("release %d block in no:%lu inode, belong first order block \r\n",node->block_use,node->n_inode);
        return ;
    }
    for(;i<MIN(node->block_use,i_data_block+i_first_order_block+i_second_order_block);++i){
        if(node->i_block[i]!=0xffffffffffffffff) 
        removeOrderBlock(node->i_block[i],2,node->i_type);
    }
    if(node->block_use<=i_data_block+i_first_order_block+i_second_order_block){
        LOG("release %d block in no:%lu inode, belong second order block \r\n",node->block_use,node->n_inode);
        return ;
    }
    for(;i<MIN(node->block_use,i_data_block+i_first_order_block+i_second_order_block+i_third_order_block);++i){
        if(node->i_block[i]!=0xffffffffffffffff) 
        removeOrderBlock(node->i_block[i],3,node->i_type);
    }
    LOG("release %d block in no:%lu inode, belong third order block \r\n",node->block_use,node->n_inode);
}

bool FileManeger::remove(const char* str,FileType type){
    Inode * root = getrootInode();
    std::unique_ptr<Inode> node =  findFile(str,root,type);
    if(node==nullptr){
        LOG("can't remove  %s in root because it doense't exit \r\n",str);
        return 0;
    }
    removeInode(node.get());
    return 1;
}
