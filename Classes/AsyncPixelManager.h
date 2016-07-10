//
//  CreaturesLogic.h
//  komorki
//
//  Created by ttohin on 21.03.15.
//
//

#ifndef komorki_AsyncPixelManager_h
#define komorki_AsyncPixelManager_h

#include <thread>
#include <mutex>
#include <condition_variable>

#include "PixelDescriptorProvider.h"

namespace komorki
{
  class AsyncPixelManager
  {
  public:
    AsyncPixelManager(PixelDescriptorProvider* provider)
    : m_provider(provider)
    , m_performUpdate(false)
    , m_shouldStop(false)
    , m_inProccess(false)
    , m_lastUpdateDuration(0)
    , m_nuberOfUpdates(0)
    , m_updateId(0)
    {
      m_thread = std::thread(&AsyncPixelManager::WorkerThread, this);
    }
    
    ~AsyncPixelManager()
    {
      m_thread.join();
    }
    
    void WorkerThread()
    {
      while (1)
      {
        {
          std::unique_lock<std::mutex> lk(m_lock);
          m_semaphore.wait(lk, [this]
          {
            return m_nuberOfUpdates != 0 || m_shouldStop;
          });
          
          if (m_shouldStop)
          {
            return;
          }
          
          m_inProccess = true;
          m_performUpdate = true;
        }
        
        struct timeval tv;
        struct timeval start_tv;
        
        gettimeofday(&start_tv, NULL);
        
        m_lastUpdateDuration = 0.0;
        
        m_updateResult.clear();
       
        if (m_nuberOfUpdates == 1)
        {
          m_provider->Update(true, m_updateResult);
        }
        else
        {
          for (int i = 0; i < m_nuberOfUpdates; ++i)
          {
            m_provider->Update(false, m_updateResult);
          }
          
          assert(m_updateResult.size() == 0);
        }

        gettimeofday(&tv, NULL);
        double lastUpdateDuration = (tv.tv_sec - start_tv.tv_sec) + (tv.tv_usec - start_tv.tv_usec) / 1000000.0;
        
        {
          std::lock_guard<std::mutex> lk(m_lock);
          m_nuberOfUpdates = 0;
          m_inProccess = false;
          m_lastUpdateDuration = lastUpdateDuration;
          ++m_updateId;
        }
      }
    }
    
    bool IsAvailable()
    {
      std::lock_guard<std::mutex> lk(m_lock);
      return !m_inProccess;
    }
    
    double GetLastUpdateTime()
    {
      return m_lastUpdateDuration;
    }
    
    void StartUpdate(unsigned int numberOfUpdates)
    {
      std::lock_guard<std::mutex> lk(m_lock);
      m_nuberOfUpdates = numberOfUpdates;
      m_semaphore.notify_one();
    }
    
    std::list<PixelDescriptorProvider::UpdateResult>& GetUpdateResult()
    {
      return m_updateResult;
    }
    
    void Stop()
    {
      std::lock_guard<std::mutex> lk(m_lock);
      m_shouldStop = true;
      m_semaphore.notify_one();
    }
    
    unsigned char GetUpdateId()
    {
      return m_updateId;
    }
    
  private:
    PixelDescriptorProvider* m_provider;
    bool m_performUpdate;
    bool m_shouldStop;
    bool m_inProccess;
    double m_lastUpdateDuration;
    unsigned int m_nuberOfUpdates;
    unsigned char m_updateId;
    std::mutex m_lock;
    std::condition_variable m_semaphore;
    std::list<PixelDescriptorProvider::UpdateResult> m_updateResult;
    std::thread m_thread;
  };
}


#endif /* defined(komorki_AsyncPixelManager_h) */
