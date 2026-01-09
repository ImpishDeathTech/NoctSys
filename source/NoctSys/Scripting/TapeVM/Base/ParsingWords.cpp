/* TapeVM/Base/ParsingWords.cpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#include <NoctSys/Scripting/TapeVM.hpp>
#include <NoctSys/Exception/TapeError.hpp>

#include <cassert>
#include <cmath>
#include <cstring>

namespace noct {
  void TapeVM::loadParsingWords() {
    addWord("\\", [=](TapeVM&){
      for (auto ch = input().get(); ch != '\n'; ch = input().get());
    });

    setImmediate("\\");

    addWord("(", [=](TapeVM&){
      switch (getInputMode()) {
        case TapeVM::InputMode::Compiling:
        {
          auto& word = *findWord(getLastDefinition());

          for (auto ch = input().get(); ch != ')'; ch = input().get())
            word.semantics += ch;

          word.semantics += ')';
        } break;
        default:
          for (auto ch = input().get(); ch != ')'; ch = input().get());
      }
    });

    setImmediate("(");

    addWord("(*", [=](TapeVM&){
      for (auto word = getNext(); word != "*)"; word = getNext());
    });

    setImmediate("(*");

    addWord("(**", [=](TapeVM&){
      switch (getInputMode()) {
        case TapeVM::InputMode::Compiling:
        {
          auto&       word = *findWord(getLastDefinition());
          std::string tmp;

          word.semantics += "(**\n";

          for (auto ch = input().get();; ch = input().get()) {
            if (std::isspace(ch)) {
              word.semantics += (tmp += ch);
              
              if (tmp == "*)") {
                word.semantics += (tmp += ch);
                break;
              }

              tmp.clear();
            }
            else tmp += ch;
          }
        } break;

        default:
          for (auto word = getNext(); word != "*)"; word = getNext());
      }
    });

    setImmediate("(**");

    addWord("s\"", [=](TapeVM&){
      if (getInputMode() == TapeVM::InputMode::Interpreting) {
        std::string str;
        char*       cstr = nullptr;

        for (auto ch = input().get(); ch != '"'; ch = input().get())
          str += ch;

        auto  data = allot(str.length());
        char* cstr = reinterpret_cast<char*>(data);

        std::strncpy(cstr, str.c_str(), str.length());

        push(data);
        push(str.length());
      } 
      else throw TapeError("Interpret Only Word: use 'c\"' when compiling", "s\"");
    });

    setImmediate("s\"");

    addWord("c\"", [=](TapeVM&){
      if (getInputMode()== TapeVM::InputMode::Compiling) {
        std::string str;
        char*       cstr = nullptr;

        for (auto ch = input().get(); ch != '"'; ch = input().get())
          str += ch;

        auto  data = alloc(str.length());
        char* cstr = reinterpret_cast<char*>(data);

        std::strncpy(cstr, str.c_str(), str.length());

        auto& lit = findWord("(LIT)")->code[0].func;

        compileInline(getLastDefinition(), lit, data);
        compileInline(getLastDefinition(), lit, str.length());
        findMem(data)->pinned = true;
      }
      else throw TapeError("Compile Only Word", "c\"");
    });

    setImmediate("c\"");

    addWord("parse", [=](TapeVM&){
      if (stackSize()) {
        char        delim = char(pop() % CHAR_MAX);
        std::string str;

        for (auto ch = input().get(); ch != delim; ch = input().get())
          str += ch;
        
        auto  data = allot(str.length());
        char* buf  = reinterpret_cast<char*>(data);

        for (auto i = 0ul; i < str.length(); i++)
          buf[i] = str[i];

        push(data);
        push(str.length());
      }
      else throw TapeError("Stack Underflow", "parse");
    });

    addWord("parse-name", [=](TapeVM&){
      std::string name   = getNext();
      auto        data   = allot(name.length());
      char*       buffer = reinterpret_cast<char*>(data);

      for (auto i = 0; i < name.length(); i++) 
        buffer[i] = name[i];
      
      push(data);
      push(name.length());
    });

    addWord("CHAR", [=](TapeVM&){
      std::string name = getNext();
      push(name[0]);
    });

    addWord("'", [=](TapeVM&){
      std::string name = getNext();
      auto* xtoken = findWord(name);

      if (xtoken)
        push(reinterpret_cast<std::uintptr_t>(xtoken));

      else throw TapeError("Unknown Word", name);
    });

    
  }
}