#include"user.h"
#define CHEAKCONNECT   \
    if(!connect){         \
        LOG("No FIle sys in now User \r\n")    \
        return ;                        \
    }

User::User():connect(0){}
int User::bind(const FileManeger& m){
     std::unique_ptr<Inode> root= m.getRoot();
     inode_st.emplace_back(std::move(root),"root");
     now_node=inode_st.back().first.get();
     connect=1;
}
//注意：当record为true时，不使用返回值，而使用inode_st的末尾元素
 std::unique_ptr<Inode> User:: findpath(Inode* root, std::vector<std::string> &path,bool record){
      std::string info;
      Inode* now=root;
      std::unique_ptr<Inode> node;
      for(auto &str:path){
        info+=str;
        info.push_back('/');     
        node = maneger->changeDir(str.c_str(),now);
        if(node==nullptr){
            LOG("can't find path:  %s \r\n",info)
            return 0;
        }
//每次进入新目录后node接管当前inode所有权,若record有效，当node被move进inode_st中后,需要重新
//从inode_st中取得当前inode指针;否则,now从node得到inode指针,在下一次move assign
//更新node销毁当前inode资源前,now指针均有效
        if(record){
          inode_st.emplace_back(std::move(node),std::forward<std::string>( str));
          now=inode_st.back().first.get(); 
        }else{
            now=node.get();
        }
    }
    return node;
 }
bool User::commandCd(Command& com){
    CHEAKCONNECT
    instrcts.push_back(com);
    if(com.compath.root_b){
       findpath(now_node,com.compath.rpath,1);
    }else{
       findpath(maneger->getRoot().get(),com.compath.rpath,1);
    } 
    now_node=inode_st.back().first.get();
    return 1;
}
bool User::commandMkdir(Command& com){
    CHEAKCONNECT 
    instrcts.push_back(com);
    std::string last = com.compath.rpath.back();
    com.compath.rpath.pop_back();
     std::unique_ptr<Inode> node;
    if(com.compath.root_b)  node = findpath(now_node,com.compath.rpath,0);
    else node = findpath(maneger->getRoot().get(),com.compath.rpath,0);
}
bool User::commandRm(Command& com){
    CHEAKCONNECT
    instrcts.push_back(com);
    bool ret=0;
    Inode* t_now=now_node;
    if(com.para.size()>1){
        LOG("too much argument \r\n")
        return 0;
    }else if(com.para.size()==0){
        std::string filename = com.compath.rpath.back();
        com.compath.rpath.pop_back();
        if(!commandCd(com)) return 0;
        std::unique_ptr<Inode> node;
        if(com.compath.root_b) node=  findpath(now_node,com.compath.rpath,0);
        else  node=  findpath(maneger->getRoot().get(),com.compath.rpath,0);
        if( maneger->remove(filename.c_str(),node.get(),FileType::Normal)) return 1;
        if( maneger->remove(filename.c_str(),node.get(),FileType::Directory)){
            LOG("bad input: can't rm a dir \r\n") 
            return 0;
        }            
    }else{
       switch(com.para[0]){
         case ArgType::_R :
           
         break;
       }
    }
}