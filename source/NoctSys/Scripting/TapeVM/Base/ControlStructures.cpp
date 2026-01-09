/* TapeVM/Base/ControlStructures.cpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#include <NoctSys/Scripting/TapeVM.hpp>
#include <NoctSys/Exception/TapeError.hpp>

#include <cassert>
#include <cmath>
#include <cstring>

namespace noct {
  void TapeVM::loadControlStructures() {
    addWord("EXIT", [=](TapeVM&){
      switch (getInputMode()) {
        case TapeVM::InputMode::Compiling:
          if (m_cstack.back().type == TapeVM::ControlFrame::DO)
            compileInline(getLastDefinition(), findWord("UNLOOP")->code[0].func);
           compileInline(getLastDefinition(), findWord("(END)")->code[0].func);
          break;
        
        default:
        {
          auto& xtoken = m_exec.back();
          xtoken.ip = xtoken.word->size();
        }
      }
    });

    setImmediate("EXIT");

    addWord("LEAVE", [=](TapeVM&){
      if (getInputMode() != TapeVM::InputMode::Compiling)
        throw TapeError("Compile Only Word", "LEAVE");

      for (auto it = m_cstack.rbegin(); it != m_cstack.rend(); ++it) {
        if (it->type == TapeVM::ControlFrame::BEGIN
        or  it->type == TapeVM::ControlFrame::WHILE
        or  it->type == TapeVM::ControlFrame::DO) {
          auto*       w        = findWord(getLastDefinition());
          
          if (it->type == TapeVM::ControlFrame::DO)
            compileInline(getLastDefinition(), findWord("UNLOOP")->code[0].func);
          
          std::size_t patch_ip = w->code.size();

          compileInline(getLastDefinition(), findWord("(JMP)")->code[0].func, 0ul);

          it->leave_patches.push_back(patch_ip);
          return;
        }
      }
      throw TapeError("LEAVE outside of loop", "LEAVE");
    });

    setImmediate("LEAVE");

    addWord("IF", [=](TapeVM&){
      if (getInputMode() != TapeVM::InputMode::Compiling)
        throw TapeError("Compile Only Word", "IF");

      auto* w = findWord(getLastDefinition());

      std::size_t ip = w->code.size();

      w->code.push_back({findWord("(0JMP)")->code[0].func, 0ul});
      cpush({TapeVM::ControlFrame::IF, ip});
    });

    setImmediate("IF");

    addWord("ELSE", [=](TapeVM&){
      if (getInputMode() != TapeVM::InputMode::Compiling)
        throw TapeError("Compile Only Word", "ELSE");

      else if (cstack_empty() || ctop().type != TapeVM::ControlFrame::IF)
        throw TapeError("ELSE without IF", "ELSE");
      
      auto* w        = findWord(getLastDefinition());
      auto  if_frame = cpop();
      
      std::size_t else_ip = w->code.size();
      w->code[if_frame.patch_ip].data = else_ip - if_frame.patch_ip - 1;

      std::size_t jmp_ip = w->code.size();
      compileInline(getLastDefinition(), findWord("(JMP)")->code[0].func, 0ul);

      cpush({ TapeVM::ControlFrame::ELSE, jmp_ip });
    });

    setImmediate("ELSE");

    addWord("THEN", [=](TapeVM&){
      if (getInputMode() != TapeVM::InputMode::Compiling) 
        throw TapeError("Compile Only Word", "THEN");
      
      else if (cstack_empty() 
      or      (ctop().type != TapeVM::ControlFrame::IF && ctop().type) != TapeVM::ControlFrame::ELSE)
        throw TapeError("THEN without IF or ELSE", "THEN");

      auto* w     = findWord(getLastDefinition());
      auto  frame = cpop();

      std::size_t then_ip = w->code.size();
      w->code[frame.patch_ip].data = then_ip - frame.patch_ip - 1;
    });

    setImmediate("THEN");

    addWord("BEGIN", [=](TapeVM&){
      if (getInputMode() != TapeVM::InputMode::Compiling)
        throw TapeError("Compile Only Word", "BEGIN");

      auto* w = findWord(getLastDefinition());
      cpush({TapeVM::ControlFrame::BEGIN, w->code.size()});
    });

    setImmediate("BEGIN");

    addWord("AGAIN", [=](TapeVM&){
      if (getInputMode() != TapeVM::InputMode::Compiling)
        throw TapeError("Compile Only Word", "AGAIN");

      else if (cstack_empty() || ctop().type != TapeVM::ControlFrame::BEGIN)
        throw TapeError("AGAIN without BEGIN", "AGAIN");

      auto* w     = findWord(getLastDefinition());
      auto  frame = cpop();
      
      std::size_t here_ip = w->code.size();
      auto         offset  = -static_cast<std::intptr_t>(here_ip - frame.patch_ip + 1);

      compileInline(getLastDefinition(), findWord("(JMP)")->code[0].func, offset);

      std::size_t exit_ip = w->code.size();

      for (auto ip : frame.leave_patches) 
        w->code[ip].data = exit_ip - ip - 1;
    });

    setImmediate("AGAIN");

    addWord("UNTIL", [=](TapeVM&){
      if (getInputMode() != TapeVM::InputMode::Compiling)
        throw TapeError("Compile Only Word", "UNTIL");

      else if (cstack_empty() || ctop().type != TapeVM::ControlFrame::BEGIN)
        throw TapeError("UNTIL without BEGIN", "UNTIL");

      auto* w     = findWord(getLastDefinition());
      auto  frame = cpop();
      
      auto offset = -static_cast<std::intptr_t>(w->code.size() - frame.patch_ip + 1);

      compileInline(getLastDefinition(), findWord("(0JMP)")->code[0].func, offset);
      
      std::size_t exit_ip = w->code.size();

      for (auto ip : frame.leave_patches) 
        w->code[ip].data = exit_ip - ip - 1;
    });

    setImmediate("UNTIL");

    addWord("WHILE", [=](TapeVM&){
      if (getInputMode() != TapeVM::InputMode::Compiling)
        throw TapeError("Compile Only Word", "WHILE");

      else if (cstack_empty() || ctop().type != TapeVM::ControlFrame::BEGIN)
        throw TapeError("WHILE without BEGIN", "WHILE");

      auto*       w  = findWord(getLastDefinition());
      std::size_t ip = w->code.size();

      compileInline(getLastDefinition(), findWord("(0JMP)")->code[0].func, 0ul);
      cpush({TapeVM::ControlFrame::WHILE, ip});
    });

    setImmediate("WHILE");

    addWord("REPEAT", [=](TapeVM&){
      if (getInputMode() != TapeVM::InputMode::Compiling)
        throw TapeError("Compile Only Word", "REPEAT");

      else if (cstack_empty())
        throw TapeError("Control Stack is empty", "REPEAT");

      auto* w          = findWord(getLastDefinition());
      auto  whileFrame = cpop();
      auto  beginFrame = cpop();

      if (whileFrame.type != TapeVM::ControlFrame::WHILE
      or (beginFrame.type != TapeVM::ControlFrame::BEGIN))
        throw TapeError("Malformed BEGIN WHILE REPEAT", "REPEAT");
      
      std::size_t here_ip = w->code.size();
      auto        offset  = -(std::intptr_t)(w->code.size() - beginFrame.patch_ip + 1);

      compileInline(getLastDefinition(), findWord("(JMP)")->code[0].func, offset);

      std::size_t exit_ip = w->code.size();

      w->code[whileFrame.patch_ip].data = exit_ip - whileFrame.patch_ip - 1;

      for (auto ip : whileFrame.leave_patches)
        w->code[ip].data = exit_ip - ip - 1;
      
      for (auto ip : beginFrame.leave_patches)
        w->code[ip].data = exit_ip - ip - 1;
    });

    setImmediate("REPEAT");

    addWord("I", [=](TapeVM&){
      if (rstackSize() >= 2) 
        push(rtop());

      else throw TapeError("Stack Underflow: return stack (< 2)", "I");
    });

    addWord("J", [=](TapeVM&){
      if (rstackSize() >= 4) 
        push(rat(rstackSize() - 3));

      else throw TapeError("Stack Underflow: return stack (< 4)", "J");
    });

    addWord("DO", [=](TapeVM&){
      if (getInputMode() != TapeVM::InputMode::Compiling)
        throw TapeError("Compile Only Word", "DO");
      
      auto*       w        = findWord(getLastDefinition());
      std::size_t start_ip = w->code.size();

      compileInline(getLastDefinition(), findWord("(DO)")->code[0].func);
      cpush({TapeVM::ControlFrame::DO, start_ip});
    });

    setImmediate("DO");

    addWord("LOOP", [=](TapeVM&){
      if (getInputMode() != TapeVM::InputMode::Compiling)
        throw TapeError("Compile Only Word", "LOOP");

      if (cstack_empty())
        throw TapeError("LOOP without DO", "LOOP");

      auto frame = cpop();

      if (frame.type != TapeVM::ControlFrame::DO)
        throw TapeError("LOOP without DO", "LOOP");

      auto*       w       = findWord(getLastDefinition());
      std::size_t here_ip = w->code.size();
      auto        offset  = -static_cast<std::intptr_t>(here_ip - frame.patch_ip + 1);

      compileInline(getLastDefinition(), findWord("(LOOP)")->code[0].func, offset);

      std::size_t exit_ip = w->code.size();

      for (auto ip : frame.leave_patches)
        w->code[ip].data = exit_ip - ip - 1;
    }); 

    setImmediate("LOOP");

    addWord("+LOOP", [=](TapeVM&){
      if (getInputMode() != TapeVM::InputMode::Compiling)
        throw TapeError("Compile Only Word", "+LOOP");

      if (cstack_empty())
        throw TapeError("+LOOP without DO", "+LOOP");

      auto frame = cpop();

      if (frame.type != TapeVM::ControlFrame::DO)
        throw TapeError("+LOOP without DO", "+LOOP");

      auto*       w       = findWord(getLastDefinition());
      std::size_t here_ip = w->code.size();
      auto        offset  = -static_cast<std::intptr_t>(here_ip - frame.patch_ip + 1);

      compileInline(getLastDefinition(), findWord("(+LOOP)")->code[0].func, offset);

      std::size_t exit_ip = w->code.size();

      for (auto ip : frame.leave_patches)
        w->code[ip].data = exit_ip - ip - 1;
    });

    setImmediate("+LOOP");

    addWord("UNLOOP", [=](TapeVM&){
      if (rstackSize() < 2)
        throw TapeError("Stack Underflow: return stack (< 2)", "I");

      rpop();
      rpop();
    });
  }
}