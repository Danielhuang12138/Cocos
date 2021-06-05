#include "MazeScene.h"

USING_NS_CC;

Scene* MazeScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = MazeScene::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool MazeScene::init()
{
	if (!Layer::init())
	{
		return false;
	}
	m_map = nullptr;
	//寻路起点指针
	m_origin = nullptr;
	//寻路终点指针
	m_destination = nullptr;
	//判断是否可以在地图上移动
	m_isReadyToMove = true;

	//获得屏幕尺寸
	m_visibleSize = Director::getInstance()->getVisibleSize();
	//载入地图，将放置到适当位置
	
	auto tileMap = TMXTiledMap::create("Chapter11/superMaze.tmx");
	tileMap->setPosition(0, m_visibleSize.height - MAP_HEIGHT * UNIT);
	this->addChild(tileMap, 1);
	//获取地图中的主图层
	m_mapLayer = tileMap->getLayer("mainLayer");
	//初始化地图数组
	initMap();
	//在屏幕底端中央显示闪烁的操作提示
	auto playTip = LabelTTF::create("Click Map To Put Treasure", "Arial", 40);
	playTip->setPosition(m_visibleSize.width / 2, 0.5*(m_visibleSize.height - MAP_HEIGHT * UNIT));
	this->addChild(playTip, 1);
	auto blink = Blink::create(5, 10);
	playTip->runAction(blink);
	//预置找到宝箱的提示，将其设置为不可见
	auto successTip = Label::createWithBMFont("fonts/futura-48.fnt", "Found Treasure!");
	successTip->setPosition(m_visibleSize.width / 2, m_visibleSize.height / 2);
	this->addChild(successTip, 2,TIP_TAG);
	successTip->setVisible(false);
	//在地图起点放置寻宝精灵
	auto smileSprite = Sprite::create("Chapter11/smile.png");
	smileSprite->setPosition(0.5*UNIT, m_visibleSize.height - 0.5*UNIT);
	this->addChild(smileSprite, 3, SMILE_TAG);
	//预置宝箱精灵，将其设置为不可见
	auto boxSprite = Sprite::create("Chapter11/box.png");
	boxSprite->setPosition(m_visibleSize.width / 2, m_visibleSize.height / 2 );
	boxSprite->setVisible(false);
	this->addChild(boxSprite, 3, BOX_TAG);
	//预置禁止放置图标，将其设置为不可见
	auto forbidSprite = Sprite::create("Chapter11/forbid.png");
	forbidSprite->setPosition(m_visibleSize.width / 2 , m_visibleSize.height / 2 );
	forbidSprite->setVisible(false);
	this->addChild(forbidSprite, 4, FORBID_TAG);
	//添加绘图节点
	m_draw = DrawNode::create();
	this->addChild(m_draw, 2, DRAW_TAG);
	//添加屏幕触摸监听
	addTouchListener();
	//调用定时器更新
	this->scheduleUpdate();
	return true;
}


void MazeScene::update(float delta)
{
	auto smile = this->getChildByTag(SMILE_TAG);
	auto box = this->getChildByTag(BOX_TAG);
	//如果笑脸精灵达到宝箱位置
	if ( smile->getBoundingBox().intersectsRect( box->getBoundingBox() ) )
	{
		//隐藏宝箱
		box->setVisible(false);
		//将地图数组恢复初始状态
		for (int i = 0; i< MAP_WIDTH; i++)
		{
			for (int j = 0; j< MAP_HEIGHT; j++)
			{
				if (m_map[i][j].status != NOT_ACCESS)
				{
					mapNode temp = { ACCESS, i, j, 0, 0, 0, nullptr };
					m_map[i][j] = temp;
				}			
			}
		}
		//将当前单元设为起点
		m_origin = m_destination;
		m_origin->status = ORIGIN;
		//清除绘制的路径
		m_draw->clear();
		//允许再次移动
		m_isReadyToMove = true;
		//显示寻路成功的标签提示
		auto tip = this->getChildByTag(TIP_TAG);
		tip->setVisible(true);
	}
}

