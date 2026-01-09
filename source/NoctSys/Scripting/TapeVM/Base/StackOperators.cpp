/* TapeVM/Base/StackOperators.cpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#include <NoctSys/Scripting/TapeVM.hpp>
#include <NoctSys/Exception/TapeError.hpp>

#include <cassert>
#include <cmath>
#include <cstring>


namespace noct {
  void TapeVM::loadStackOperators() {
    addWord("+", [=](TapeVM&){
      if (stackSize() >= 2) {
        auto a = pop(),
             b = pop();
        push(a+b);
      }
      else throw TapeError("Stack Underflow", "+");
    });

    addWord("-", [=](TapeVM&){
      if (stackSize() >= 2) {
        auto a = pop(),
             b = pop();
        push(b-a);
      }
      else throw TapeError("Stack Underflow", "-");
    });

    addWord("/", [=](TapeVM&){
      if (stackSize() >= 2) {
        auto a = pop(),
             b = pop();
        push(b/a);
      }
      else throw TapeError("Stack Underflow", "/");
    });

    addWord("*", [=](TapeVM&){
      if (stackSize() >= 2) {
        auto a = pop(),
             b = pop();
        push(a*b);
      }

      else throw TapeError("Stack Underflow", "*");
    });

    addWord("%", [=](TapeVM&){
      if (stackSize() >= 2) {
        auto a = pop(),
             b = pop();
        push(b%a);
      }

      else throw TapeError("Stack Underflow", "+");
    });

    addWord("swap", [=](TapeVM&){
      if (stackSize() >= 2)
        std::swap(top(), at(stackSize()-2));
        
      else throw TapeError("Stack Underflow", "swap");
    });

    addWord("dup", [=](TapeVM&){
      if (stackSize())
        push(top());
        
      else throw TapeError("Stack Underflow", "dup");
    });

    addWord("drop", [=](TapeVM&){
      if (stackSize())
        pop();
        
      else throw TapeError("Stack Underflow", "drop");
    });

    addWord("over", [=](TapeVM&){
      if (stackSize() >= 2) {
        push(at(stackSize()-2));
      }
      else throw TapeError("Stack Underflow", "over");
    });

    addWord("rot", [=](TapeVM&){
      if (stackSize() >= 3) {
        auto a = top();
        auto b = stackSize() - 2,
             c = stackSize() - 3;
        top()  = at(b);
        at(b)  = at(c);
        at(c)  = a;

      }
      else throw TapeError("Stack Underflow", "rot");
    });

    addWord("=", [=](TapeVM&){
      if (stackSize() >= 2) {
        auto a = pop(),
             b = pop();
        push(a == b);
      }
    });

    addWord("<", [=](TapeVM&){
      if (stackSize() >= 2) {
        auto a = pop(),
             b = pop();
        push(a<b);
      }
    });

    addWord(">", [=](TapeVM&){
      if (stackSize() >= 2) {
        auto a = pop(),
             b = pop();
        push(a>b);
      }
    });

    addWord("<=", [=](TapeVM&){
      if (stackSize() >= 2) {
        auto a = pop(),
             b = pop();
        push(a<=b);
      }
    });

    addWord(">=", [=](TapeVM&){
      if (stackSize() >= 2) {
        auto a = pop(),
             b = pop();
        push(a>=b);
      }
    });

    addWord("<>", [=](TapeVM&){
      if (stackSize() >= 2) {
        auto a = pop(),
             b = pop();
        push(a<b || a>b);
      }
    });

    addWord("|", [=](TapeVM&){
      if (stackSize() >= 2) {
        auto a = pop(),
             b = pop();
        push(a|b);
      }
    });

    addWord("&", [=](TapeVM&){
      if (stackSize() >= 2) {
        auto a = pop(),
             b = pop();
        push(a&b);
      }
    });

    addWord("i>f", [=](TapeVM&){
      if (stackSize())
        fpush((float)pop());

      else throw TapeError("Stack Underflow", "i>f");
    });

    addWord("f+", [=](TapeVM&){
      if (fstackSize() >= 2) {
        auto a = fpop(),
             b = fpop();
        fpush(a+b);
      }
      else throw TapeError("Stack Underflow", "f+");
    });

    addWord("f-", [=](TapeVM&){
      if (fstackSize() >= 2) {
        auto a = fpop(),
             b = fpop();
        fpush(b-a);
      }
      else throw TapeError("Stack Underflow", "f-");
    });

    addWord("f/", [=](TapeVM&){
      if (fstackSize() >= 2) {
        auto a = fpop(),
             b = fpop();
        fpush(b/a);
      }
      else throw TapeError("Stack Underflow", "f/");
    });

    addWord("f*", [=](TapeVM&){
      if (fstackSize() >= 2) {
        auto a = fpop(),
             b = fpop();
        fpush(a*b);
      }

      else throw TapeError("Stack Underflow", "f*");
    });

    addWord("f%", [=](TapeVM&){
      if (fstackSize() >= 2) {
        auto a = fpop(),
             b = fpop();
        fpush(std::fmod(b, a));
      }

      else throw TapeError("Stack Underflow", "f+");
    });

    addWord("fswap", [=](TapeVM&){
      if (fstackSize() >= 2)
        std::swap(ftop(), fat(fstackSize()-2));
        
      else throw TapeError("Stack Underflow", "fswap");
    });

    addWord("fdup", [=](TapeVM&){
      if (stackSize())
        fpush(ftop());
        
      else throw TapeError("Stack Underflow", "fdup");
    });

    addWord("fdrop", [=](TapeVM&){
      if (fstackSize())
        fpop();
        
      else throw TapeError("Stack Underflow", "fdrop");
    });

    addWord("fover", [=](TapeVM&){
      if (fstackSize() >= 2) {
        fpush(fat(fstackSize()-2));
      }
      else throw TapeError("Stack Underflow", "fover");
    });

    addWord("frot", [=](TapeVM&){
      if (fstackSize() >= 3) {
        auto a = ftop();
        auto b = fstackSize() - 2,
             c = fstackSize() - 3;
        ftop() = fat(b);
        fat(b) = fat(c);
        fat(c) = a;

      }
      else throw TapeError("Stack Underflow", "frot");
    });

    addWord("f>i", [=](TapeVM&){
      if (fstackSize())
        push((std::uintptr_t)fpop());
      
      else throw TapeError("Stack Underflow", "f>i");
    });

    addWord(">R", [=](TapeVM&){
      if (stackSize())
        rpush(pop());

      else throw TapeError("Stack Underflow", ">R");
    });

    addWord("R@", [=](TapeVM&){
      if (rstackSize())
        push(rtop());
      
      else throw TapeError("Stack Underflow", "R@");
    });

    addWord("R>", [=](TapeVM&){
      if (rstackSize())
        push(rpop());

      else throw TapeError("Stack Underflow", "R>");
    });
  }
}