#include<iostream>
#include<string>
#include "../src/FileManeger.h"
int main(){
   FileManeger f;
    std::vector<std::string> vc;
    for(int i=0;i<500;++i){
        std::string s("str");
        s+= std::to_string(i);
        vc.push_back(s);
        f.mkdir(s.c_str());
    }
    LOG("-- test remove -- \r\n")
    LOG("-- ---- ---- ---- --\r\n")
    int cnt=0;
    for(auto &it:vc){
        LOG("before remove:   ")
        cnt++;
         if(f.find(it.c_str(),FileType::Directory))
         LOG("is find?:true \r\n")
         else LOG("is find? No \r\n")
         f.remove(it.c_str(),FileType::Directory);
         LOG("after remove:   ")
         if(f.find(it.c_str(),FileType::Directory))
         LOG("is find?:true \r\n")
         else LOG("is find? No \r\n")      
    }
    const char *word="last";
    f.mkdir(word);
    if(f.find(word,FileType::Directory)){
        LOG("find %s :true \r\n",word)
    }else{
       LOG("find %s :false \r\n",word) 
    }
     f.remove(word,FileType::Directory);
     LOG("after remove:   ")
     if(f.find(word,FileType::Directory))
    LOG("is find?:true \r\n",word)
    return 0;
    CLOSE_LOG(LOGFILE)
}