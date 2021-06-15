#ifndef  _MAZE_SCENE_H
#define _MAZE_SCENE_H
#include "aStar.h"
#include "cocos2d.h"
USING_NS_CC;

#define SMILE_TAG  1
#define DRAW_TAG  2
#define FORBID_TAG 3
#define BOX_TAG 4
#define TIP_TAG 5
#define BLOCK 6
#define DEATH 7
#define DOWN 8
#define UP 9
#define D_TAG 10
#define DE_TAG 11
#define NOT_ACCESS_TILE 80

class MazeScene : public cocos2d::Layer
{
private:
	//瓦片地图的图层对象
	TMXLayer* m_mapLayer;
	//绘制路径的绘图节点对象
	DrawNode* m_draw;
	//屏幕的可见尺寸
	Size m_visibleSize;
	//地图数组指针
	mapNode** m_map;
	//寻路起点指针
	mapNode* m_origin;
	//寻路终点指针
	mapNode* m_destination;
	//判断是否可以在地图上移动
	bool m_isReadyToMove; 
	bool aflag;
	int floors;
	int stx, sty;
	string map;
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(MazeScene);
	//初始化地图数组
	void initMap();
	//控制精灵沿路径移动
	void moveOnPath(mapNode* tempNode);
	//A*算法主函数
	int aStar(mapNode** map, mapNode* origin, mapNode* destination);
	//添加屏幕触摸监听
	void addTouchListener();
	//定时器更新
	void update(float delta);
	void judge(int type);
	bool onTouchBegan(Touch *touch, Event *unused_event);
	void onTouchEnded(Touch *touch, Event *unused_event);
	void addKeyboardListerner();
	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event*event);
	void reboot(float t);
};
#endif

