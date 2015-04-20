//
//  ConfigManager.cpp
//  komorki
//
//  Created by ttohin on 14.04.15.
//
//

#include "ConfigManager.h"
#include "cocos2d.h"
#include "tinydir.h"
#include "cocostudio/DictionaryHelper.h"
#include "json/prettyWriter.h"
#include "json/stringbuffer.h"

USING_NS_CC;
using namespace cocostudio;
using namespace komorki;

namespace
{
  bool ReadCellConfig(const rapidjson::Value& root, const std::string& key, PixelDescriptorProvider::Config::CellConfig& cellConfig)
  {
    const rapidjson::Value& cellConfigValue = DICTOOL->getSubDictionary_json(root, key.c_str());
    if (!cellConfigValue.IsNull())
    {
      cellConfig.health = DICTOOL->getIntValue_json(cellConfigValue, "health", cellConfig.health);
      cellConfig.sleepTime = DICTOOL->getIntValue_json(cellConfigValue, "sleepTime", cellConfig.sleepTime);
      cellConfig.attack = DICTOOL->getIntValue_json(cellConfigValue, "attack", cellConfig.attack);
      cellConfig.passiveHealthChunkMin = DICTOOL->getIntValue_json(cellConfigValue, "passiveHealthChunkMin", cellConfig.passiveHealthChunkMin);
      cellConfig.passiveHealthChunkMax = DICTOOL->getIntValue_json(cellConfigValue, "passiveHealthChunkMax", cellConfig.passiveHealthChunkMax);
      cellConfig.healthPerAttack = DICTOOL->getIntValue_json(cellConfigValue, "healthPerAttack", cellConfig.healthPerAttack);
      cellConfig.armor = DICTOOL->getIntValue_json(cellConfigValue, "armor", cellConfig.armor);
      cellConfig.lifeTime = DICTOOL->getIntValue_json(cellConfigValue, "lifeTime", cellConfig.lifeTime);
      cellConfig.percentOfMutations = DICTOOL->getFloatValue_json(cellConfigValue, "percentOfMutations", cellConfig.percentOfMutations);
    }
    else
    {
      return false;
    }
    
    return true;
  }
  
