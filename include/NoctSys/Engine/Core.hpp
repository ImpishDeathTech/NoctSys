/* Core.hpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#pragma once

// NoctSys Libraries
#include <NoctSys/Configuration.hxx>
#include <NoctSys/Engine/StateStack.hpp>
#include <NoctSys/Resource/Database.hpp>

// SFML Libraries
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Audio/Music.hpp>

namespace noct {
  class NoctSysAPI Core 
  {
    public:
      Core() {}
      ~Core() {}

      sf::RenderWindow window;
      sf::Music        music;
      StateStack       state;
      ResourceDatabase resource;
  };

  typedef std::shared_ptr<Core> CorePtr;
  typedef StateBase* (*StateLoader)(CorePtr&);
}