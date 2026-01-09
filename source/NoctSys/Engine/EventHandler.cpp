/* EventHandler.cpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#include <NoctSys/Engine/EventHandler.hpp>

namespace noct {

  NoctSysAPI void EventHandler::onHandleEvents(sf::Window& window) {
    static const auto x_onClosed                 = [=](const sf::Event::Closed& e){ onClosed(); };
    static const auto x_onFocusGained            = [=](const sf::Event::FocusGained& e){ onFocusGained(); };
    static const auto x_onFocusLost              = [=](const sf::Event::FocusLost& e){ onFocusLost(); };
    static const auto x_onJoystickButtonPressed  = [=](const sf::Event::JoystickButtonPressed& e){ onJoystickButtonPressed(e); };
    static const auto x_onJoystickButtonReleased = [=](const sf::Event::JoystickButtonReleased& e){ onJoystickButtonReleased(e); };
    static const auto x_onJoystickConnected      = [=](const sf::Event::JoystickConnected& e){ onJoystickConnected(e); };
    static const auto x_onJoystickDisconnected   = [=](const sf::Event::JoystickDisconnected& e){ onJoystickDisconnected(e); };
    static const auto x_onJoystickMoved          = [=](const sf::Event::JoystickMoved& e){ onJoystickMoved(e); };
    static const auto x_onKeyPressed             = [=](const sf::Event::KeyPressed& e){ onKeyPressed(e); };
    static const auto x_onKeyReleased            = [=](const sf::Event::KeyReleased& e){ onKeyReleased(e); };
    static const auto x_onMouseButtonPressed     = [=](const sf::Event::MouseButtonPressed& e){ onMouseButtonPressed(e); };
    static const auto x_onMouseButtonReleased    = [=](const sf::Event::MouseButtonReleased& e){ onMouseButtonReleased(e); };
    static const auto x_onMouseEntered           = [=](const sf::Event::MouseEntered& e){ onMouseEntered(); };
    static const auto x_onMouseLeft              = [=](const sf::Event::MouseLeft& e){ onMouseLeft(); };
    static const auto x_onMouseMoved             = [=](const sf::Event::MouseMoved& e){ onMouseMoved(e); };
    static const auto x_onMouseWheelScrolled     = [=](const sf::Event::MouseWheelScrolled& e){ onMouseWheelScrolled(e); };
    static const auto x_onMouseMovedRaw          = [=](const sf::Event::MouseMovedRaw& e){ onMouseMovedRaw(e); };
    static const auto x_onResized                = [=](const sf::Event::Resized& e){ onResized(e); };
    static const auto x_onSensorChanged          = [=](const sf::Event::SensorChanged& e){ onSensorChanged(e); };
    static const auto x_onTextEntered            = [=](const sf::Event::TextEntered& e){ onTextEntered(e); };
    static const auto x_onTouchBegan             = [=](const sf::Event::TouchBegan& e){ onTouchBegan(e); };
    static const auto x_onTouchEnded             = [=](const sf::Event::TouchEnded& e){ onTouchEnded(e); };
    static const auto x_onTouchMoved             = [=](const sf::Event::TouchMoved& e){ onTouchMoved(e); };

    window.handleEvents(
      x_onClosed,
      x_onFocusGained,
      x_onFocusLost,
      x_onJoystickButtonPressed,
      x_onJoystickButtonReleased,
      x_onJoystickConnected,
      x_onJoystickDisconnected,
      x_onJoystickMoved,
      x_onKeyPressed,
      x_onKeyReleased,
      x_onMouseButtonPressed,
      x_onMouseButtonReleased,
      x_onMouseEntered,
      x_onMouseLeft,
      x_onMouseMoved, 
      x_onMouseMovedRaw,
      x_onResized, 
      x_onSensorChanged,
      x_onTextEntered,
      x_onTouchBegan,
      x_onTouchMoved, 
      x_onTouchEnded
    );
  }
}