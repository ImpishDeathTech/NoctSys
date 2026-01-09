/* Console.hpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#pragma once

// NoctSys Libraries
#include <NoctSys/Configuration.hxx>
#include <NoctSys/Exception/Error.hpp>

// SFML Libraries
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

// Standard Libraries 
#include <cstdint>
#include <atomic>
#include <array>
#include <string>
#include <mutex>
#include <map>
#include <utility>
#include <functional>

namespace noct {
  enum class LogLevel 
    : std::uint8_t
  {
    Debug,
    Info, 
    Warn,
    Error,
    Fatal
  };

  struct NoctSysAPI LogEntry 
  {
    std::time_t   timestamp;
    LogLevel      level;
    std::uint32_t threadId;
    char          message[256];
  };

  constexpr std::size_t LOG_CAPACITY = 2048;


  struct NoctSysAPI LogRingBuffer 
  {
    std::atomic<std::uint32_t>         writeIdx{0},
                                       readIdx{0};
    std::array<LogEntry, LOG_CAPACITY> entries;

    void push(const LogEntry& e);
    bool pop(LogEntry& out);
  };

  struct NoctSysAPI ConsoleLine
  {
    sf::Text text;
    LogLevel level;
  };


  class NoctSysAPI Console
    : public sf::Drawable
  {
    std::vector<ConsoleLine> m_log;
    const sf::Font*          m_font;
    LogRingBuffer            m_logRing;

  public:

    Console();
    ~Console();

    void           setFont(const sf::Font* font);
    LogRingBuffer& getLogRing();
    void           update();
    
    void           write(LogLevel lvl, const std::string_view& msg);

    void           debug(const std::string_view& message);
    void           info(const std::string_view& message);
    void           warn(const std::string_view& message);
    void           error(const std::string_view& message);
    void           error(Error& exn);
    void           fatal(const std::string_view& message);
    void           fatal(Error& exn);

  private: 
    void        addLine(const LogEntry& entry);
    sf::Color   levelColor(LogLevel l);
    std::string levelText(LogLevel l);
    void        draw(sf::RenderTarget& target, sf::RenderStates states) const;
  };


  NoctSysAPI Console console;
}