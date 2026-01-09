/* Database.cpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#include <NoctSys/Resource/Database.hpp>
#include <NoctSys/Exception/ResourceError.hpp>

#include <fstream>
#include <iomanip>
#include <algorithm>
#include <array>
#include <string>

namespace noct {
  ResourceDatabase::ResourceDatabase()
    : m_textures(), m_fonts(), m_sounds(), m_shaders(), m_colors(), m_mutex()
  {}


  ResourceDatabase::ResourceDatabase(const std::filesystem::path& xml_path) 
    : m_textures(), m_fonts(), m_sounds(), m_shaders(), m_colors(), m_mutex()
  {
    loadFromFile(xml_path);
  }

  ResourceDatabase::~ResourceDatabase() {

  }


  void ResourceDatabase::loadFromFile(const std::filesystem::path& xml_path) {
    std::lock_guard<std::mutex> lock(m_mutex);
    pugi::xml_document     doc; 
    pugi::xml_parse_result result = doc.load_file(xml_path.c_str());
    
    if (!result)
      throw ResourceError(xml_path, result.description());

    pugi::xml_node root = doc.child("ResourceDatabase");
    double version = root.attribute("version").as_double();

    if (version != NSRESDB_VERSION)
      throw ResourceError(xml_path, "invalid ResourceDatabase version: " + std::to_string(version));

    readTextureDatabase(root);
    readFontDatabase(root);
    readSoundDatabase(root);
    readShaderDatabase(root);
    readColorDatabase(root);
    readScriptDatabase(root);
  }


  bool ResourceDatabase::saveToFile(const std::filesystem::path& xml_path) {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::fstream file(xml_path, std::ios::out | std::ios::trunc);

    if (file.is_open()) {
      file << "<?xml version='1.0' encoding='UTF-8'>\n"
           << "<ResourceDatabase version='" << NSRESDB_VERSION << "'>\n"; 

      writeTextureDatabase(file);
      writeFontDatabase(file);
      writeSoundDatabase(file);
      writeShaderDatabase(file);
      writeColorDatabase(file);
      writeScriptDatabase(file);

      file << "</ResourceDatabase>\n";
      file.close();

      return true;
    }

    return false;
  }


  void ResourceDatabase::writeTextureDatabase(std::fstream& file) {
    file << "  <TextureDatabase directory=" << m_textureDirectory << ">\n";

    for (const auto& res : m_textures) {
      file << "    <TextureEntry name='" << res.first.toAnsiString() << "' type=" 
           << res.second.path.extension() << ">\n      " 
           << res.second.path.stem().string() << std::endl
           << "    </TextureEtry>\n";
    }

    file << "  </TextureDatabase>\n";
  }


  void ResourceDatabase::writeFontDatabase(std::fstream& file) {
    file << "  <FontDatabase diretory=" << m_fontDirectory << ">\n";

    for (const auto& res : m_fonts) {
      file << "    <FontEntry name='" << res.first.toAnsiString() << "' type=" 
           << res.second.path.extension() << ">\n      " 
           << res.second.path.stem().string() << std::endl
           << "</FontEntry>\n";
    }

    file << "  </FontDatabase>\n";
  }


  void ResourceDatabase::writeSoundDatabase(std::fstream& file) {
    file << "  <SoundDatabase directory=" << m_soundDirectory << ">\n";

    for (const auto& res : m_sounds) {
      file << "    <SoundEntry name='" << res.first.toAnsiString() << "' type=" 
           << res.second.path.extension() << ">\n      " 
           << res.second.path.string() 
           << "</SoundEntry>\n";
    }

    file << "  </SoundDatabase>\n";
  }

  
  std::string ResourceDatabase::shaderTypeToString(sf::Shader::Type type) {
    std::string ret;

    switch (type) {
      case sf::Shader::Type::Vertex:   ret = "vert"; break;
      case sf::Shader::Type::Geometry: ret = "geom"; break;
      case sf::Shader::Type::Fragment: ret = "frag"; break;
    }
    return ret;
  }


  sf::Shader::Type ResourceDatabase::shaderTypeFromString(const std::string& type) {
    sf::Shader::Type ret;

    if (type == "vert")      ret = sf::Shader::Type::Vertex;
    else if (type == "geom") ret = sf::Shader::Type::Geometry;
    else if (type == "frag") ret = sf::Shader::Type::Fragment;
    
    return ret;
  }


  void ResourceDatabase::writeShaderDatabase(std::fstream& file) {
    file << "  <ShaderDatabase directory=" << m_shaderDirectory << ">\n";

    for (const auto& res : m_shaders) {
      file << "    <ShaderEntry name=\"" << res.first.toAnsiString() 
           << " type=" <<  shaderTypeToString(res.second.type)
           << " inline=" << (res.second.isInline ? "'true'" : "'false'") << ">\n";
           
      if (res.second.isInline) 
        file << res.second.code << std::endl
             << "    </ShaderEntry>\n";
      
      else file << "      " << res.second.path.string() << std::endl
                << "</ShaderEntry>\n";
       
    }

    file << "  </ShaderDatabase>\n";
  }


  void ResourceDatabase::writeColorDatabase(std::fstream& file) {
    file << "  <ColorDatabase>\n";

    for (const auto& res : m_colors) {
      sf::Color color(res.second);
      file << "    <ColorEntry name='" << res.first.toAnsiString() << "' hex='#";
      file << std::hex << std::setw(2) << std::setfill('0') << (unsigned)color.r;
      file << std::hex << std::setw(2) << std::setfill('0') << (unsigned)color.g; 
      file << std::hex << std::setw(2) << std::setfill('0') << (unsigned)color.b; 
      file << std::hex << std::setw(2) << std::setfill('0') << (unsigned)color.a;
      file << "'>\n";
    }

    file << "  </ColorDatabase>\n";
  }


  void ResourceDatabase::writeScriptDatabase(std::fstream& file) {
    file << "  <ScriptDatabase directory='" << m_scriptDirectory << "'>\n";

    for (const auto& script : m_nativeScripts) {
      file << "    <ScriptEntry name='" << script.first.toAnsiString() << "' lang='noct'>\n"
           << "      " << script.second->getFilePath().stem().string() << std::endl
           << "    </ScriptEntry>\n";
    }

    file << "  </ScriptDatabase>\n";
  }


  void ResourceDatabase::readTextureDatabase(pugi::xml_node& root) {
    if (auto textureDB = root.child("TextureDatabase")) {
      std::string directory = textureDB.attribute("directory").as_string();

      for (auto texture : textureDB.children("TextureEntry")) {
        TextureResource res {
#if defined(__NoctSys_UNIX__)
          std::filesystem::path(directory + '/' + texture.text().as_string() + '.' + texture.attribute("type").as_string()),
#elif defined(__NoctSys_Windows__)
          std::filesystem::path(directory + '\\' + texture.text().as_string() + '.' + texture.attribute("type").as_string()),
#endif
          std::make_shared<sf::Texture>()
        };

        if (!res.data->loadFromFile(res.path))
          throw ResourceError(res.path, "could not locate texture resource");
        
        else m_textures[sf::String(texture.attribute("name").as_string())] = res;
      }
    }
  }


  void ResourceDatabase::readFontDatabase(pugi::xml_node& root) {
    if (auto fontDB = root.child("FontDatabase")) {
      std::string directory = fontDB.attribute("directory").as_string();

      for (auto font : fontDB.children("FontEntry")) {
        FontResource res {
#if defined(__NoctSys_UNIX__)
          std::filesystem::path(directory + '/' + font.text().as_string() + '.' + font.attribute("type").as_string()),
#elif defined(__NoctSys_Windows__)
          std::filesystem::path(directory + '\\' + font.text().as_string() + '.' + font.attribute("type").as_string()),
#endif
          std::make_shared<sf::Font>()
        };

        if (!res.data->openFromFile(res.path))
          throw ResourceError(res.path, "could not locate font resource");
        
        else m_fonts[sf::String(font.attribute("name").as_string())] = res;
      }
    }
  }


  void ResourceDatabase::readSoundDatabase(pugi::xml_node& root) {
    if (auto soundDB = root.child("SoundDatabase")) {
      std::string directory = soundDB.attribute("directory").as_string();

      for (auto sound : soundDB.children("SoundEntry")) {
        SoundResource res {
#if defined(__NoctSys_UNIX__)
          std::filesystem::path(directory + '/' + sound.text().as_string() + '.' + sound.attribute("type").as_string()),
#elif defined(__NoctSys_Windows__)
          std::filesystem::path(directory + '\\' + sound.text().as_string() + '.' + sound.attribute("type").as_string()),
#endif
          std::make_shared<sf::SoundBuffer>()
        };

        if (!res.data->loadFromFile(res.path))
          throw ResourceError(res.path, "could not locate sound resource");
        
        else m_sounds[sf::String(sound.attribute("name").as_string())] = res;
      }
    }
  }


  void ResourceDatabase::readShaderDatabase(pugi::xml_node& root) {
    if (auto shaderDB = root.child("ShaderDatabase")) {
      std::string directory = shaderDB.attribute("directory").as_string();

      for (auto shader : shaderDB.children("ShaderEntry")) {
        ShaderResource res;
        res.isInline = shader.attribute("inline").as_bool();
        res.type     = shaderTypeFromString(shader.attribute("type").as_string());
        res.data     = std::make_shared<sf::Shader>();

        if (res.isInline) {
          res.code = shader.text().as_string();

          if (!res.data->loadFromMemory(res.code.c_str(), res.type)) {
            std::string shaderName = shader.attribute("name").as_string();
            throw ResourceError("couldn't load shader '" + shaderName + "'");
          }
          
        } else {
#if defined(__NoctSys_UNIX__)
          res.path = std::filesystem::path(directory + '/' + shader.text().as_string() + '.' + shader.attribute("type").as_string());
#elif defined(__NoctSys_Windows__)
          res.path = std::filesystem::path(directory + '\\' + shader.text().as_string() + '.' + shader.attribute("type").as_string());
#endif
          if (!res.data->loadFromFile(res.path, res.type))
            throw ResourceError(res.path, "could not locate shader resource");
          
          m_shaders[sf::String(shader.attribute("name").as_string())] = res;
        }
      }
    }
  }


  void ResourceDatabase::readColorDatabase(pugi::xml_node& root) {
    if (auto colorDB = root.child("ColorDatabase")) {
      for (auto color : colorDB.children("ColorEntry")) {
        if (auto hex = color.attribute("hex")) {
          std::string hexstring = hex.as_string();
          
          hexstring.erase(0);

          ColorInt    colorint  = std::stoul(hexstring, nullptr, 16);
          m_colors[sf::String(color.attribute("name").as_string())] = colorint;
        }
        else if (auto rgba = color.attribute("rgba")) {
          std::string                 csv = rgba.as_string();
          std::array<std::uint8_t, 4> v;
          std::string                 value;
          std::size_t                 j   = 0;

          for (std::size_t i = 0; i < csv.length(); i++) {
            if (csv[i] == ';') {
              v[j++] = (std::uint8_t)std::stoi(value);
              value.clear();
            } 
            else value += csv[i];
          }

          m_colors[sf::String(color.attribute("name").as_string())] = sf::Color(v[0], v[1], v[2], v[3]).toInteger();
        }
      }
    }
  }


  void ResourceDatabase::readScriptDatabase(pugi::xml_node& root) {
    if (auto scriptDB = root.child("ScriptDatabase")) {
      m_scriptDirectory = scriptDB.attribute("directory").as_string();

      for (auto script : scriptDB.children("ScriptEntry")) {
        if (auto type = script.attribute("lang")) {
          if (type.as_string() == "noct") {
#if defined(__NoctSys_UNIX__)
             NativeScriptPtr ptr = std::make_shared<NativeScript>(m_scriptDirectory.string() + '/' + script.text().as_string() + ".noct");

#elif defined(__NoctSys_Windows__)
            NativeScriptPtr ptr = std::make_shared<NativeScript>(m_scriptDirectory.string() + '\\' + script.text().as_string() + ".noct");
#endif
            m_nativeScripts[script.attribute("name").as_string()] = ptr;
          } else {
            throw ResourceError("'" + std::string(script.attribute("lang").as_string()) + "' scripting has not been implemented");
          }
        }
      }
    }
  }


  bool ResourceDatabase::loadTextureFromFile(const std::filesystem::path& path, const std::string_view& name) {
    std::lock_guard<std::mutex> lock(m_mutex);

    TextureResource res {
      path,
      std::make_shared<sf::Texture>()
    };

    sf::String setName(name.empty() ? path.stem().string() : name.data());

    if (res.data->loadFromFile(path.string())) {
      auto it = m_textures.find(setName);

      if (it != m_textures.end())
        it->second = res;

      else m_textures[setName] = res;
      
      return true;
    }

    return false;
  }


  bool ResourceDatabase::loadFontFromFile(const std::filesystem::path& path, const std::string_view& name) {
    std::lock_guard<std::mutex> lock(m_mutex);

    FontResource res {
      path,
      std::make_shared<sf::Font>()
    };

    sf::String setName(name.empty() ? path.stem().string() : name.data());

    if (res.data->openFromFile(path.string())) {
      auto it = m_fonts.find(setName);

      if (it != m_fonts.end())
        it->second = res;

      else m_fonts[setName] = res;
      
      return true;
    }

    return false;
  }


  bool ResourceDatabase::loadSoundFromFile(const std::filesystem::path& path, const std::string_view& name) {
    std::lock_guard<std::mutex> lock(m_mutex);

    SoundResource res {
      path,
      std::make_shared<sf::SoundBuffer>()
    };

    sf::String setName(name.empty() ? path.stem().string() : name.data());

    if (res.data->loadFromFile(path.string())) {
      auto it = m_sounds.find(setName);

      if (it != m_sounds.end()) 
        it->second = res;
      
      else m_sounds[setName] = res;
      
      return true;
    }

    return false;
  }


  bool ResourceDatabase::loadShaderFromFile(const std::filesystem::path& path, sf::Shader::Type type, const std::string_view& name) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!sf::Shader::isAvailable()) 
      throw ResourceError("sf::Shader is not supported by your GPU");

    ShaderResource res {
      false, 
      type,
      nullptr,
      path,
      std::make_shared<sf::Shader>()
    };

    sf::String setName(name.empty() ? path.stem().string() : name.data());

    if (res.data->loadFromFile(path.string(), type)) {
      auto it = m_shaders.find(setName);

      if (it != m_shaders.end())
        it->second = res;
      
      else m_shaders[setName] = res;
      
      return true;
    }

    return false;
  }


  bool ResourceDatabase::loadNativeScriptFromFile(const std::filesystem::path& path, const std::string_view& name) {
    std::lock_guard<std::mutex> lock    (m_mutex);
    NativeScriptPtr             script  (std::make_shared<NativeScript>());
    sf::String                  setName (name.empty() ? path.stem().string() : name.data());
    
    if (script->open(path)) {
      auto it = m_nativeScripts.find(setName);

      if (it != m_nativeScripts.end()) {
        it->second->close();
        it->second.swap(script);
      }
      else m_nativeScripts[setName] = script;

      return true;
    }

    return false;
  }


  void ResourceDatabase::addColor(const std::string_view& name, const sf::Color& color) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_colors.find(name.data());

    if (it != m_colors.end())
      it->second = color.toInteger();
    
    else m_colors[name.data()] = color.toInteger();
  }


  void ResourceDatabase::addColor(const sf::Color& color, const std::string_view& name) {
    addColor(name, color);
  }


  TexturePtr ResourceDatabase::findTexture(const std::string_view& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_textures.find(name.data());

    if (it != m_textures.end())
      return it->second.data;
    
    return {};
  }

  FontPtr ResourceDatabase::findFont(const std::string_view& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_fonts.find(name.data());

    if (it != m_fonts.end())
      return it->second.data;
    
    return {};
  }


  SoundBufferPtr ResourceDatabase::findSound(const std::string_view& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_sounds.find(name.data());

    if (it != m_sounds.end())
      return it->second.data;
    
    return {};
  }


  ShaderPtr ResourceDatabase::findShader(const std::string_view& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_shaders.find(name.data());

    if (it != m_shaders.end())
      return it->second.data;
    
    return {};
  }


  MaybeColor ResourceDatabase::findColor(const std::string_view& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_colors.find(name.data());

    if (it != m_colors.end())
      return std::make_optional<sf::Color>(it->second);
    
    return {};
  }


  NativeScriptPtr ResourceDatabase::findNativeScript(const std::string_view& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_nativeScripts.find(name.data());

    if (it != m_nativeScripts.end())
      return it->second;
  }


  bool ResourceDatabase::eraseTexture(const std::string_view& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
 
    if (m_textures.find(name.data()) != m_textures.end()) {
      m_textures.erase(name.data());
      return true;
    }

    return false;
  }


  bool ResourceDatabase::eraseFont(const std::string_view& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
 
    if (m_fonts.find(name.data()) != m_fonts.end()) {
      m_fonts.erase(name.data());
      return true;
    }

    return false;
  }


  bool ResourceDatabase::eraseSound(const std::string_view& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
 
    if (m_sounds.find(name.data()) != m_sounds.end()) {
      m_sounds.erase(name.data());
      return true;
    }

    return false;
  }


  bool ResourceDatabase::eraseShader(const std::string_view& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
 
    if (m_shaders.find(name.data()) != m_shaders.end()) {
      m_shaders.erase(name.data());
      return true;
    }

    return false;
  }


  bool ResourceDatabase::eraseColor(const std::string_view& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
 
    if (m_colors.find(name.data()) != m_colors.end()) {
      m_colors.erase(name.data());
      return true;
    }

    return false;
  }


  bool ResourceDatabase::eraseNativeScript(const std::string_view& name) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_nativeScripts.find(name.data()) != m_nativeScripts.end()) {
      m_nativeScripts.erase(name.data());
      return true;
    }

    return false;
  }


  StringVector ResourceDatabase::listTextures() {
    std::lock_guard<std::mutex> lock(m_mutex);
    StringVector list;

    for (const auto& entry : m_textures)
      list.push_back(entry.first);
    
    return list;
  }


  StringVector ResourceDatabase::listFonts() {
    std::lock_guard<std::mutex> lock(m_mutex);
    StringVector list;

    for (const auto& entry : m_fonts)
      list.push_back(entry.first);
    
    return list;
  }


  StringVector ResourceDatabase::listSounds() {
    std::lock_guard<std::mutex> lock(m_mutex);
    StringVector list;

    for (const auto& entry : m_sounds)
      list.push_back(entry.first);
    
    return list;
  }


  StringVector ResourceDatabase::listShaders() {
    std::lock_guard<std::mutex> lock(m_mutex);
    StringVector list;

    for (const auto& entry : m_shaders)
      list.push_back(entry.first);
    
    return list;
  }


  StringVector ResourceDatabase::listColors() {
    std::lock_guard<std::mutex> lock(m_mutex);
    StringVector list;

    for (const auto& entry : m_colors)
      list.push_back(entry.first);
    
    return list;
  }


  StringVector ResourceDatabase::listNativeScripts() {
    std::lock_guard<std::mutex> lock(m_mutex);
    StringVector list;

    for (const auto& entry : m_nativeScripts) 
      list.push_back(entry.first);
    
    return list;
  }


  void ResourceDatabase::setTextureDirectory(const std::filesystem::path& directory) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!std::filesystem::is_directory(directory))
      throw ResourceError(directory, "not a directory");

    m_textureDirectory = directory;
  }


  void ResourceDatabase::setFontDirectory(const std::filesystem::path& directory) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!std::filesystem::is_directory(directory))
      throw ResourceError(directory, "not a directory");
      
    m_fontDirectory = directory;
  }


  void ResourceDatabase::setSoundDirectory(const std::filesystem::path& directory) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!std::filesystem::is_directory(directory))
      throw ResourceError(directory, "not a directory");
      
    m_soundDirectory = directory;
  }


  void ResourceDatabase::setShaderDirectory(const std::filesystem::path& directory) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!std::filesystem::is_directory(directory))
      throw ResourceError(directory, "not a directory");
      
    m_shaderDirectory = directory;
  }


  void ResourceDatabase::setScriptDirectory(const std::filesystem::path& directory) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!std::filesystem::is_directory(directory))
      throw ResourceError(directory, "not a directory");

    m_scriptDirectory = directory;
  }

}