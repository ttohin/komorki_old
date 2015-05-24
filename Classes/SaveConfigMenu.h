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
#include "ListView.h"

class SaveConfigMenu : public ListController
{
public:
  
  typedef std::function<void(void)> SaveConfigMenuCallback;
 
  SaveConfigMenu();
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
  void OnLeftConfirmationButtonPressed();
  void SaveConfig(const std::string& name);
  void RemoveConfig(const std::string& name);
  void SaveNewConfig();
  
  cocos2d::ui::Layout* CreateLoadConfigView(const std::string& name);
  cocos2d::ui::Layout* CreateNewSaveConfigView();
  void addChildFromAnotherNode(cocos2d::Node* node);
  void insertWidgetFromAnotherNode(cocos2d::ui::ListView* listView, cocos2d::ui::Widget* widget);
  
  cocos2d::Node* m_rootNode;
  cocos2d::ui::Layout* m_mainLayer;
  SaveConfigMenuCallback m_savedCallback;
  SaveConfigMenuCallback m_cancelCallback;
  cocos2d::ui::Button* m_load;
  bool m_configApplied;
  
  typedef std::function<void(int step)> ChangeValueFunction;
  std::list<ChangeValueFunction> m_updateValueList;
};

#endif /* __komorki__LoadConfigMenu__ */
