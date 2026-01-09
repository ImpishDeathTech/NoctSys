/* GameLoop.hpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#pragma once

#include <NoctSys/Configuration.hxx>
#include <NoctSys/Engine/Core.hpp>

#include <SFML/System/Clock.hpp>

namespace noct {
  class NoctSysAPI GameLoop 
  {
    CorePtr   m_core;
    sf::Clock m_delta,
              m_clock;

  public:
    GameLoop();
    ~GameLoop();

    Core& getCore();

    template<typename StateT>
    void load() {
      m_core->state.push(new StateT(m_core));
      m_core->state.process();
    }

    void start();
  };
}