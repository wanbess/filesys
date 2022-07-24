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
     connect=1;
}

bool User::commandCd(Command& com){
    CHEAKCONNECT
    instrcts.push_back(com);
    std::string info;
    for(int i=0;i<com.rpath.size()-1;++i){
        std::string& str=com.rpath[i];
        info+=str;
        info.push_back('/');     
        std::unique_ptr<Inode> node= maneger->changeDir(str.c_str(),now_node);
        if(node==nullptr){
            LOG("can't find path:  %s \r\n",info)
            return 0;
        }
        inode_st.emplace_back(std::move(node),std::forward<std::string>( str));
        now_node=inode_st.back().first.get();
    }
    return 1;
}
bool User::commandMkdir(Command& com){
    CHEAKCONNECT 
    instrcts.push_back(com);
    std::string last = com.rpath.back();
    com.rpath.pop_back();
    if(commandCd(com)){
       maneger->mkdir(last.c_str(),now_node);
    }
}
bool User::commandRm(Command& com){
    CHEAKCONNECT
    instrcts.push_back(com);
    Inode* t_now=now_node;
    if(com.para.size()>1){
        LOG("too much argument \r\n")
        return 0;
    }else if(com.para.size()==0){
        std::string filename = com.rpath.back();
        com.rpath.pop_back();
        if(!commandCd(com)) return 0;
            
    }else{

    }
}