/* TapeVM.cpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#include <NoctSys/Scripting/TapeVM.hpp>
#include <NoctSys/Exception/TapeError.hpp>
#include <cassert>
#include <cmath>

namespace noct {

  TapeVM::TapeVM() 
    : m_stack(), m_fstack(), m_dict(), m_exec(), m_mem(), m_mode(TapeVM::InputMode::Interpreting)
  {
#if defined(__NoctSys_Unix__) 
    m_includeDirectories = {
      "/usr/share/NoctSys/tape/?.tape",
      "/usr/share/NoctSys/tape/?.fth",
      "/usr/share/NoctSys/tape/?.4th",
      "/usr/share/NoctSys/tape/?.fs",
      "/usr/share/NoctSys/tape/?.f",
      "./tape/?.tape",
      "./tape/?.fth",
      "./tape/?.4th",
      "./tape/?.fs",
      "./tape/?.f",
      "./?.tape",
      "./?.fth",
      "./?.4th",
      "./?.fs",
      "./?.f"
    };
#elif defined(__NoctSys_Windows__)
    m_includeDirectories = { 
      "C:\\ProgramFiles\\NoctSys\\tape\\?.tape",
      "C:\\ProgramFiles\\NoctSys\\tape\\?.fth",
      "C:\\ProgramFiles\\NoctSys\\tape\\?.4th",
      "C:\\ProgramFiles\\NoctSys\\tape\\?.fs",
      "C:\\ProgramFiles\\NoctSys\\tape\\?.f",
      ".\\tape\\?.tape",
      ".\\tape\\?.fth",
      ".\\tape\\?.4th",
      ".\\tape\\?.fs",
      ".\\tape\\?.f",
      ".\\?.tape",
      ".\\?.fth",
      ".\\?.4th",
      ".\\?.fs",
      ".\\?.f"
    };
#endif 
  }

  TapeVM::~TapeVM() {
    clearStacks();

    for (auto& tag : m_mem) {
      if (tag.data) 
        std::free((char*)tag.data);
    }
  }

  void TapeVM::addIncludeDirectory(const std::string& directory) {
    m_includeDirectories.push_back(directory);
  }

  std::string TapeVM::convertModname(std::string modname) {
#if defined(__NoctSys_UNIX__)
    std::replace(modname.begin(), modname.end(), '.', '/');

#elif defined(__NoctSys_Windows__);
    std::replace(modname.begin(), modname.end(), '.', '\\');
#endif 
    return modname;
  }

  std::string TapeVM::convertDirectory(std::string directory, const std::string& modname) {
    static const std::string re  = "?";

    std::replace(directory.begin(), directory.end(), re, modname);
    return directory;
  }

  void TapeVM::setInputMode(TapeVM::InputMode mode) {
    m_mode = mode;
  }


  TapeVM::InputMode TapeVM::getInputMode() {
    return m_mode;
  }


  std::string_view TapeVM::getLastDefinition() {
    return m_lastDefinition;
  }


  TapeVM::WordTag* TapeVM::findWord(const std::string_view& word) {
    auto it = m_dict.find(std::string(word));
    if (it != m_dict.end())
      return &(it->second);

    return nullptr;
  }


  void TapeVM::addWord(const std::string_view& word) {
    auto        it = m_dict.find(word.data());
    std::string w{word};

    if (it != m_dict.end())
      m_dict[w].code.clear();
    
    else m_dict[w] = {};

    m_lastDefinition = std::string(word);
  }


  void TapeVM::addWord(const std::string_view& name, const TapeVM::Function& func, std::uintptr_t data) {
    addWord(name);
    auto* token = findWord(name);
    token->code.push_back({func, data});
  }


  void TapeVM::addWord(const std::string_view& name, const TapeVM::Word& token) {
    m_dict[std::string(name)] = { token };
  }


  void TapeVM::compileInline(const std::string_view& word, const Function& func, std::uintptr_t data) {
    auto* w = findWord(word);

    if (w)
      w->code.push_back({func, data});
  }


  void TapeVM::compileReference(const std::string_view& word, const std::string_view& token) {
    std::string tkn { token };

    auto* w = findWord(tkn);

    if (!w)
      throw TapeError("Unknown Word", tkn);

    compileInline(word, findWord("(BRANCH)")->code[0].func, reinterpret_cast<std::uintptr_t>(w));
  }


  void TapeVM::xpush(const Word& word) {
    m_exec.push_back({&word, 0ul});
  }


  TapeVM::XToken& TapeVM::getExecuting() {
    return m_exec.back();
  }


  void TapeVM::execute() {
    if (!m_exec.empty()) {
      auto lastMode = m_mode;
      m_mode = TapeVM::InputMode::Executing;

      do {
        auto& token = m_exec.back();

        if (token.ip >= token.word->size()) {
          m_exec.pop_back();
          continue;
        }
        else {
          token.word->at(token.ip).func(*this);
          token.ip++;
        }
      } while (!m_exec.empty());

      m_mode = lastMode;
    }
  }


  std::uintptr_t TapeVM::allot(std::size_t size) {
    if (m_smem.dp + size > m_smem.buffer.size())
      m_smem.buffer.resize(std::max(m_smem.buffer.size() * 2, m_smem.dp + size));
    
    auto addr = reinterpret_cast<std::uintptr_t>(m_smem.buffer.data() + m_smem.dp);

    m_smem.dp += size;
    return addr;
  }

  bool TapeVM::isScratchData(std::uintptr_t data) {
    for (auto dp = 0ul; dp < m_smem.buffer.size(); dp++) {
      if (std::uintptr_t(m_smem.buffer.data() + dp) == data)
        return true;
    }
    return false;
  }

  void TapeVM::resetScratchArena(TapeVM::ScratchReset reset) {
    switch (reset) {
      case TapeVM::ScratchReset::Line:
      case TapeVM::ScratchReset::Definition:
        m_smem.dp = 0ul;
    }
  }

  void TapeVM::reserveScratchArena(std::size_t reserve) {
    if (m_smem.buffer.empty())
      m_smem.buffer.reserve(reserve);
  }

  std::uintptr_t TapeVM::alloc(std::size_t size) {
    for (auto& tag : m_mem) {
      if (tag.free) {
        tag.size   = size;
        tag.data   = (std::uintptr_t)std::malloc(size);
        tag.free   = false;
        tag.pinned = false;

        return tag.data;
      }
    }

    m_mem.push_back({
      size,
      (std::uintptr_t)std::malloc(size),
      false,
      false
    });

    return m_mem.back().data;
  }


  std::uintptr_t TapeVM::realloc(std::uintptr_t data, std::size_t size) {
    auto tag = findMem(data);

    if (tag && !(tag->free)) {
      if (tag->pinned)
        throw TapeError("Cannot reallocate pinned data", std::to_string(data));

      tag->data = (std::uintptr_t)std::realloc((char*)(tag->data), size);
      tag->size = size;

      return tag->data;
    }

    return 0ul;
  }


  void TapeVM::freeMem(std::uintptr_t data) {
    auto* tag = findMem(data);
    if (!tag) return;

    else if (tag->pinned)
      throw TapeError("Cannot free pinned data", std::to_string(tag->data));

    std::free((char*)(tag->data));

    tag->data   = 0ul;
    tag->size   = 0ul;
    tag->free   = true;
  }



  TapeVM::MemTag* TapeVM::findMem(std::uintptr_t data) {
    for (auto& tag : m_mem) {
      if (tag.data == data) 
        return &tag;
    }

    return nullptr;
  }


  void TapeVM::setPinned(std::uintptr_t data, bool flag) {
    for (auto& tag : m_mem) {
      if (tag.data == data)
        tag.pinned = flag;
        return;
    }
  }

  void TapeVM::push(std::uintptr_t data) {
    m_stack.push_back(data);
  }

  std::uintptr_t& TapeVM::top() {
    assert(!m_stack.empty());
    return m_stack.back();
  }


  std::uintptr_t& TapeVM::at(std::size_t index) {
    assert(!m_stack.empty() && index < m_stack.size());
    return m_stack[index];
  }


  std::uintptr_t TapeVM::pop() {
    assert(!m_stack.empty());
    std::uintptr_t ret = m_stack.back();
    m_stack.pop_back();
    return ret;
  }


  std::size_t TapeVM::stackSize() {
    return m_stack.size();
  }

  void TapeVM::rpush(std::uintptr_t data) {
    m_rstack.push_back(data);
  }


  std::uintptr_t& TapeVM::rtop() {
    assert(!m_rstack.empty());
    return m_rstack.back();
  }


  std::uintptr_t& TapeVM::rat(std::size_t index) {
    assert(!m_rstack.empty() && index < m_rstack.size());
    return m_rstack[index];
  }


  std::uintptr_t TapeVM::rpop() {
    assert(!m_rstack.empty());
    std::uintptr_t ret = m_rstack.back();
    m_rstack.pop_back();
    return ret;
  }


  std::size_t TapeVM::rstackSize() {
    return m_rstack.size();
  }


  void TapeVM::fpush(float data) {
    m_fstack.push_back(data);
  }

  float& TapeVM::ftop() {
    assert(!m_fstack.empty());
    return m_fstack.back();
  }


  float& TapeVM::fat(std::size_t index) {
    assert(!m_fstack.empty() && index < m_fstack.size());
    return m_fstack[index];
  }


  float TapeVM::fpop() {
    assert(!m_fstack.empty());
    float ret = m_fstack.back();
    m_fstack.pop_back();
    return ret;
  }


  std::size_t TapeVM::fstackSize() {
    return m_fstack.size();
  }


  void TapeVM::jump(int branches) {
    if (m_exec.empty()) return;

    auto& ip  = m_exec.back().ip;
    auto  nip = static_cast<std::ptrdiff_t>(ip) + branches;

    assert(nip >= 0);

    ip = static_cast<std::size_t>(nip);
  }

  std::string TapeVM::getNext() {
    std::string output;
    int         ch;

    while((ch = m_input.get()) != EOF && std::isspace(ch));

    if (ch == EOF)
      return {};

    for (ch = m_input.get(); ch != EOF && !std::isspace(ch); ch = m_input.get())
      output.push_back(char(ch));
    
    if (ch != EOF)
      m_input.unget();

    return output;
  }

    bool TapeVM::isInteger(const std::string& word) {
    switch (word[0]) {
      case '#':
      {
        bool n = false;
        for (auto i = 1; i < word.length(); i++) {
          if (!std::isdigit(word[i]))
            if (word[i] == '-' && i == 1 && !n) n = true;
            else return false;
        }
      } break;

      case '$':
        for (auto i = 1; i < word.length(); i++) {
          if (!std::isxdigit(word[i]))
            return false;
        }
        break;

      case '%':
        for (auto i = 1; i < word.length(); i++) {
          if (word[i] != '1' && word[i] != '0')
            return false;
        }
        break;
      
      default: 
        return false;
    }

    return true;
  }

  bool TapeVM::isRealnum(const std::string& word) {
    bool        p = false,
                e = false,
                m = false,
                n = false;
    std::size_t i = 0;

    if (word[i] != '&')
      return false;
    
    if (word[++i] == '-') {
      n = true;
      i++;
    }

    for (; i < word.length(); i++) {
      char ch = word[i];

      if (!std::isdigit(ch)) {
        switch (ch) {
          case '.': 
            if (!p) p = true;
            else return false;
            break;

          case 'e':
            if (!e) e = true;
            else return false;
            break;

          case '-':
            if (e && !m) m = true;
            else return false;
            break;

          case '+':
            if (e && !m) m = true;
            else return false;
            break;
          
          default:
            return false;
        }
      }
    }
    return true;
  }


  std::uintptr_t TapeVM::toInteger(const std::string& word) {
    std::string    translation = word;
    std::uintptr_t output      = 0;
    char           prefix      = translation[0];

    translation.erase(0, 1);

    switch (prefix) {
      case '#': output = (std::uintptr_t)std::stol(translation); break;
      case '$': output = std::stoul(translation, nullptr, 16);   break;
      case '%': output = std::stoul(translation, nullptr, 2);    break;
    }

    return output;
  }


  float TapeVM::toRealnum(const std::string& word) {
    std::string translation = word;
    translation.erase(0, 1);
    return std::stof(translation);
  }


  void TapeVM::processToken(const std::string& word) {
    auto* w = findWord(word);

    if (w) {
      switch (getInputMode()) {
        case TapeVM::InputMode::Interpreting:
          xpush(w->code);
          execute();
          break;
        
        case TapeVM::InputMode::Compiling:
          if (w->immediate) {
            xpush(w->code);
            execute();
          }
          else {
            if (w->code.size() > 2)
              compileReference(getLastDefinition(), word);

            else 
              compileInline(getLastDefinition(), w->code[0].func, w->code[0].data);
          }
          break;
      }
      return;
    }
    else if (isInteger(word)) {
      switch (getInputMode()) {
        case TapeVM::InputMode::Interpreting:
          push(toInteger(word));
          break;
        
        case TapeVM::InputMode::Compiling:
          compileInline(getLastDefinition(), findWord("(LIT)")->code[0].func, toInteger(word));
          break;
      }
      return;
    }
    else if (isRealnum(word)) {
      switch (getInputMode()) {
        case TapeVM::InputMode::Interpreting:
          fpush(toRealnum(word));
          break;
        
        case TapeVM::InputMode::Compiling:
        {
          auto* d = findWord(getLastDefinition());
          auto  p = alloc(sizeof(float));
          
          compileInline(getLastDefinition(), findWord("(FLIT)")->code[0].func, p);
          findMem(p)->pinned = true;

          auto* f = (float*)(d->code.back().data);
          *f      = toRealnum(word);
        }  break;
      }
      return;
    }
    
    throw TapeError("Unknown Word", word);
  }

  void TapeVM::setImmediate(const std::string_view& word) {
    auto* w = findWord(word);

    if (!w)
      throw TapeError("No word to mark IMMEDIATE", word);

    w->immediate = true;
  }

  void TapeVM::setAllocating(bool flag) {
    m_isAllocating = flag;
  }

  bool TapeVM::isAllocating() {
    return m_isAllocating;
  }

  void TapeVM::cpush(const TapeVM::ControlFrame& frame) {
    m_cstack.push_back(frame);
  }

  TapeVM::ControlFrame& TapeVM::ctop() {
    assert(!m_cstack.empty());
    return m_cstack.back();
  }

  TapeVM::ControlFrame TapeVM::cpop() {
    assert(!m_cstack.empty());
    auto frame = m_cstack.back();
    m_cstack.pop_back();
    return frame;
  }

  bool TapeVM::cstack_empty() {
    return m_cstack.empty();
  }

  void TapeVM::setSemmantics(const std::string_view& word, const std::string_view& comment) {
    auto* w = findWord(word);

    if (w)
      w->semantics += std::string(comment);
  }

  void TapeVM::clearStacks() {
    m_stack.clear();
    m_fstack.clear();
    m_rstack.clear();
    m_exec.clear();
    m_cstack.clear();
    m_ostack.clear();
    resetScratchArena(TapeVM::ScratchReset::ClearStacks);
  }

}