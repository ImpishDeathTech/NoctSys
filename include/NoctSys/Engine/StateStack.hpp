/* StateStack.hpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#pragma once

#include <NoctSys/Configuration.hxx>
#include <NoctSys/Engine/StateBase.hpp>
#include <stack>

namespace noct {
  class NoctSysAPI StateStack 
  {
    std::stack<StatePtr> m_stack;
    StatePtr             m_newState;

    bool                 m_pushing,
                         m_popping,
                         m_replacing;

  public:
    StateStack();
    ~StateStack();

    void push(StateBase* state, bool isReplacing=false);
    void pop();
    void process();

    void update(float dt);
    void handleEvents(sf::Window& window);
    void render();
  };
}