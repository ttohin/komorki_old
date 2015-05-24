//
//  TextButton.h
//  komorki
//
//  Created by ttohin on 25.04.15.
//
//

#ifndef komorki_TextButton_h
#define komorki_TextButton_h

#include "cocos2d.h"
#include "CocosGUI.h"
#include "ConfigManager.h"

namespace komorki
{
  class TextButton : public cocos2d::ui::Button
  {
  public:
    static TextButton* create(const std::string& normalImage,
                              const std::string& selectedImage = "",
                              const std::string& disableImage = "",
                              TextureResType texType = TextureResType::LOCAL)
    {
      TextButton *btn = new (std::nothrow) TextButton;
      if (btn && btn->init(normalImage,selectedImage,disableImage,texType))
      {
        btn->autorelease();
        return btn;
      }
      CC_SAFE_DELETE(btn);
      return nullptr;
    }
    
  protected:
    bool init(const std::string &normalImage,
              const std::string& selectedImage ,
              const std::string& disableImage,
              TextureResType texType)
    {
      if(cocos2d::ui::Button::init(normalImage, selectedImage, disableImage, texType))
      {
      }
      return false;
    }
  };
}

#endif
