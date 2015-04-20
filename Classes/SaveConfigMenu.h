//
//  SaveConfigMenu.h
//  komorki
//
//  Created by ttohin on 19.04.15.
//
//

#ifndef __komorki__SaveConfigMenu__
#define __komorki__SaveConfigMenu__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "PixelDescriptorProvider.h"
#include "ListController.h"

class SaveConfigMenu : public ListController
{
public:
  
  typedef std::function<void(void)> SaveConfigMenuCallback;
  
  void Init(const SaveConfigMenuCallback& savedCallback,
            const SaveConfigMenuCallback& cancelCallback);


  virtual ~SaveConfigMenu();
 
  // IFullScreenMenu
  virtual void ShowInView(cocos2d::Node* root);
  virtual void Hide();
  virtual void Resize(const cocos2d::Size& size);
  virtual void OnKeyRelease(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
  
private:
  
  void Cancel();
  void SaveConfig(const std::string& name);
  void RemoveConfig(const std::string& name);
  void SaveNewConfig();
  
  cocos2d::ui::Layout* CreateLoadConfigView(const std::string& name);
  cocos2d::ui::Layout* CreateNewSaveConfigView();
  void AddParameterViewForSection(const std::string& sectionName,
                                  komorki::PixelDescriptorProvider::Config::CellConfig* config,
                                  cocos2d::ui::ListView* list);
  void addChildFromAnotherNode(cocos2d::Node* node);
  void insertWidgetFromAnotherNode(cocos2d::ui::ListView* listView, cocos2d::ui::Widget* widget);
  void ConfigureConfirmationPanel(cocos2d::ui::Layout* confirmationPanel);
  
  cocos2d::Node* m_rootNode;
  cocos2d::ui::Layout* m_mainLayer;
  cocos2d::ui::Layout* m_confirmationPanel;
  cocos2d::ui::ListView* m_list;
  SaveConfigMenuCallback m_savedCallback;
  SaveConfigMenuCallback m_cancelCallback;
  cocos2d::ui::Button* m_cancel;
  cocos2d::ui::Button* m_load;
  cocos2d::ui::Text* m_title;
  bool m_configApplied;
  
  typedef std::function<void(int step)> ChangeValueFunction;
  std::list<ChangeValueFunction> m_updateValueList;
};

#endif /* __komorki__LoadConfigMenu__ */