void MazeScene::initMap()
{
	//根据地图宽、高分配数组空间
	m_map = new mapNode*[MAP_WIDTH];
	for (int n = 0; n < MAP_WIDTH; n++)
		m_map[n] = new mapNode[MAP_HEIGHT];
	//依次扫描地图数组每一个单元
	for (int i = 0; i< MAP_WIDTH; i++)
	{
		for (int j = 0; j< MAP_HEIGHT; j++)
		{
			//若当前位置为墙体瓦片设置为不可通过
			if (m_mapLayer->getTileGIDAt(Vec2(i, j)) == NOT_ACCESS_TILE)
			{
				mapNode temp={NOT_ACCESS, i, j, 0, 0, 0, nullptr };
				m_map[i][j] = temp;
			}
				
			//否则设置为可以通过
			else
			{
				mapNode temp={ ACCESS, i, j, 0, 0, 0, nullptr };
				m_map[i][j] = temp;
			}
		}
	}
	//初始化起点为（0，0）单元
	m_map[0][0].status = ORIGIN;
	m_origin = &m_map[0][0];
}

int MazeScene::aStar(mapNode** map, mapNode* origin, mapNode* destination)
{
	openList* open = new openList;
	open->next = nullptr;
	open->openNode = origin;
	closedList* close = new closedList;
	close->next = nullptr;
	close->closedNode = nullptr;
	//循环检验8个方向的相邻节点
	while (checkNeighboringNodes(map, open, open->openNode, destination))
	{
		//从OPEN表中选取节点插入CLOSED表
		insertNodeToClosedList(close, open);
		//若OPEN表为空，表明寻路失败
		if (open == nullptr)
		{
			//屏幕中央显示寻路失败的提示
			auto failedTip = Label::createWithBMFont("fonts/futura-48.fnt", "No Way!");
			failedTip->setPosition(m_visibleSize.width / 2, m_visibleSize.height / 2);
			this->addChild(failedTip, 2, TIP_TAG);
			break;
		}
		//若终点在OPEN表中，表明寻路成功
		if (open->openNode->status == DESTINATION)
		{
			mapNode* tempNode = open->openNode;
			//调用moveOnPath（）函数控制精灵在路径上移动
			moveOnPath(tempNode);
			break;
		}
	}
	return 0;
}

void MazeScene::moveOnPath(mapNode* tempNode)
{
	//声明存储路径坐标的结构体
	struct pathCoordinate { int x; int y; int type; };
	//分配路径坐标结构体数组
	pathCoordinate* path = new pathCoordinate[MAP_WIDTH*MAP_HEIGHT];
	//利用父节点信息逆序存储路径坐标
	int loopNum = 0;
	while (tempNode != nullptr)
	{
		path[loopNum].x = tempNode->xCoordinate;
		path[loopNum].y = tempNode->yCoordinate;
		loopNum++;
		tempNode = tempNode->parent;
	}
	//将笑脸精灵的坐标存为绘制线段起点
	auto smile = this->getChildByTag(SMILE_TAG);
	int fromX = smile->getPositionX();
	int fromY = smile->getPositionY();
	//危险区域检测
	int type = BLOCK;
	//声明动作向量存储动作序列
	Vector<FiniteTimeAction*> actionVector;
	//从结构体数组尾部开始扫描
	for (int j = loopNum - 2; j >= 0; j--)
	{
		//将地图数组坐标转化为屏幕实际坐标
		int realX = (path[j].x + 0.5)*UNIT;
		int realY = m_visibleSize.height - (path[j].y + 0.5)*UNIT;
		//创建移动动作并存入动作向量
		auto moveAction = MoveTo::create(0.2, Vec2(realX, realY));
		if (aflag) {
			actionVector.pushBack(moveAction);
		}
		if (path[j].type == DEATH || path[j].type == DOWN || path[j].type == UP) {
			if (aflag) {
				type = path[j].type;
			}
			if (path[j].type == DOWN) {
				if (floors == 1) {
					stx = path[j].x - 1;
				}
				else {
					stx = path[j].x;
				}
				sty = path[j].y;
			}
			else if (path[j].type == UP) {
				stx = path[j].x + 1;
				sty = path[j].y;
			}
			aflag = false;
		}
		//绘制从起点到下一个地图单元的线段
		m_draw->drawLine(Vec2(fromX, fromY), Vec2(realX, realY), Color4F(1.0, 1.0, 1.0, 1.0));
		//将当前坐标保存为下一次绘制的起点
		fromX = realX;
		fromY = realY;
	}
	if (type == DEATH) {
		auto caution = this->getChildByTag(D_TAG);
		caution->setVisible(true);
	}
	//创建动作序列
	auto actionSequence = Sequence::create(actionVector);
	//笑脸精灵执行移动动作序列
	smile->runAction(Sequence::create(actionSequence,CallFunc::create(CC_CALLBACK_0(MazeScene::judge,this,type)),NULL));
}

