#ifndef __GAME_CLIENT_H__
#define __GAME_CLIENT_H__

#include "cocos2d.h"
#include "Tank.h"
#include "Brick.h"
#include "time.h"
USING_NS_CC;
using namespace cocos2d;

static int tankcount = 0;     // 记录当前坦克数
static int NET_TAG = 11111;   

class GameClient : public Scene
{
public:
	GameClient();
	~GameClient();

	CREATE_FUNC(GameClient);
	virtual bool init();
	static Scene* createScene();
	void createBackGround();
	void update(float delta);
	void AIcontrol();
	void drawBigBG(Vec2 position);
	void drawWbigBG(Vec2 position);
	int idcount;
	time_t newtank;
	time_t now;
	// 对网络传来的消息作出响应
	void addTank(int id, float x, float y, int dir, int kind);

	// 实现键盘回调
	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event); 
	void greboot();
	void gpause();
	void gresume();

	// get
	Tank* getTank() { return m_tank; };
	Vector<Tank*> getTankList() { return m_tankList; };

private:
	Vector<Brick*>  m_bgList;     // 背景块列表
	Vector<Wbrick*> m_wbgList;	  // 加强墙
	Vector<Tank*>   m_tankList;   // 坦克列表
	Tank*           m_tank;       // 主坦克
	Vector<Tank*>	m_drawList;   // 已绘制的坦克
	
	Tank*           m_maxTank[50];        // 允许链接客户数
	Vector<Tank*>   m_shouldFireList;     // 记录需要开火的坦克 - 处理接收到开火消息的坦克

	Vector<Bullet*> m_deleteBulletList;   // 删除子弹列表
	Vector<Ubullet*> m_deleteUbulletList;
	Vector<Brick*>  m_deleteBrickList;    // 删除砖块列表
	Vector<Wbrick*> m_deleteWbrickList;
	Vector<Tank*>   m_deleteTankList;     // 删除坦克列表
};

#endif