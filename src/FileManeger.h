#pragma once
#include"channel.h"
#include"Allocator.h"
#include<memory>

class FileManeger {
public:
  FileManeger();
  FileManeger(std::unique_ptr<Channel>&& ch,std::unique_ptr<BlockAlloc>&& allocator);
 void mkdir(const char* str);
 bool remove(const char* str,FileType type);
 bool find(const char* str,FileType type);
 bool findfileInBlock(const char* str,unsigned long no_block,Inode* target,FileType type);
 bool findFileInOrderBlock(const char * str,unsigned long no_block,Inode* target,int order,FileType type);
  std::unique_ptr<Inode> findFile(const char* str,Inode* root,FileType type);
  bool findSpaceInBlock(const char* str,unsigned long no_block,Inode *target,FileType  type);
  bool findSpaceInOrderBlock(const char* str,unsigned long no_block,Inode* target,short order,FileType type);
  std::unique_ptr<Inode> creatDir(const char* str,Inode* root,FileType type);
private:
  void InitLinkInode(unsigned long last_no,Inode* node,Inode* target);
  void removeInode(Inode* node);
  void removeAllInBlock(unsigned long no_block,FileType type);
  void removeOrderBlock(unsigned long no_block,short order,FileType type);
  void InitDirInode(unsigned long last_no,Inode* node,FileType type);
  void InitRootNode();
  Inode*  getrootInode();
  std::unique_ptr<Channel> ch;
 std::unique_ptr<BlockAlloc> allocator;
 std::unique_ptr<Inode> root_node;
};
