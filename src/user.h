#pragma once
#include"FileManeger.h"
#include<vector>
#include<string>
enum class BaseCommand{
    CD,
    MKDIR,
    LINK,
    RM,
    CP
};
enum class ArgType{
    _P,
    _R,
    _P,
};
struct Command{
   BaseCommand  suf; 
   std::vector<ArgType> para;
   struct Compath{
       std::vector<std::string> rpath;
       bool root_b;
   }compath;
};
class User{
public:
  User();
  User( User &&);
  User( FileManeger& manger);
  User& operator=(User &&);
  User& operator=(const User&);
  ~User();
  int bind(const FileManeger& manger);
  void command(const char* comman);
  void disconnect();
  void parse(const char* comman);
  void pathparse();
  bool commandCd(Command& com);
  bool commandMkdir(Command& com);
  bool commandRm(Command& com);
  bool commandLink(Command& com);
private:
  std::unique_ptr<Inode>  findpath(Inode* root,std::vector<std::string> &path,bool recor);
  Mode auth;
  Inode* now_node;
  FileManeger* maneger;
  std::vector<std::pair< std::unique_ptr<Inode>,std::string>> inode_st;
  std::vector<Command> instrcts;
  bool connect;
};