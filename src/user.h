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
struct Command{
   BaseCommand  suf; 
   std::vector<std::string> para;
   std::vector<std::string> rpath;
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
  
  Mode auth;
  Inode* now_node;
  FileManeger* maneger;
  std::vector<std::pair< std::unique_ptr<Inode>,std::string>> inode_st;
  std::vector<Command> instrcts;
  bool connect;
};