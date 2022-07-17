#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<chrono>
#include"log.h"
#define MAX_USER 20
#define CHEAK_BOUNDER
const int MAX_DIR_NAME =64;
const int MAX_INODE_ORDER = 3 ;
const size_t block_size=1024; //一个磁盘块的大小，可以容
const int i_data_block=12;
const int inode_size=256;//一个inode条目大小
const int maxa_dir_entry_size=64;  //目录键值对所占大小
const int i_first_order_block=1;
const int i_second_order_block=1;
const int i_third_order_block=1;
const int order_entry_num=block_size/sizeof(unsigned long); //一个磁盘块可容乃的间接块索引数量
const int i_block_num = i_data_block+i_first_order_block+i_second_order_block+i_third_order_block;
const int dir_entry_num = block_size/maxa_dir_entry_size; //一个磁盘块的目录数量
const long long max_dir_num = i_data_block*dir_entry_num+i_first_order_block*order_entry_num*dir_entry_num
+i_second_order_block*order_entry_num*order_entry_num*dir_entry_num+i_third_order_block*order_entry_num
*order_entry_num*order_entry_num*dir_entry_num;
const long long max_file_size=max_dir_num*(block_size/dir_entry_num);
struct Superblock{
    unsigned long transaction=0;
    unsigned long inode_n=0;
    unsigned long inode_fn=0;
    unsigned long block_n=0;
    unsigned long block_fn=0;
    unsigned long transaction_map_ptr=0;
    unsigned long inode_map_ptr=0;
    unsigned long inode_entry_ptr=0;
    unsigned long block_map_ptr=0;
    unsigned long block_entry_ptr=0;
    unsigned long end_block;
};

enum class FileType{
    Normal,
    Directory,
    Link
};
enum  class Mode{
    Write,
    Read,
    Excuse
};

struct Inode{
    unsigned long n_inode;
    FileType i_type;
    Mode i_auth;
    short block_use=0;
    union I_Count
    {
       unsigned long i_node_cnt=0;
       unsigned long i_block_cnt;
    }i_count;
    short order;
    size_t file_size=0;
    unsigned long i_block[i_block_num]={0};
    unsigned char unsers[MAX_USER]={0};
    int link=0;
    std::chrono::system_clock::time_point last_re;
} __attribute__((aligned(inode_size))) ;
struct Directory_entry{
    unsigned long offset=0;
    char dir_name[maxa_dir_entry_size-sizeof(unsigned long)];
};
struct Block{
#ifdef CHEAK_BOUNDER
 unsigned char b_eof;
#endif
 bool is_data;
 unsigned long b_offset;
unsigned long b_next;
 Directory_entry dir_entry[dir_entry_num];
};


struct  Indirect_block
{
    unsigned long offset[order_entry_num];
    short order;
    int num;
};

