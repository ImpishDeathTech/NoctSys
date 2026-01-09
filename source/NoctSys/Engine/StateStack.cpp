/* StateStack.cpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#include <NoctSys/Engine/StateStack.hpp>
#include <NoctSys/Exception/StackUnderflowError.hpp>

namespace noct {
  StateStack::StateStack()
    : m_stack(), 
      m_newState(), 
      m_pushing(false), 
      m_popping(false),
      m_replacing(false)
  {}

  StateStack::~StateStack() {
    while (!m_stack.empty())
      m_stack.pop();
  }

  
  void StateStack::push(StateBase* state, bool isReplacing) {
    m_newState.reset(state);
    m_pushing = true;
    m_replacing = isReplacing;
  }


  void StateStack::pop() {
    m_popping = true;
  }


  void StateStack::update(float dt) {
    m_stack.top()->onUpdate(dt);
  }


  void StateStack::handleEvents(sf::Window& window) {
    m_stack.top()->onHandleEvents(window);
  }

  void StateStack::render() {
    m_stack.top()->onRender();
  }

  void StateStack::process() {
    if (m_pushing) {
      if (m_replacing) {
        if (!m_stack.empty())
          m_stack.pop();
        
        else throw StackUnderflowError("cannot replace a state that does not exist");
      }
      if (!m_stack.empty())
        m_stack.top()->onFreeze();
      
      m_stack.push(StatePtr(m_newState.release()));
      m_stack.top()->onInit();
      m_pushing = m_replacing = false;
    }

    if (m_popping) {
      if (!m_stack.empty()) {
        m_stack.pop();

        if (!m_stack.empty())
          m_stack.top()->onThaw();
      }
      else throw StackUnderflowError();

      m_popping = false;
    }
  }
}