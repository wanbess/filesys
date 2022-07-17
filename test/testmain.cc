#include<iostream>
#include<string>
#include "../src/FileManeger.h"
int main(){
   FileManeger f;
   std::cout<<sizeof(unsigned long)<<std::endl;
    f.mkdir("aaa");
    f.mkdir("mmt");
    std::vector<std::string> vc;
    for(int i=0;i<100;++i){
        std::string s("str");
        s+= std::to_string(i);
        vc.push_back(s);
        f.mkdir(s.c_str());
    }
    for(auto &it:vc){
        std::cout<<f.find(it.c_str(),FileType::Directory)<<std::endl;
    }

}