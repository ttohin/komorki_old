//
//  TestPixelProvider.h
//  prsv
//
//  Created by Anton Simakov on 17.07.16.
//
//

#ifndef __prsv__TestPixelProvider__
#define __prsv__TestPixelProvider__

#include "PixelDescriptorProvider.h"

namespace komorki
{
  class TestPixelProvider : public PixelDescriptorProvider
  {
  public:
    typedef std::function<void(char symbol, const Vec2& pos)> PerMapSymbolFunc;
    
    TestPixelProvider();
    bool LoadMap();
    void ForEachSymbolInMap(const PerMapSymbolFunc& func);
    void PrintAsciiArt() const;
    
    // overrides
    virtual void GenTerrain() override;
    virtual void PopulateCells() override;
    virtual void GenLights() override;
    
  private:
    std::string m_stringMap;
    Vec2 m_mapSize;
    Config m_testConfig;
  };
}

#endif /* defined(__prsv__TestPixelProvider__) */
