/* TapeVM/Base.cpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#include <NoctSys/Scripting/TapeVM.hpp>
#include <NoctSys/Exception/TapeError.hpp>

#include <cassert>
#include <cmath>
#include <cstring>
#include <memory>

namespace noct {
  void TapeVM::loadTapeBase() {
    loadCompilerPrimitives();
    loadStackOperators();
    loadControlStructures();
    loadVariableDefiners();
    loadParsingWords();

    addWord("words", [=](TapeVM&){
      for (auto word : m_dict) 
        std::fprintf(stderr, "%s %s\n\n", word.first.c_str(), word.second.semantics.c_str());
    });

    addWord("dict", findWord("words")->code[0].func);

    /////////////////////////////////
    // Module Loading              //
    /////////////////////////////////

    addWord("INCLUDE", [=](TapeVM&){
      std::string modname = getNext();

      for (const auto& pathFmt : m_includeDirectories) {
        std::filesystem::path path(convertDirectory(pathFmt, convertModname(modname)));

        if (std::filesystem::exists(path)) {
          input().push(new FileInputSource(path));
          return;
        }
      } 

      throw TapeError("Module Not Found", modname);
    });
  }
}