void MazeScene::addTouchListener()
{
	//获得监听器
	auto listener = EventListenerTouchOneByOne::create();
	//为触摸开始和触摸结束设置回调函数
	listener->onTouchBegan = CC_CALLBACK_2(MazeScene::onTouchBegan, this);
	listener->onTouchEnded = CC_CALLBACK_2(MazeScene::onTouchEnded, this);
	//设置触摸监听优先级
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

bool MazeScene::onTouchBegan(Touch *touch, Event *unused_event)
{
	//将触摸点坐标转化为地图数组坐标
	int x = (int)(touch->getLocation().x / UNIT);
	int y = (int)((m_visibleSize.height - touch->getLocation().y) / UNIT);
	//若该地图单元为墙体或超出地图范围，设置禁止图标
	if (m_map[x][y].status == NOT_ACCESS || y >= MAP_HEIGHT)
	{
		auto forbid = this->getChildByTag(FORBID_TAG);
		forbid->setPosition(0.5*UNIT + x*UNIT, m_visibleSize.height - 0.5*UNIT - y*UNIT);
		forbid->setVisible(true);
		return true;
	}
	else
	{
		//若该地图单元可到达且允许移动
		if (m_isReadyToMove)
		{
			//为到达终点前不再允许移动
			m_isReadyToMove = false;
			//将当前地图单元设置为终点
			m_map[x][y].status = DESTINATION;
			m_destination = &m_map[x][y];
			//在当前地图单元添加宝箱精灵
			auto box= this->getChildByTag(BOX_TAG);
			box->setPosition(0.5*UNIT + x*UNIT, m_visibleSize.height - 0.5*UNIT - y*UNIT);
			box->setVisible(true);
			//调用A*算法开始寻路
			aStar(m_map, m_origin, m_destination);
			//将上一次寻路成功的提示隐藏
			auto tip = this->getChildByTag(TIP_TAG);
			tip->setVisible(false);
			return false;
		}
		//若不允许移动，在该地图单元设置禁止图标
		else
		{
			auto forbid = this->getChildByTag(FORBID_TAG);
			forbid->setPosition(0.5*UNIT + x*UNIT, m_visibleSize.height - 0.5*UNIT - y*UNIT);
			forbid->setVisible(true);
			return true;
		}
	}
}

void MazeScene::onTouchEnded(Touch *touch, Event *unused_event)
{
	//隐藏禁止图标
	auto forbid = this->getChildByTag(FORBID_TAG);
	forbid->setVisible(false);
	//隐藏寻路成功提示
	auto tip = this->getChildByTag(TIP_TAG);
	tip->setVisible(false);
}

void MazeScene::judge(int type) {
	auto caution = this->getChildByTag(D_TAG);
	auto death = this->getChildByTag(DE_TAG);
	auto smile = this->getChildByTag(SMILE_TAG);
	auto box = this->getChildByTag(BOX_TAG);
	auto tip = this->getChildByTag(TIP_TAG);
	if (aflag == false) {
		if (type == DEATH) {

		}
	}
}