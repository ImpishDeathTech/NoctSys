/* GameLoop.cpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#include <NoctSys/Engine/GameLoop.hpp>

namespace noct {
  GameLoop::GameLoop()
    : m_core(), m_clock(), m_delta()
  {
    m_core->window.create(sf::VideoMode({600, 800}), "NoctSys", 
      sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close);

    m_core->window.setVisible(false);
  }


  GameLoop::~GameLoop() {

  }


  Core& GameLoop::getCore() {
    return *m_core;
  }


  void GameLoop::start() {
    float newTime     = 0.f,
          frameTime   = 0.f,
          currentTime = m_clock.getElapsedTime().asSeconds(),
          deltaTime   = m_delta.restart().asSeconds(),
          accumulator = 0.f;

    m_core->window.setVisible(true);

    while (m_core->window.isOpen()) {
      m_core->state.process();

      newTime   = m_clock.getElapsedTime().asSeconds();
      frameTime = newTime - currentTime;

      if (frameTime < .25f)
        frameTime = .25f;

      currentTime  = newTime;
      accumulator += frameTime;

      while (accumulator >= deltaTime) {
        m_core->state.handleEvents(m_core->window);
        m_core->state.update(deltaTime);

        accumulator -= deltaTime;
      }

      m_core->state.render();
      deltaTime = m_delta.restart().asSeconds();
    }
  }
}