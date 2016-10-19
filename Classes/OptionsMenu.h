//
//  OptionsMenu.h
//  komorki
//
//  Created by ttohin on 12.04.15.
//
//

#ifndef __komorki__OptionsMenu__
#define __komorki__OptionsMenu__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "PixelDescriptorProvider.h"
#include "IFullScreenMenu.h"
#include "ListController.h"

class OptionsMenu : public ListController
{
public:
  
  typedef std::function<void(void)> OptionsMenuCallback;
 
  OptionsMenu();
  void Init(const OptionsMenuCallback& confirmCallback,
            const OptionsMenuCallback& cancelCallback,
            const OptionsMenuCallback& saveAsCallback,
            const OptionsMenuCallback& loadCallback);

  virtual ~OptionsMenu();
  
  // IFullScreenMenu
  virtual void ShowInView(cocos2d::Node* root);
  virtual void Hide();
  virtual void Resize(const cocos2d::Size& size);
  virtual void OnKeyRelease(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
  
private:
  
  void ResetToDefaults();
  void Confirm();
  void Cancel();
  void OnLeftConfirmationButtonPressed();
  void OnRightConfirmationButtonPressed();
  
  cocos2d::ui::Layout* CreateParameterView(const std::string& name,
                                           int digitTimit,
                                           std::function<int(int step)> proccessStep,
                                           std::function<void(cocos2d::ui::Text* label, int value)> changeLabel,
                                           std::function<void(cocos2d::ui::Text* valueText, const std::string& stringValue)>,
                                           std::function<void(const std::string& stringValue)>,
                                           const std::string& textureName = std::string());
  void AddParameterView(const std::string& text, int* value, int minValue, int maxValue,
                        const std::string& textureName = std::string());
  void AddParameterView(const std::string& text, float* value, int minValue, int maxValue,
                        const std::string& textureName = std::string());
  void AddParameterViewForSection(const std::string& sectionName,
                                  komorki::Config::CellConfig* config);
  void insertWidgetFromAnotherNode(cocos2d::ui::ListView* listView, cocos2d::ui::Widget* widget);
  void ConfigureConfirmationPanel(cocos2d::ui::Layout* confirmationPanel);
  void ConfigureControlPanel(cocos2d::ui::Layout* controlPanel);
  void UpdateView();
  void ConfigChanged();
  void addChildFromAnotherNode(Node* node);
  
  cocos2d::Node* m_rootNode;
  cocos2d::ui::Layout* m_mainLayer;
  cocos2d::ui::Layout* m_controlPanel;
  OptionsMenuCallback m_cancelCallback;
  OptionsMenuCallback m_confirmCallback;
  OptionsMenuCallback m_saveAsCallback;
  OptionsMenuCallback m_loadCallback;
  cocos2d::ui::Button* m_load;
  cocos2d::ui::Button* m_save;
  cocos2d::ui::Button* m_saveAs;
  cocos2d::ui::Button* m_reset;
  cocos2d::ui::TextField* m_hiddenTextField;
  cocos2d::ui::Text* m_editingTitle;
  cocos2d::ui::Layout* m_editingTitleBg;
  float m_scroolDirection;
  std::function<void()> m_cancelEditing;
  std::function<void()> m_applyEditing;
  
  typedef std::function<void(int step)> ChangeValueFunction;
  std::list<ChangeValueFunction> m_updateValueList;
};

#endif /* __komorki__OptionsMenu__(__komorki__OptionsMenu__) */
