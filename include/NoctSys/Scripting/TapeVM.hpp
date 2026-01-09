/* TapeVM.hpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#pragma once 

#include <NoctSys/Configuration.hxx>
#include <NoctSys/Scripting/TapeVM/InputStream.hpp>
#include <NoctSys/Scripting/TapeVM/OutputStream.hpp>

#include <cstdint>
#include <atomic>
#include <array>
#include <string>
#include <mutex>
#include <vector>
#include <map>
#include <utility>
#include <functional>

#define TAPE_VERSION_MAJOR 1 
#define TAPE_VERSION_MINOR 0
#define TAPE_VERSION_PATCH 0


namespace noct {

  class NoctSysAPI TapeVM
  {
    std::vector<std::uintptr_t>     m_stack;
    std::vector<std::uintptr_t>     m_rstack;
    std::vector<float>              m_fstack;
    std::string                     m_lastDefinition;
    InputStream                     m_input;
    OutputStream                    m_output;
    std::vector<std::unique_ptr<OutputSource<char>>>
                                    m_ostack;
    bool                            m_isAllocating;
    std::vector<std::string>        m_includeDirectories;

  public:
    TapeVM();
    ~TapeVM();

    enum class InputMode
      : std::uint8_t 
    {
      Executing,
      Compiling,
      Interpreting
    };

    typedef std::function<void(TapeVM&)>  Function;

    struct FuncdatPair {
      Function       func; 
      std::uintptr_t data;
    };
    
    typedef std::vector<FuncdatPair> Word;

    struct WordTag {
      Word        code;
      bool        immediate { false };
      std::string semantics;
    };

    struct XToken {
      const Word* word; 
      std::size_t ip;
    };

    typedef std::vector<XToken>                  XVector;
    typedef std::map<std::string, WordTag>       Dictionary;

    struct MemTag {
      std::size_t    size;
      std::uintptr_t data;
      bool           free,
                     pinned;
    };
    
    typedef std::vector<MemTag> HeapArena;

    struct ScratchArena {
      std::vector<std::uint8_t> buffer;
      std::size_t               dp { 0ul };
    };

    enum ScratchReset {
      Line,
      Definition,
      ClearStacks
    };

    struct ControlFrame {
      enum Type { 
        IF, 
        ELSE,
        BEGIN,
        WHILE,
        DO
      }                        type;
      std::size_t              patch_ip;
      std::vector<std::size_t> leave_patches;
    };

    typedef std::vector<ControlFrame> ControlStack;

  private:
    ControlStack m_cstack;
    InputMode    m_mode;
    Dictionary   m_dict;
    XVector      m_exec;
    HeapArena    m_mem;
    ScratchArena m_smem;

  public:
    void             addIncludeDirectory(const std::string& directory);
    std::string      convertModname(std::string modname);
    std::string      convertDirectory(std::string directory, const std::string& modname);

    void             setAllocating(bool flag);
    bool             isAllocating();
    InputStream&     input();
    OutputStream&    output();
    void             pushOutput(std::unique_ptr<OutputSource<char>> source);
    void             popOutput();

    void             setInputMode(InputMode mode);
    InputMode        getInputMode();
    std::string_view getLastDefinition();

    WordTag*        findWord(const std::string_view& word);
    void            addWord(const std::string_view& name, const Function& func, std::uintptr_t data=0ul);
    void            addWord(const std::string_view& name, const Word& token);
    void            addWord(const std::string_view& name);
    void            compileInline(const std::string_view& word, const Function& func, std::uintptr_t data=0ul);
    void            compileReference(const std::string_view& word, const std::string_view& token);
    void            setImmediate(const std::string_view& word);
    void            setSemmantics(const std::string_view& word, const std::string_view& comment);
    
    void            xpush(const Word& word);
    XToken&         getExecuting();
    void            jump(int branches);
    void            execute();

    std::uintptr_t  allot(std::size_t sz);
    bool            isScratchData(std::uintptr_t p);
    void            resetScratchArena(ScratchReset r);
    void            reserveScratchArena(std::size_t reserve);

    std::uintptr_t  alloc(std::size_t sz);
    std::uintptr_t  realloc(std::uintptr_t p, std::size_t sz);
    void            freeMem(std::uintptr_t p);
    MemTag*         findMem(std::uintptr_t p);
    void            setPinned(std::uintptr_t word, bool flag=true);

    void            push(std::uintptr_t cell);
    std::uintptr_t& top();
    std::uintptr_t& at(std::size_t idx);
    std::uintptr_t  pop();
    std::size_t     stackSize();

    void            rpush(std::uintptr_t cell);
    std::uintptr_t& rtop();
    std::uintptr_t& rat(std::size_t idx);
    std::uintptr_t  rpop();
    std::size_t     rstackSize();

    void            fpush(float real);
    float&          ftop();
    float&          fat(std::size_t idx);
    float           fpop();
    std::size_t     fstackSize();

    void            cpush(const ControlFrame& frame);
    ControlFrame&   ctop();
    ControlFrame    cpop();
    bool            cstack_empty();

    void            clearStacks();

    std::string    getNext();

    bool           isInteger(const std::string& word);
    std::uintptr_t toInteger(const std::string& word);
    bool           isRealnum(const std::string& word);
    float          toRealnum(const std::string& word);

    void           processToken(const std::string& token);
    void           loadTapeBase();

    void operator<<(const std::string& s) {
      m_input.push(new StringInputSource(s));
    }

    void operator<<(const std::filesystem::path& p) {
      m_input.push(new FileInputSource(p));
    }

  private:
    void loadCompilerPrimitives();
    void loadStackOperators();
    void loadControlStructures();
    void loadParsingWords();
    void loadVariableDefiners();
    void loadStdIO();
  };
}