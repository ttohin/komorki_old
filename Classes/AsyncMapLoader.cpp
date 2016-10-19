//
//  AsyncMapLoader.cpp
//  prsv
//
//  Created by ttohin on 12.09.15.
//
//

#include "AsyncMapLoader.h"
#include "ConfigManager.h"
#include "TestPixelProvider.h"

USING_NS_CC;

AsyncMapLoader::AsyncMapLoader()
: m_config()
, m_performUpdate(false)
, m_shouldStop(false)
, m_inProccess(true)
, m_lastUpdateDuration(0)
, m_nuberOfUpdates(0)
, m_updateId(0)
{
  m_thread = std::thread(&AsyncMapLoader::WorkerThread, this);
}

AsyncMapLoader::~AsyncMapLoader()
{
  m_thread.join();
}

void AsyncMapLoader::WorkerThread()
{
  SetCurrentJobString("Creating provider");
  
#ifdef USE_TEST_PROVIDER
  m_provider = std::make_shared<komorki::TestPixelProvider>();
#else
  komorki::ConfigManager::GetInstance()->CreateNewConfig();
  m_provider = std::make_shared<komorki::PixelDescriptorProvider>();
  m_provider->InitWithConfig(komorki::ConfigManager::GetInstance()->GetCurrentConfig().get());
#endif
  
  SetCurrentJobString("Runnig few generations");
  std::list<komorki::PixelDescriptorProvider::UpdateResult> result;
  for (int i = 0; i < 0; i++)
  {
    m_provider->Update(false, result);
  }
  
  std::lock_guard<std::mutex> lk(m_lock);
  m_inProccess = false;
}

bool AsyncMapLoader::IsAvailable()
{
  std::lock_guard<std::mutex> lk(m_lock);
  return !m_inProccess;
}


std::string AsyncMapLoader::GetCurrentJobString()
{
  std::lock_guard<std::mutex> lk(m_lock);
  return m_currentJob;
}

komorki::ui::Viewport::Ptr AsyncMapLoader::GetViewport()
{
  return m_viewport;
}

void AsyncMapLoader::SetCurrentJobString(const std::string& text)
{
  std::lock_guard<std::mutex> lk(m_lock);
  m_currentJob = text;
}

std::shared_ptr<komorki::IPixelDescriptorProvider> AsyncMapLoader::GetProvider()
{
  return m_provider;
}
