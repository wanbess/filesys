#pragma once
#include<fcntl.h>
#include<stdlib.h>
#include<stdio.h>
#include<vector>
#include"block.h"
class Channel{
public:
    enum class readpolicy{
        READBYCACHE,
        NOCHA
    };
    enum class writepolict{
        ASYNIC,
        SYNIC
    };
    explicit Channel(size_t c,size_t transaction,size_t block,const char* file);
    ~Channel();
    void putInode(unsigned long offset, Inode *node);
    void putDirBlock(unsigned long offset,Block *node);
    void putOrderBlock(unsigned long offset,Indirect_block *node);
    void getOrderBlock(unsigned long offset,Indirect_block *node);
    void getInode(unsigned long offset,Inode*node);
    void getDirBlock(unsigned long offset,Block *node);
    void read(unsigned long offset,int read_b,std::intptr_t ptr);
    bool write(unsigned long offset,std::intptr_t content);
    bool write(unsigned long offset,std::intptr_t content,size_t buf);
    unsigned long transactionStart();
    unsigned long inodeMapStart();
    unsigned long blockMapStart();
    unsigned long inodeEntryStart();
    unsigned long blockEntryStart();
    unsigned long inodenum();
    unsigned long blocknum();
    bool isInodeUse(unsigned long);
    bool isBlockUse(unsigned long);
    void markInodeUse(unsigned long);
    void markBlockUse(unsigned long);
    size_t allNodeNum();
    Superblock* getSpuerBlock();
private:
 Superblock s_block;
 FILE* f;
 size_t capacity;
 const char* filename;
 const static unsigned long format_block=32; //每次格式化磁盘的盘块数
};