  void WriteCellConfig(const PixelDescriptorProvider::Config::CellConfig& cellConfig,
                       rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
  {
    writer.StartObject();
    writer.String("health");
    writer.Int(cellConfig.health);
    writer.String("sleepTime");
    writer.Int(cellConfig.sleepTime);
    writer.String("attack");
    writer.Int(cellConfig.attack);
    writer.String("passiveHealthChunkMin");
    writer.Int(cellConfig.passiveHealthChunkMin);
    writer.String("passiveHealthChunkMax");
    writer.Int(cellConfig.passiveHealthChunkMax);
    writer.String("healthPerAttack");
    writer.Int(cellConfig.healthPerAttack);
    writer.String("armor");
    writer.Int(cellConfig.armor);
    writer.String("lifeTime");
    writer.Int(cellConfig.lifeTime);
    writer.String("percentOfMutations");
    writer.Double(cellConfig.percentOfMutations);
    writer.EndObject();
  }
  
  bool WriteConfigToFile(const PixelDescriptorProvider::Config& config, const std::string& filePath)
  {
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    writer.StartObject();
    writer.String("map");
    {
      writer.StartObject();
      writer.String("maxTerrainSize");
      writer.Int(config.terrainSize);
      writer.String("width");
      writer.Int(config.mapWidth);
      writer.String("height");
      writer.Int(config.mapHeight);
      writer.EndObject();
    }
    
    writer.String("greenCell");
    WriteCellConfig(config.green, writer);
    writer.String("orangeCell");
    WriteCellConfig(config.orange, writer);
    writer.String("saladCell");
    WriteCellConfig(config.salad, writer);
    writer.String("cyanCell");
    WriteCellConfig(config.cyan, writer);
    
    writer.String("percentOfCreatures");
    writer.Double(config.percentOfCreatures);
    writer.String("percentOfOrange");
    writer.Double(config.percentOfOrange);
    writer.String("percentOfGreen");
    writer.Double(config.percentOfGreen);
    writer.String("percentOfSalad");
    writer.Double(config.percentOfSalad);
    writer.String("percentOfCyan");
    writer.Double(config.percentOfCyan);
    
    writer.EndObject();
  
    std::string result = buffer.GetString();
  
    FILE* fp = fopen(filePath.c_str(), "wb");
    if (fp)
    {
      size_t ret = fwrite(result.c_str(), 1, result.length(), fp);
      if (ret == 0)
      {
        log("fwrite function returned zero value");
        fclose(fp);
        return false;
      }
      
      fclose(fp);
    }
    else
    {
      log("Could not open file to write [%s]", filePath.c_str());
      return false;
    }
  
    return true;
  }
}

ConfigManager::ConfigManager()
: m_nextConfigNumber(1)
, m_configChanged(false)
{

}

ConfigManager::ConfigNamesList ConfigManager::GetConfigs()
{
  ConfigNamesList configNames;
  
  auto sharedFileUtils = FileUtils::getInstance();
  std::string dir = sharedFileUtils->getWritablePath();
  log("writabePath: %s", dir.c_str());
  
  std::string configDir = GetConfigDir();
  
  bool ok = sharedFileUtils->createDirectory(configDir);
  if (ok && sharedFileUtils->isDirectoryExist(configDir))
  {
    tinydir_dir dir;
    int i;
    if( 0 != tinydir_open_sorted(&dir, configDir.c_str()))
    {
      log("Could not open directory %s", configDir.c_str());
      return ConfigNamesList();
    }
    
    for (i = 0; i < dir.n_files; i++)
    {
      tinydir_file file;
      tinydir_readfile_n(&dir, &file, i);
      
      std::string fileName = file.name;
      
      if (!file.is_dir)
      {
        log("file in [%s][%s]",configDir.c_str(), fileName.c_str());
        size_t pos = fileName.find_last_of(".json");
        if (pos != std::string::npos)
        {
          ConfigPtr configPtr = ReadConfig(fileName);
          if (configPtr)
          {
            Config config {configPtr, fileName};
            m_configList.push_back(config);
            configNames.push_back(fileName);
            
            int number = 0;
            char buffer[256];
            int result = std::sscanf(fileName.c_str(), "%d_%s.json", &number, buffer);
            if (result != 0)
            {
              m_nextConfigNumber = std::max(number + 1, m_nextConfigNumber);
            }
            log("[%s] %d", fileName.c_str(), result);
          }
        }
      }
    }
    
    tinydir_close(&dir);
  }
  
  return configNames;
}

ConfigPtr ConfigManager::ReadConfig(const std::string& configName)
{
  std::string configDir = GetConfigDir();
  std::string contentStr = FileUtils::getInstance()->getStringFromFile(configDir + configName);
  
  rapidjson::Document doc;
  doc.Parse<0>(contentStr.c_str());
  if (doc.HasParseError())
  {
    log("Could not parse config [%s]", configName.c_str());
    return nullptr;
  }
  
  ConfigPtr config = std::make_shared<PixelDescriptorProvider::Config>();
 
  const rapidjson::Value& map = DICTOOL->getSubDictionary_json(doc, "map");
  if (!map.IsNull())
  {
    config->terrainSize = DICTOOL->getIntValue_json(map, "maxTerrainSize", config->terrainSize);
    config->mapWidth = DICTOOL->getIntValue_json(map, "width", config->mapWidth);
    config->mapHeight = DICTOOL->getIntValue_json(map, "height", config->mapHeight);
  }
  
  if (!ReadCellConfig(doc, "greenCell", config->green))
  {
    log("Could not find secttion greenCell in cofig %s", configName.c_str());
  }
  if (!ReadCellConfig(doc, "orangeCell", config->orange))
  {
    log("Could not find secttion orangeCell in cofig %s", configName.c_str());
  }
  if (!ReadCellConfig(doc, "saladCell", config->salad))
  {
    log("Could not find secttion saladCell in cofig %s", configName.c_str());
  }
  if (!ReadCellConfig(doc, "cyanCell", config->cyan))
  {
    log("Could not find secttion cyanCell in cofig %s", configName.c_str());
  }
  
  config->percentOfCreatures = DICTOOL->getFloatValue_json(doc, "percentOfCreatures", config->percentOfCreatures);
  config->percentOfOrange = DICTOOL->getFloatValue_json(doc, "percentOfOrange", config->percentOfOrange);
  config->percentOfGreen = DICTOOL->getFloatValue_json(doc, "percentOfGreen", config->percentOfGreen);
  config->percentOfSalad = DICTOOL->getFloatValue_json(doc, "percentOfSalad", config->percentOfSalad);
  config->percentOfCyan = DICTOOL->getFloatValue_json(doc, "percentOfCyan", config->percentOfCyan);

  return config;
}

void ConfigManager::PendingConfigChanged()
{
  m_configChanged = true;
}

bool ConfigManager::LoadConfig(const std::string& configName)
{
  ConfigPtr config = ReadConfig(configName);
  if (config)
  {
    *m_pendingConfig.config.get() = *config.get();
    m_pendingConfig.name = configName;
    m_configChanged = false;
    return true;
  }
  
  return false;
}

ConfigPtr ConfigManager::GetCurrentConfig()
{
  return m_currentConfig.config;
}

bool ConfigManager::SetConfig(const ConfigPtr& config)
{
  m_currentConfig.config = config;
  return true;
}

bool ConfigManager::ResetConfig()
{
  PixelDescriptorProvider::Config defaultConfig;
  if (m_currentConfig.config)
  {
    *m_currentConfig.config.get() = defaultConfig;
    return true;
  }
  
  return false;
}

bool ConfigManager::RemoveAllConfigs()
{
  return false;
}

bool ConfigManager::SavePendingConfig()
{
  if (m_pendingConfig.config && !m_pendingConfig.name.empty())
  {
    m_configChanged = false;
    
    std::string configDir = GetConfigDir();
    if (!WriteConfigToFile(*(m_pendingConfig.config.get()), configDir + m_pendingConfig.name))
    {
      log("Could not write config to file [%s]", m_pendingConfig.name.c_str());
      return false;
    }
    
    return true;
  }
  
  return false;
}

bool ConfigManager::SavePendingConfig(const std::string& name)
{
  std::string configDir = GetConfigDir();
  m_configChanged = false;
  
  if (!WriteConfigToFile(*(m_pendingConfig.config.get()), configDir + name))
  {
    log("Could not write config to file [%s]", name.c_str());
    return false;
  }
  
  m_pendingConfig.name = name;
  
  return true;
}

bool ConfigManager::SavePendingConfigToNewFile()
{
  timeval t;
  gettimeofday(&t, 0);
  
  int hour = t.tv_sec / (60 * 60) % 24;
  int minute = t.tv_sec / (60) % 60;
  int sec = t.tv_sec % 60;
  
  std::string newConfigName = std::to_string(m_nextConfigNumber++) + "_" +
  std::to_string(hour) + "_" + std::to_string(minute) + "_" + std::to_string(sec) +
  ".json";
  
  SavePendingConfig(newConfigName);
  
  return true;
}

bool ConfigManager::RemoveConfig(const std::string& name)
{
  std::string filePath = GetConfigDir() + name;
  FileUtils::getInstance()->removeFile(filePath);
  
  if (m_pendingConfig.name.compare(name) == 0)
  {
    m_pendingConfig.name = std::string();
  }
  
  return true;
}

bool ConfigManager::CreateNewConfig()
{
  m_currentConfig.config = std::make_shared<PixelDescriptorProvider::Config>();
  m_currentConfig.name = std::string();
  return true;
}

bool ConfigManager::CreatePendingConfig()
{
  m_pendingConfig.config = std::make_shared<PixelDescriptorProvider::Config>();
  if (m_currentConfig.config)
  {
    *m_pendingConfig.config.get() = *m_currentConfig.config.get();
    m_pendingConfig.name = m_currentConfig.name;
  }
  return true;
}

ConfigPtr ConfigManager::GetPendingConfig()
{
  return m_pendingConfig.config;
}

bool ConfigManager::CanSaveConfig()
{
  if (m_pendingConfig.config && !m_pendingConfig.name.empty() && m_configChanged)
  {
    return true;
  }
  
  return false;
}

bool ConfigManager::ApplyPendingConfig()
{
  m_currentConfig = m_pendingConfig;
  m_pendingConfig.config = nullptr;
  return true;
}

ConfigManager* ConfigManager::GetInstance()
{
  static ConfigManager* instance = nullptr;
  if (instance == nullptr)
  {
    instance = new ConfigManager();
  }
  
  return instance;
}

std::string ConfigManager::GetConfigDir()
{
  auto sharedFileUtils = FileUtils::getInstance();
  std::string dir = sharedFileUtils->getWritablePath();
  log("writabePath: %s", dir.c_str());
  
  std::string configDir = dir + "Komorki" + "/" + "configs" + "/";
  return configDir;
}

std::string ConfigManager::GetPendingConfigName()
{
  return m_pendingConfig.name;
}

bool ConfigManager::IsPendingConfigChanged()
{
  return m_configChanged;
}

