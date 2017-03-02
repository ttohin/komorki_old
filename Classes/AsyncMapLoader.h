//
//  AsyncMapLoader.h
//  prsv
//
//  Created by ttohin on 12.09.15.
//
//

#ifndef __prsv__AsyncMapLoader__
#define __prsv__AsyncMapLoader__

#include "PixelDescriptorProvider.h"
#include "Viewport.h"

class AsyncMapLoader
{
public:
  AsyncMapLoader();
  ~AsyncMapLoader();
  
  void WorkerThread();
  bool IsAvailable();
  unsigned char GetUpdateId();
  std::string GetCurrentJobString();
  
  komorki::graphic::Viewport::Ptr GetViewport();
  std::shared_ptr<komorki::IPixelDescriptorProvider> GetProvider();
  
private:

  void SetCurrentJobString(const std::string& text);
  
  bool m_performUpdate;
  bool m_shouldStop;
  bool m_inProccess;
  double m_lastUpdateDuration;
  unsigned int m_nuberOfUpdates;
  unsigned char m_updateId;
  std::mutex m_lock;
  std::condition_variable m_semaphore;
  std::thread m_thread;
  std::shared_ptr<komorki::Config> m_config;
  std::shared_ptr<komorki::IPixelDescriptorProvider> m_provider;
  std::string m_currentJob;
  komorki::graphic::Viewport::Ptr m_viewport;
};

#endif /* defined(__prsv__AsyncMapLoader__) */
