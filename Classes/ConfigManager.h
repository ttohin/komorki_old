//
//  ConfigManager.h
//  komorki
//
//  Created by ttohin on 14.04.15.
//
//

#ifndef komorki_ConfigManager_h
#define komorki_ConfigManager_h

#include <list>
#include <string>
#include "PixelDescriptorProvider.h"
#include "PixelProviderConfig.h"

namespace komorki
{
typedef std::shared_ptr<Config> ConfigPtr;
  
class ConfigManager
{
public:
  static ConfigManager* GetInstance();
  static std::string GetConfigDir();
  typedef std::list<std::string> ConfigNamesList;
  ConfigManager();
  ConfigNamesList GetConfigs();
  bool LoadConfig(const std::string& configName);
  ConfigPtr GetCurrentConfig();
  bool SetConfig(const ConfigPtr& config);
  bool ResetPendingConfig();
  bool RemoveAllConfigs();
  bool SavePendingConfig();
  bool SavePendingConfig(const std::string& name);
  bool SavePendingConfigToNewFile();
  bool RemoveConfig(const std::string& name);
  bool CreateNewConfig();
  bool CreatePendingConfig();
  ConfigPtr GetPendingConfig();
  bool CanSaveConfig();
  bool ApplyPendingConfig();
  ConfigPtr ReadConfig(const std::string& configName);
  void PendingConfigChanged();
  bool IsPendingConfigChanged();
  std::string GetPendingConfigName();
  bool RequiredDoubleSize();
private:
  
  struct ConfigFile
  {
    ConfigPtr config;
    std::string name;
  };
  
  ConfigFile m_pendingConfig;
  ConfigFile m_currentConfig;
  
  typedef std::list<ConfigFile> ConfigsList;
  ConfigsList m_configList;
  bool m_configChanged;
  int m_nextConfigNumber;
};

}
#endif
