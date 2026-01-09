/* EventHandler.hpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#pragma once

#include <NoctSys/Configuration.hxx>
#include <SFML/Window/Window.hpp>
#include <SFML/Window/Event.hpp>

namespace noct {
  class NoctSysAPI EventHandler 
  {
    public:
      EventHandler() {}
      virtual ~EventHandler() {}

      virtual void onClosed() {}
      virtual void onFocusGained() {}
      virtual void onFocusLost() {}
      virtual void onJoystickButtonPressed(const sf::Event::JoystickButtonPressed& e) {}
      virtual void onJoystickButtonReleased(const sf::Event::JoystickButtonReleased& e) {}
      virtual void onJoystickConnected(const sf::Event::JoystickConnected& e) {}
      virtual void onJoystickDisconnected(const sf::Event::JoystickDisconnected& e) {}
      virtual void onJoystickMoved(const sf::Event::JoystickMoved& e) {}
      virtual void onKeyPressed(const sf::Event::KeyPressed& e) {}
      virtual void onKeyReleased(const sf::Event::KeyReleased& e) {}
      virtual void onMouseButtonPressed(const sf::Event::MouseButtonPressed& e) {}
      virtual void onMouseButtonReleased(const sf::Event::MouseButtonReleased& e) {}
      virtual void onMouseEntered() {}
      virtual void onMouseLeft() {}
      virtual void onMouseMoved(const sf::Event::MouseMoved& e) {}
      virtual void onMouseMovedRaw(const sf::Event::MouseMovedRaw& e) {}
      virtual void onMouseWheelScrolled(const sf::Event::MouseWheelScrolled& e) {}
      virtual void onResized(const sf::Event::Resized& e) {}
      virtual void onSensorChanged(const sf::Event::SensorChanged& e) {}
      virtual void onTextEntered(const sf::Event::TextEntered& e) {}
      virtual void onTouchBegan(const sf::Event::TouchBegan& e) {}
      virtual void onTouchEnded(const sf::Event::TouchEnded& e) {}
      virtual void onTouchMoved(const sf::Event::TouchMoved& e) {}

      void onHandleEvents(sf::Window& window);
  };
}