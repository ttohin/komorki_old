#include "AppDelegate.h"
#include "PartialMapScene.h"

USING_NS_CC;

AppDelegate::AppDelegate(int argc, char *argv[])
: m_jsonConfigPath(NULL)
{
  if (argc > 2)
  {
    if (strcmp(argv[1], "-j") == 0)
    {
      m_jsonConfigPath = argv[2];
    }
  }
}

AppDelegate::~AppDelegate()
{
}

bool AppDelegate::applicationDidFinishLaunching()
{
  auto director = Director::getInstance();
  auto glview = director->getOpenGLView();
  if(!glview)
  {
    glview = GLViewImpl::createWithRect("Komorki 0.0.2", Rect(0, 0, 800, 600), 1.0);
    director->setOpenGLView(glview);
    
    Size glViewSize = glview->getFrameSize();
    glview->setFrameSize(glViewSize.width, glViewSize.height);
  }
  
  director->setAnimationInterval(1.0 / 60);
  auto scene = PartialMapScene::createScene(m_jsonConfigPath);
  director->runWithScene(scene);
  
  return true;
}

void AppDelegate::applicationDidEnterBackground()
{
    Director::getInstance()->stopAnimation();
}

void AppDelegate::applicationWillEnterForeground()
{
    Director::getInstance()->startAnimation();
}
