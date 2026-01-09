/* StateBase.hpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#pragma once

#include <NoctSys/Configuration.hxx>
#include <NoctSys/Engine/EventHandler.hpp>
#include <memory>

namespace noct {
  class NoctSysAPI StateBase
    : public EventHandler
  {
    public:
      virtual void onInit()           = 0;
      virtual void onUpdate(float dt) = 0;
      virtual void onRender()         = 0;

      virtual void onFreeze() {}
      virtual void onThaw()   {}
  };

  typedef std::unique_ptr<StateBase> StatePtr;
}