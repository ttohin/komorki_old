#ifndef PIXELDESCRIPTORPROVIDER_H_69TPCKTG
#define PIXELDESCRIPTORPROVIDER_H_69TPCKTG

#include <vector>

#include "IPixelDescriptorProvider.h"
#include "PixelDescriptor.h"
#include <memory>
#include <map>

namespace komorki
{
  enum CellType {
    eCellTypeStart = 0,
    eCellTypeGreen = eCellTypeStart,
    eCellTypeSalat,
    eCellTypeHunter,
    eCellTypeImprovedSalat,
    eCellTypeEnd
  };

class PixelDescriptorProvider : public IPixelDescriptorProvider
{
public:
  
  class Config
  {
  public:
    int terrainSize;
    int mapHeight;
    int mapWidth;
    
    int greenHealth;
    int hunterHealth;
    int baseHealth;
    
    int hunterHealthIncome;
    int hunterAttack;
    int hunterHungryDamage;
    
    int greenHealthIncome;
    int greenAttack;
    int greenHugryDamage;
    int greenSleepTime;
    
    float percentOfCreatures;
    float percentOfHunters;
    float percentOfGreen;
    float percentOfSalat;
    
    int salatIncomeMin;
    int salatIncomeMax;
    int imporvedSalatIncomeMin;
    int improvedSalatIncomeMax;
   
    int improvedSalatArmor;
    int salatArmor;
    int salatSleepTime;
    int maxLifeTime;
    int hunterLifeTime;
    int baseArmor;
    float percentOfMutations;
    
    Config();
  };
  
  typedef std::shared_ptr<PixelDescriptor> PixelPtr;
  typedef std::vector<std::vector<PixelPtr> > PixelMap;
  
  void InitWithDefaults();
  void InitWithMutations(Config* config);
  void InitWithHighMutations(Config* config);
  void InitWithConfig(Config* config);
  
  virtual IPixelDescriptor* GetDescriptor(komorki::PixelPos x, komorki::PixelPos y) const;
  virtual komorki::Vec2 GetSize() const;
  virtual void Update(bool passUpdateResult, std::list<UpdateResult>& result);
  virtual ~PixelDescriptorProvider () {};
  void ProcessMutation(PixelDescriptor* source, PixelDescriptor* destination);
  void ProcessImprovedSalat(PixelDescriptor* pd, komorki::Vec2 pos, komorki::Optional<komorki::Movement>& movement, komorki::Optional<komorki::Action>& action);
  void ProcessSalat(PixelDescriptor* pd, komorki::Vec2 pos, komorki::Optional<komorki::Movement>& movement, komorki::Optional<komorki::Action>& action);
  void ProcessGreenCreature(PixelDescriptor* pd, komorki::Vec2 pos, komorki::Optional<komorki::Movement>& movement, komorki::Optional<komorki::Action>& action);
  void ProcessHunterCreature(PixelDescriptor* pd, komorki::Vec2 pos, komorki::Optional<komorki::Movement>& movement, komorki::Optional<komorki::Action>& action);
  void KillAllCells();
  void KillCellAtPostiion(const Vec2& pos);
  void ProccessTransaction(bool passUpdateResult, std::list<UpdateResult>& result);
  bool CheckBounds(int x, int y);
  void SetCreatureType(const Vec2& pos, CellType type);
  PixelPtr CreateCell(CellType type);
  
  std::map<int, int> m_population;
  
private:
  int CountTypeAroundPosition(komorki::Vec2 pos, int character);
  PixelMap m_map;
  std::vector<std::vector<IPixelDescriptor::Type> > m_typeMap;
  
  
};
  
}

#endif
