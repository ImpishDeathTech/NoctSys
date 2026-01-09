/* Console.cpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#include <NoctSys/Engine/Console.hpp>

#include <cstring>
#include <sstream>
#include <cstdio>

#if defined(__NoctSys_Windows__)
  #include <processthreadsapi.h>
#endif

namespace noct {


  void LogRingBuffer::push(const LogEntry& entry) {
    std::uint32_t w = writeIdx.load(std::memory_order_relaxed),
                  n = (w+1) % LOG_CAPACITY;
    
    if (n == readIdx.load(std::memory_order_acquire))
      readIdx.store((readIdx+1) %LOG_CAPACITY, std::memory_order_release);
    
    entries[w] = entry;
    writeIdx.store(n, std::memory_order_release);
  }


  bool LogRingBuffer::pop(LogEntry& out) {
    std::uint32_t r = readIdx.load(std::memory_order_relaxed);

    if (r == writeIdx.load(std::memory_order_acquire))
      return false;
    
    out = entries[r];

    readIdx.store((r+1) % LOG_CAPACITY, std::memory_order_release);
    return true;
  }


  Console::Console()
    : m_log(), m_font(nullptr), m_logRing()
  {}

  Console::~Console() {}


  std::string Console::levelText(LogLevel l) {
    std::string ret;

    switch (l) {
      case LogLevel::Info:  ret = "INFO";  break;
      case LogLevel::Warn:  ret = "WARN";  break;
      case LogLevel::Error: ret = "ERROR"; break;
      case LogLevel::Debug: ret = "DEBUG"; break;
      case LogLevel::Fatal: ret = "FATAL"; break;
      default: ret = "...";
    }

    return ret;
  }


  sf::Color Console::levelColor(LogLevel l) {
    sf::Color ret;

    switch (l) {
      case LogLevel::Info: ret  = sf::Color::White;       break;
      case LogLevel::Warn: ret  = sf::Color::Yellow;      break;
      case LogLevel::Error: ret = sf::Color::Red;         break;
      case LogLevel::Debug: ret = sf::Color(150,150,150); break;
      case LogLevel::Fatal: ret = sf::Color::Magenta;     break;
      default: ret = sf::Color::Cyan;
    }

    return ret;
  }


  void Console::setFont(const sf::Font* font) {
    m_font = font;
  }


  LogRingBuffer& Console::getLogRing() {
    return m_logRing;
  }


  void Console::update() {
    LogEntry entry;

    while (m_logRing.pop(entry))
      addLine(entry);
  }


  void Console::write(LogLevel lvl, const std::string_view& message)  {
    LogEntry entry;

    entry.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    entry.level     = lvl;
#if defined(__NoctSys_Windows__)
    entry.threadId  = GetCurrentThreadId();
#endif 
    std::memset(entry.message, '\0', sizeof(entry.message) - 1);
    std::strncpy(entry.message, message.data(), sizeof(entry.message) - 1);

    m_logRing.push(entry);
  }


  void Console::addLine(const LogEntry& entry) {
    if (!m_font) return;

    ConsoleLine line{
      sf::Text(*m_font),
      entry.level
    };

    char              timestamp[std::size("HH:MM:SS")]; 
    std::stringstream ss;
    std::tm           tm;

#if defined(__NoctSys_Windows__)
    gmtime_s(&tm, &entry.timestamp);
#else 
    gmtime_r(&entry.timestamp, &tm);
#endif

    std::strftime(std::data(timestamp), std::size(timestamp), "%H:%M:%S", &tm);       
    
    ss << timestamp << " -> [" << levelText(entry.level) << "]{" << entry.threadId << "}: " << entry.message;
    
    line.text.setFillColor(levelColor(entry.level));
    line.text.setString(ss.str());
    line.text.setPosition(m_log.back().text.getPosition() + 
      sf::Vector2f(0.f, m_log.back().text.getGlobalBounds().size.y));

    m_log.push_back(line);
  }

  
  void Console::debug(const std::string_view& message) {
    write(LogLevel::Debug, message);
  }


  void Console::info(const std::string_view& message) {
    write(LogLevel::Info, message);
  }


  void Console::warn(const std::string_view& message) {
    write(LogLevel::Warn, message);
  }


  void Console::error(const std::string_view& message) {
    write(LogLevel::Error, message);
  }


  void Console::error(Error& exn) {
    write(LogLevel::Error, exn.what());
  }


  void Console::fatal(const std::string_view& message) {
    write(LogLevel::Fatal, message);
  }


  void Console::fatal(Error& exn) {
    write(LogLevel::Fatal, exn.what());
  }


  void Console::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (const auto& line : m_log)
      target.draw(line.text, states);
  }

}