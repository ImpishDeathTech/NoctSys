/* TapeVM/Base/CompilerPrimitives.cpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#include <NoctSys/Scripting/TapeVM.hpp>
#include <NoctSys/Exception/TapeError.hpp>

#include <cassert>
#include <cmath>
#include <cstring>

namespace noct {
  void TapeVM::loadCompilerPrimitives() {
    addWord(":", [=](TapeVM&){
      std::string name = getNext();
      addWord(name);
      setInputMode(TapeVM::InputMode::Compiling);
    });

    setImmediate(":");

    addWord(";", [=](TapeVM&){
      if (getInputMode() != TapeVM::InputMode::Compiling)
        throw TapeError("Compile Only Word", ";");

      if (!cstack_empty())
        throw TapeError("Unclosed control structure", getLastDefinition());

      compileInline(getLastDefinition(), findWord("(END)")->code[0].func);
      resetScratchArena(TapeVM::ScratchReset::Definition);
      setInputMode(TapeVM::InputMode::Interpreting);
    });

    setImmediate(";");

    addWord("[", [=](TapeVM&){
      if (getInputMode() == TapeVM::InputMode::Compiling)
        setInputMode(TapeVM::InputMode::Interpreting);

      else throw TapeError("Compile Only Word", "[");
    });

    addWord("(END)", [=](TapeVM&){
      switch (getInputMode()) {
        case TapeVM::InputMode::Executing:
          m_exec.pop_back();
          break;

        case TapeVM::InputMode::Compiling:
          if (!cstack_empty()) 
            throw TapeError("Unclosed control structure", getLastDefinition());

          else {
            compileInline(getLastDefinition(), findWord("(END)")->code[0].func);
            resetScratchArena(TapeVM::ScratchReset::Definition);
            setInputMode(TapeVM::InputMode::Interpreting);
          }
          break;
          
        default:
          throw TapeError("Compile Only Word", "(END)");
      }
    });
    
    setImmediate("(END)");

    addWord("(LIT)", [=](TapeVM&){
      switch (getInputMode()) {
        case TapeVM::InputMode::Executing:
        {
          auto& token = getExecuting();
          push(token.word->at(token.ip).data);
        } break;
        
        case TapeVM::InputMode::Compiling:
        {
          std::string number = getNext();
          if (isInteger(number)) 
            compileInline(getLastDefinition(), findWord("(LIT)")->code[0].func, toInteger(number));
          
          else throw TapeError("Not An Integral Number", number);
        } 
        break;

        default:
          throw TapeError("Compile Only Word", "(LIT)");
      }
    });

    setImmediate("(LIT)");

    addWord("(FLIT)", [=](TapeVM&){
      switch (getInputMode()) {
        case TapeVM::InputMode::Executing:
        {
          auto& token = getExecuting();
          fpush(*reinterpret_cast<float*>(token.word->at(token.ip).data));
        } break;
        
        case TapeVM::InputMode::Compiling:
        {
          std::string number = getNext();
          if (isRealnum(number)) {
            float* f = (float*)alloc(sizeof(float));
            *f = toRealnum(number);
            compileInline(getLastDefinition(), findWord("(FLIT)")->code[0].func, (std::uintptr_t)f);
            setPinned((std::uintptr_t)f);
          }
          else throw TapeError("Not A Real Number", number);
        } break;

        default:
          throw TapeError("Compile Only Word", "(FLIT)");
      }
    });

    setImmediate("(FLIT)");

    addWord("(JMP)", [=](TapeVM&){
      switch (getInputMode()) {
        case TapeVM::InputMode::Executing:
        {
          auto& token    = getExecuting();
          auto  offset   = static_cast<std::intptr_t>(token.word->at(token.ip).data),
                next     = static_cast<std::intptr_t>(token.ip + offset),
                codeSize = static_cast<std::intptr_t>(token.word->size());

          assert(next >= 0 && next < codeSize);
          jump(offset);
        } break;

        case TapeVM::InputMode::Compiling:
        {
          std::string number = getNext();

          if (isInteger(number))
            compileInline(getLastDefinition(), findWord("(JMP)")->code[0].func, toInteger(number));

          else throw TapeError("Not An Integral Number", number);
        } break;

        default:
          throw TapeError("Compile Only Word", "(JMP)");
      }
    });

    setImmediate("(JMP)");

    addWord("(0JMP)", [=](TapeVM&){
      switch (getInputMode()) {
        case TapeVM::InputMode::Executing:
          if (stackSize()) {
            bool flag = pop();

            if (!flag) {
              auto& token    = getExecuting();
              auto  offset   = static_cast<std::intptr_t>(token.word->at(token.ip).data),
                    next     = static_cast<std::intptr_t>(token.ip + offset),
                    codeSize = static_cast<std::intptr_t>(token.word->size());

              assert(next >= 0 && next < codeSize);
              jump(offset);
            }
          } else throw TapeError("Stack Underflow", "(0JMP)");
          break;

        case TapeVM::InputMode::Compiling:
        {
          std::string number = getNext();

          if (isInteger(number))
            compileInline(getLastDefinition(), findWord("(0JMP)")->code[0].func, toInteger(number));

          else throw TapeError("Not An Integral Number", number);
        } break;
        default:
          throw TapeError("Compile Only Word", "(0JMP)");
      }
    });

    setImmediate("(0JMP)");

    addWord("(BRANCH)", [=](TapeVM&){
      switch (getInputMode()) {
        
        case TapeVM::InputMode::Executing:
        {
          auto& xtoken = getExecuting();
          auto* target = reinterpret_cast<WordTag*>(xtoken.word->at(xtoken.ip).data);
          xpush(target->code);
        } break;
        case TapeVM::InputMode::Compiling:
        {
          std::string name = getNext();
          compileReference(getLastDefinition(), name);
        } break;
        default:
          throw TapeError("Compile Only Word", "(BRANCH)");
      }
    });

    setImmediate("(BRANCH)");

    addWord("(DO)", [=](TapeVM&){
      if (stackSize() >= 2) {
        auto start = pop(),
             limit = pop();
        
        rpush(limit);
        rpush(start);
      }
      else throw TapeError("Stack Underflow", "(DO)");
    });

    addWord("(LOOP)", [=](TapeVM&){
      if (rstackSize() >= 2) {
        auto& index = rtop();
        auto  limit = rat(rstackSize() - 2);

        index++;

        if (index != limit) {
          auto& xtoken = getExecuting();
          jump(xtoken.word->at(xtoken.ip).data);
        }
        else {
          rpop();
          rpop();
        }
      }
      else throw TapeError("Stack Underflow", "(LOOP)");
    });

    addWord("(+LOOP)", [=](TapeVM&){
      if (rstackSize() < 2)
        throw TapeError("Return stack underflow", "+LOOP");

      auto  inc    = static_cast<std::intptr_t>(pop());
      auto &index  = rtop(),
            limit  = rat(rstackSize()),
            next   = index + inc;
      bool  isExit = (inc > 0 && next >= limit) || (inc < 0 && next <= limit);

      index = next;

      if (!isExit) {
        auto& xtoken = getExecuting();
        jump(xtoken.word->at(xtoken.ip).data);
      }
      else {
        rpop();
        rpop();
      }
    });

    addWord("IMMEDIATE", [=](TapeVM&){
      auto* w = findWord(getLastDefinition());

      if (!w)
        throw TapeError("No word to mark", "IMMEDIATE");
      
      w->immediate = true;
    });

    setImmediate("IMMEDIATE");

    addWord("POSTPONE", [=](TapeVM&){
      if (getInputMode()!= TapeVM::InputMode::Compiling)
        throw TapeError("Compile Only Word", "POSTPONE");

      std::string word = getNext();
      compileReference(getLastDefinition(), word);
    });

    setImmediate("POSTPONE");

    addWord("[CHAR]", [=](TapeVM&){
      switch (getInputMode()) {
        case TapeVM::InputMode::Executing:
        {
          auto& xtoken = getExecuting();
          auto  ch     = static_cast<char>(xtoken.word->at(xtoken.ip).data);

          push(ch);
        } break;

        case TapeVM::InputMode::Compiling:
        {
          auto parsed = getNext();
          compileInline(getLastDefinition(), findWord("[CHAR]")->code[0].func, parsed[0]);
        } break;
        
        default:
          throw TapeError("Compile Only Word", "[CHAR]");
      }
    });

    setImmediate("[CHAR]");

    addWord("[']", [=](TapeVM&){
      if (getInputMode() == TapeVM::InputMode::Compiling) {
        std::string name   = getNext();
        auto*       xtoken = findWord(name);
        
        if (xtoken)
          compileInline(getLastDefinition(), findWord("LIT")->code[0].func, reinterpret_cast<std::uintptr_t>(xtoken));
        else throw TapeError("Unknown Word", name);
      } 
      else throw TapeError("Compile Only Word", "[']");
    });

    setImmediate("[']");
  }
}