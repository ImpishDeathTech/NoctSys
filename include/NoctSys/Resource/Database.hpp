/* Database.hpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#pragma once

// NoctSys Libraries
#include <NoctSys/Configuration.hxx>
#include <NoctSys/Utilities.hpp>
#include <NoctSys/Resource/NativeScript.hpp>

// XML Library
#include <pugixml.hpp>

// SFML Libraries
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/System/String.hpp>

// Standard Library
#include <map>
#include <memory>
#include <mutex>
#include <optional>


#define NSRESDB_VERSION 1.1

namespace noct {

#if defined(__NoctSys_UNIX__)
  typedef unsigned long ColorInt;
#elif defined(__NoctSys_Windows__)
  typedef unsigned long int ColorInt;
#endif

  template<class T>
  class NoctSysAPI Resource
  {
  public:
    std::filesystem::path path;
    T                     data;
  };

  typedef std::shared_ptr<sf::Texture>     TexturePtr;
  typedef std::shared_ptr<sf::Font>        FontPtr;
  typedef std::shared_ptr<sf::SoundBuffer> SoundBufferPtr;
  typedef std::shared_ptr<sf::Shader>      ShaderPtr;
  typedef std::shared_ptr<NativeScript>    NativeScriptPtr;
  typedef std::optional<sf::Color>         MaybeColor;

  typedef Resource<TexturePtr>     TextureResource;
  typedef Resource<FontPtr>        FontResource;
  typedef Resource<SoundBufferPtr> SoundResource;
  

  class NoctSysAPI ShaderResource 
  {
  public:
    bool                  isInline;
    sf::Shader::Type      type;
    std::string           code;
    std::filesystem::path path;
    ShaderPtr             data;
  };


  typedef std::map<sf::String, TextureResource> TextureMap;
  typedef std::map<sf::String, FontResource>    FontMap;
  typedef std::map<sf::String, SoundResource>   SoundMap;
  typedef std::map<sf::String, ShaderResource>  ShaderMap;
  typedef std::map<sf::String, ColorInt>        ColorMap;
  typedef std::map<sf::String, NativeScriptPtr> NativeScriptMap;

  class NoctSysAPI ResourceDatabase
  {
    mutable std::mutex    m_mutex;

    std::filesystem::path m_textureDirectory,
                          m_fontDirectory,
                          m_soundDirectory,
                          m_shaderDirectory,
                          m_scriptDirectory;

    TextureMap            m_textures;
    FontMap               m_fonts;
    SoundMap              m_sounds;
    ShaderMap             m_shaders;
    ColorMap              m_colors;
    NativeScriptMap       m_nativeScripts;

  public:
    
    ResourceDatabase();
    ResourceDatabase(const std::filesystem::path& xml_path);
    ~ResourceDatabase();

    void setTextureDirectory(const std::filesystem::path& directory);
    void setFontDirectory(const std::filesystem::path& directory);
    void setSoundDirectory(const std::filesystem::path& directory);
    void setShaderDirectory(const std::filesystem::path& directory);
    void setScriptDirectory(const std::filesystem::path& directory);
    
    void loadFromFile(const std::filesystem::path& xml_path);
    bool saveToFile(const std::filesystem::path& xml_path);

    bool loadTextureFromFile(const std::filesystem::path& path, const std::string_view& name={});
    bool loadFontFromFile(const std::filesystem::path& path, const std::string_view& name={});
    bool loadSoundFromFile(const std::filesystem::path& path, const std::string_view& name={});
    bool loadShaderFromFile(const std::filesystem::path& path, sf::Shader::Type type, const std::string_view&name={});
    bool loadNativeScriptFromFile(const std::filesystem::path& path, const std::string_view& name={});

    void addColor(const std::string_view& name, const sf::Color& color);
    void addColor(const sf::Color& color, const std::string_view& name);

    TexturePtr      findTexture(const std::string_view& name);
    FontPtr         findFont(const std::string_view& name);
    SoundBufferPtr  findSound(const std::string_view& name);
    ShaderPtr       findShader(const std::string_view& name);
    MaybeColor      findColor(const std::string_view& name);
    NativeScriptPtr findNativeScript(const std::string_view& name);

    bool eraseTexture(const std::string_view& name);
    bool eraseFont(const std::string_view& name);
    bool eraseSound(const std::string_view& name);
    bool eraseShader(const std::string_view& name);
    bool eraseColor(const std::string_view& name);
    bool eraseNativeScript(const std::string_view& name);

    StringVector listTextures();
    StringVector listFonts();
    StringVector listSounds();
    StringVector listShaders();
    StringVector listColors();
    StringVector listNativeScripts();

  private:
    void writeTextureDatabase(std::fstream& file);
    void writeFontDatabase(std::fstream& file);
    void writeSoundDatabase(std::fstream& file);
    void writeShaderDatabase(std::fstream& file);
    void writeColorDatabase(std::fstream& file);
    void writeScriptDatabase(std::fstream& file);

    void readTextureDatabase(pugi::xml_node& root);
    void readFontDatabase(pugi::xml_node& root);
    void readSoundDatabase(pugi::xml_node& root);
    void readShaderDatabase(pugi::xml_node& root);
    void readColorDatabase(pugi::xml_node& root);
    void readScriptDatabase(pugi::xml_node& root);

    std::string      shaderTypeToString(sf::Shader::Type type);
    sf::Shader::Type shaderTypeFromString(const std::string& type);
  };
}