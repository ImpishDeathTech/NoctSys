/* TapeVM/Base/Variables.cpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#include <NoctSys/Scripting/TapeVM.hpp>
#include <NoctSys/Exception/TapeError.hpp>

#include <cassert>
#include <cmath>
#include <cstring>


namespace noct {
  void TapeVM::loadVariableDefiners() {
    addWord("VARIABLE", [=](TapeVM&){
      std::string name = getNext();
      auto        data = alloc(sizeof(std::uintptr_t));

      addWord(name, findWord("(LIT)")->code[0].func, data);
      findMem(data)->pinned = true;
    });

    addWord("CREATE", [=](TapeVM&){
      std::string name = getNext();
      addWord(name, findWord("(LIT)")->code[0].func, 0ul);
      setAllocating(true);
    });

    addWord("ALLOT", [=](TapeVM&){
      if (stackSize()) {
        auto sz = pop();
        auto p  = allot(sz);
        push(p);
      }
      else throw TapeError("Stack Underflow", "ALLOT");
    });

    addWord("ALLOC", [=](TapeVM&){
      if (isAllocating()) {
        if (stackSize()) {
          auto sz = pop();
          compileInline(getLastDefinition(), findWord("(END)")->code[0].func, alloc(sz));
          setAllocating(false);
        }
        else throw TapeError("Stack Underflow", "ALLOC");
      }
      else if (stackSize()) {
        auto sz = pop();
        auto p  = alloc(sz);
        push(p);
      }
      else throw TapeError("Stack Underflow", "alloc");
    });

    addWord("free", [=](TapeVM&){
      if (stackSize()) {
        auto p = pop();

        if (!findMem(p))
          throw TapeError("Not a valid adress", std::to_string(p));
        
        freeMem(p);
      }
    });

    addWord("@", [=](TapeVM&){
      if (stackSize()) {
        auto addr = pop();
        push(*(std::uintptr_t*)addr);
      }
      else throw TapeError("Stack Underflow", "@");
    });

    addWord("!", [=](TapeVM&){
      if (stackSize() >= 2) {
        auto  a = pop(),
              b = pop();
        auto* c = reinterpret_cast<std::uintptr_t*>(a);
        *c = b;
      }
    });

    addWord("f@", [=](TapeVM&){
      if (stackSize()) {
        auto addr = pop();
        fpush(*reinterpret_cast<float*>(addr));
      }
      else throw TapeError("Stack Underflow", "f@");
    });

    addWord("f!", [=](TapeVM&){
      if (stackSize() && fstackSize()) {
        auto   a = pop();
        float  b = fpop(),
              *c = reinterpret_cast<float*>(a);
        *c = b;
      }
      else throw TapeError("Stack Underflow", "f!");
    });

    addWord("CONSTANT", [=](TapeVM&){
      if (stackSize()) {
        std::string name = getNext();
        auto        data = pop();
        addWord(name, findWord("(LIT)")->code[0].func, data);
      }
      else throw TapeError("Stack Underflow", "CONSTANT");
    });

    addWord("SCONSTANT", [=](TapeVM&){
      if (stackSize()) {
        std::string name = getNext();
        auto        size = pop(),
                    data = pop();

        if (isScratchData(data)) {
          auto *scratchBytes = reinterpret_cast<std::uint8_t*>(data),
               *heapBytes    = (std::uint8_t*)alloc(sizeof scratchBytes);

          for (auto i = 0ul; i < size; i++)
            heapBytes[i] = scratchBytes[i];

          data = reinterpret_cast<std::uintptr_t>(heapBytes);
          findMem(data)->pinned = true;
        
        } else if (auto p = findMem(data))
          p->pinned = true;
        
        addWord(name, findWord("(LIT)")->code[0].func, data);
      }
      else throw TapeError("Stack Underflow", "CONSTANT");
    });

    addWord("FCONSTANT", [=](TapeVM&){
      if (fstackSize()) {
        std::string name = getNext();
        float* data = (float*)alloc(sizeof(float));
        *data       = fpop();

        addWord(name, findWord("(FLIT)")->code[0].func, (std::uintptr_t)data);
        findMem((std::uintptr_t)data)->pinned = true;
      }
      else throw TapeError("Stack Underflow", "CONSTANT");
    });
  }
}