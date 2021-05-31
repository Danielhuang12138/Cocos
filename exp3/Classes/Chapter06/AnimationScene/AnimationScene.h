// 第6章例子2 -- 综合动画例子

#ifndef __ANIMATION_SCENE_H__
#define __ANIMATION_SCENE_H__

#include "config_set.h"
#include "Joystick.h"
#include "Hero.h"
#include "Enemy.h"
#include "MyContactListener.h"
#include "AIManager.h"
#include "chapter06/Billboard.h"
#include <stdio.h>

class AnimationScene : public Layer
{
public:
	AnimationScene();
	~AnimationScene();
	virtual bool init();
	CREATE_FUNC(AnimationScene);
	static Scene* createScene();
	void reboot(float t);
	void update(float delta);

	void attackCallback(Ref* pSender);
	void defendCallback(Ref* pSender);
	
private:
	Joystick* m_joystick;   // 摇杆
	MyContactListener* m_contactListener;  // 碰撞检测
	AIManager* m_aimanager;  // AI manager

	Hero* m_player;
	Enemy* m_enemy;

	Sprite* m_bloodBar_h;
	Sprite* m_bloodBar_e;
};

#endif