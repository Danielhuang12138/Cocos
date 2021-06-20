#include "GameClient.h"
#include <string>
GameClient::GameClient()
{

}

GameClient::~GameClient()
{

}

bool GameClient::init()
{
	playerlives = 3;
	now = time(&now);
	srand((unsigned int)time(NULL));
	if (!Scene::init())
	{
		return false;
	}
	idcount = 0;
	// 背景
	createBackGround();
	newtank = time(&newtank);
	roundcount = 25;
	// 玩家
	m_tank = Tank::create(11000,160, 570, 1, 2);
	m_tankList.pushBack(m_tank);

	// 碰撞检测
	this->scheduleUpdate();

	// 键盘事件
	auto key_listener = EventListenerKeyboard::create();
	key_listener->onKeyPressed = CC_CALLBACK_2(GameClient::onKeyPressed, this);
	key_listener->onKeyReleased = CC_CALLBACK_2(GameClient::onKeyReleased, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(key_listener, this);

	m_shouldFireList.clear(); 
	m_pause = NULL;
	std::string s1 = "TOTAL KILL: ";
	std::string s2 = std::to_string(m_tank->killcount);
	std::string s4 = "/24";
	std::string s3 = s1 + s2+s4;
	m_killcount = Label::createWithSystemFont(s3, "System", 25);
	m_killcount->setTextColor(Color4B(255, 255, 255, 255));
	m_killcount->setPosition(Vec2(1080, 580));
	this->addChild(m_killcount, 10);
	s1 = "BOOST IN:";
	s2 = std::to_string(20 - (now - m_tank->boost));
	s3 = s1 + s2;
	m_boostct = Label::createWithSystemFont(s3, "System", 25);
	m_boostct->setTextColor(Color4B(255, 255, 255, 255));
	m_boostct->setPosition(Vec2(1080, 550));
	this->addChild(m_boostct, 10);
	m_ammo = Label::createWithSystemFont("AMMO: APC", "System", 25);
	m_ammo->setTextColor(Color4B(255, 255, 255, 255));
	m_ammo->setPosition(Vec2(1080, 520));
	this->addChild(m_ammo, 10);
	m_reload = Label::createWithSystemFont("GUN RELOADING", "System", 25);
	if (now - m_tank->cool < 2) {
		s1 = "RELOADING ";
		s2 = std::to_string(2 - (now - m_tank->cool));
		s3 = " S";
		s4 = s1 + s2 + s3;
		m_reload = Label::createWithSystemFont(s4, "System", 25);
		m_reload->setTextColor(Color4B(255, 255, 255, 255));
		m_reload->setPosition(Vec2(1080, 490));
		m_reload->enableShadow(Color4B(255, 255, 0, 255), Size(2, 2));
	}
	else {
		m_reload = Label::createWithSystemFont("GUN RELOADED!", "System", 25);
		m_reload->setTextColor(Color4B(255, 255, 255, 255));
		m_reload->setPosition(Vec2(1080, 490));
		m_reload->enableShadow(Color4B(0, 255, 0, 255), Size(2, 2));
	}
	this->addChild(m_reload, 10);
	m_lives = Label::createWithSystemFont("Lives: 3", "System", 25);
	m_lives->setTextColor(Color4B(255, 255, 255, 255));
	m_lives->setPosition(Vec2(1080, 460));
	m_lives->enableShadow(Color4B(0, 255, 0, 255), Size(2, 2));
	this->addChild(m_lives, 10);
	return true;
}

Scene* GameClient::createScene()
{
	auto scene = Scene::create();
	auto layer = GameClient::create();
	scene->addChild(layer);
	return scene;
}

void GameClient::update(float delta)
{
	AIcontrol();
	Display();
	now = time(&now);
	int posx = (int)(CCRANDOM_0_1() * 6) % 5;
	int posy = (int)(CCRANDOM_0_1() * 4) % 3;
	if (now - newtank > 2&&roundcount-m_tank->killcount> 1&&m_tankList.size()<5) {
		addTank(idcount, 930-(posx*2*16), 32+posy*2*16, 1, 1);
		idcount++;
		newtank = now;
	}
	// 收到传来的开火消息的坦克执行Fire
	if (m_shouldFireList.size() > 0)
	{
		auto tank = m_shouldFireList.at(0);
		tank->Fire();
		m_shouldFireList.clear();
	}

	// 维护坦克列表
	for (int i = 0;i < m_tankList.size(); i++)
	{
		auto nowTank = m_tankList.at(i);
		if (nowTank->getLife() <= 0)
		{
			m_tankList.eraseObject(nowTank);
		}
		bool notDraw = true;
		for (int j = 0;j < m_drawList.size(); j ++)
		{
			auto drawTank = m_drawList.at(j);
			if (drawTank->getID() == nowTank->getID() )
			{
				notDraw = false;
			}
		}

		// 绘制尚未绘制的坦克-针对后连进来的客户端
		if (notDraw)  
		{
			this->addChild(nowTank);
			m_drawList.pushBack(nowTank);
		}
	}

	auto playtank = m_tankList.at(0);
	if (playtank->upcount > 5) {
		playtank->fireupgra();
	}
	if (playtank->isBoost) {
		playtank->now = time(&playtank->now);
		if (playtank->now - 5 > playtank->boost) {
			playtank->EndBoost();
		}
	}
	if (playtank->getfireup()) {
		for (int j = 0; j < playtank->getUbulletList().size(); j++) {
			auto bullet = playtank->getUbulletList().at(j);
			for (int k = 1; k < m_tankList.size(); k++) {
				auto tank_another = m_tankList.at(k);
				if (playtank->getID() != tank_another->getID()) {
					if (bullet->getRect().intersectsRect(tank_another->getRect())) {
						m_deleteUbulletList.pushBack(bullet);
						m_deleteTankList.pushBack(tank_another);
						playtank->killcount++;
						playtank->upcount++;
					}
				}
				for (int l = 0; l < tank_another->getBulletList().size(); l++) {
					auto abullet = tank_another->getBulletList().at(l);
					if (bullet->getRect().intersectsRect(abullet->getRect())) {
						m_deleteUbulletList.pushBack(bullet);
						m_deleteBulletList.pushBack(abullet);
					}
				}
			}
		}

		for (int j = 0; j < playtank->getUbulletList().size(); j++) {
			auto bullet = playtank->getUbulletList().at(j);
			if (bullet->getPositionX() > 955) {
				m_deleteUbulletList.pushBack(bullet);
			}
			if (bullet->getRect().intersectsRect(m_core->getRect())&&m_core->getLife()>0) {
				m_deleteUbulletList.pushBack(bullet);
				m_core->Blast();
				GameOver();
				m_deleteTankList.pushBack(m_tank);
			}
		}

		for (int j = 0; j < playtank->getUbulletList().size(); j++) {
			auto bullet = playtank->getUbulletList().at(j);
			for (int k = 0; k < m_bgList.size(); k++) {
				auto brick = m_bgList.at(k);
				
				if (bullet->getRect().intersectsRect(brick->getRect())) {
					m_deleteUbulletList.pushBack(bullet);
					m_deleteBrickList.pushBack(brick);
				}
				
			}
			for (int k = 0; k < m_wbgList.size(); k++) {
				auto wbrick = m_wbgList.at(k);
				if (bullet->getRect().intersectsRect(wbrick->getRect())) {
					m_deleteUbulletList.pushBack(bullet);
					m_deleteWbrickList.pushBack(wbrick);
				}
			}
		}
	}

	// 坦克与 坦克，物品的碰撞检测
	for (int i = 0;i < m_tankList.size(); i++)
	{
		auto c_tank = m_tankList.at(i);
		for (int j = 0; j < c_tank->getBulletList().size(); j++) {
			auto cbullet = c_tank->getBulletList().at(j);
			if (cbullet->getRect().intersectsRect(m_core->getRect())&&m_core->getLife()>0) {
				GameOver();
				m_core->Blast();
				m_deleteTankList.pushBack(m_tank);
			}
		}
		for (int j = 0; j < c_tank->getBulletList().size(); j++) {
			auto cbullet = c_tank->getBulletList().at(j);
			if (cbullet->getPositionX() > 955) {
				m_deleteBulletList.pushBack(cbullet);
			}
		}
		for (int j = 0; j < m_bgList.size(); j++)
		{
			auto nowTank = m_tankList.at(i);
			auto nowBrick = m_bgList.at(j);
			if (nowTank->getLife() && (nowTank->getRect().intersectsRect(nowBrick->getRect())) && (nowTank->getDirection() == TANK_UP))
			{
				// 方法1：履带持续转动
				nowTank->setHindered(TANK_UP);
				nowTank->setPositionY(nowTank->getPositionY() - 1); // 避免检测成功后坦克持续受，无法行动造成卡住

				// 方法2：履带停止转动
				// nowTank->Stay(TANK_UP);
			}
			if (nowTank->getLife() && (nowTank->getRect().intersectsRect(nowBrick->getRect())) && (nowTank->getDirection() == TANK_DOWN))
			{
				// 方法1：履带持续转动
				nowTank->setHindered(TANK_DOWN); 
				nowTank->setPositionY(nowTank->getPositionY() + 1); // 避免检测成功后坦克持续受，无法行动造成卡住

				// 方法2：履带停止转动
				// nowTank->Stay(TANK_DOWN);
			}
			if (nowTank->getLife() && (nowTank->getRect().intersectsRect(nowBrick->getRect())) && (nowTank->getDirection() == TANK_LEFT))
			{
				// 方法1：履带持续转动
				nowTank->setHindered(TANK_LEFT); 
				nowTank->setPositionX(nowTank->getPositionX() + 1); // 避免检测成功后坦克持续受，无法行动造成卡住

				// 方法2：履带停止转动
				// nowTank->Stay(TANK_LEFT);
			}
			if (nowTank->getLife() && (nowTank->getRect().intersectsRect(nowBrick->getRect())) && (nowTank->getDirection() == TANK_RIGHT) || nowTank->getPositionX() > 910)
			{
				// 方法1：履带持续转动
				nowTank->setHindered(TANK_RIGHT); 
				nowTank->setPositionX(nowTank->getPositionX() - 1); // 避免检测成功后坦克持续受，无法行动造成卡住

				// 方法2：履带停止转动
				// nowTank->Stay(TANK_RIGHT);
			}
		}
		for (int j = 0; j < m_wbgList.size(); j++)
		{
			auto nowWbrick = m_wbgList.at(j);
			auto nowTank = m_tankList.at(i);
			if (nowTank->getLife() && (nowTank->getRect().intersectsRect(nowWbrick->getRect())) && (nowTank->getDirection() == TANK_UP))
			{
				// 方法1：履带持续转动
				nowTank->setHindered(TANK_UP);
				nowTank->setPositionY(nowTank->getPositionY() - 1); // 避免检测成功后坦克持续受，无法行动造成卡住

				// 方法2：履带停止转动
				// nowTank->Stay(TANK_UP);
			}
			if (nowTank->getLife() && (nowTank->getRect().intersectsRect(nowWbrick->getRect())) && (nowTank->getDirection() == TANK_DOWN))
			{
				// 方法1：履带持续转动
				nowTank->setHindered(TANK_DOWN);
				nowTank->setPositionY(nowTank->getPositionY() + 1); // 避免检测成功后坦克持续受，无法行动造成卡住

				// 方法2：履带停止转动
				// nowTank->Stay(TANK_DOWN);
			}
			if (nowTank->getLife() && (nowTank->getRect().intersectsRect(nowWbrick->getRect())) && (nowTank->getDirection() == TANK_LEFT))
			{
				// 方法1：履带持续转动
				nowTank->setHindered(TANK_LEFT);
				nowTank->setPositionX(nowTank->getPositionX() + 1); // 避免检测成功后坦克持续受，无法行动造成卡住

				// 方法2：履带停止转动
				// nowTank->Stay(TANK_LEFT);
			}
			if (nowTank->getLife() && (nowTank->getRect().intersectsRect(nowWbrick->getRect())) && (nowTank->getDirection() == TANK_RIGHT))
			{
				// 方法1：履带持续转动
				nowTank->setHindered(TANK_RIGHT);
				nowTank->setPositionX(nowTank->getPositionX() - 1); // 避免检测成功后坦克持续受，无法行动造成卡住

				// 方法2：履带停止转动
				// nowTank->Stay(TANK_RIGHT);
			}
		}
		
		// 坦克与坦克
		for (int j = 0; j < m_tankList.size(); j ++)
		{
			auto nowTank = m_tankList.at(i);
			auto anotherTank = m_tankList.at(j);
			if ((nowTank->getLife() && anotherTank->getLife()) && (anotherTank->getID() != nowTank->getID()) && (nowTank->getRect().intersectsRect(anotherTank->getRect())))
			{
				// 正在运动的坦克才作出如下动作
				if (nowTank->getDirection() == TANK_UP && nowTank->isMoving())
				{
					nowTank->Stay(TANK_UP);
				}
				if (nowTank->getDirection() == TANK_DOWN && nowTank->isMoving())
				{
					nowTank->Stay(TANK_DOWN);
				}
				if (nowTank->getDirection() == TANK_LEFT && nowTank->isMoving())
				{
					nowTank->Stay(TANK_LEFT);
				}
				if (nowTank->getDirection() == TANK_RIGHT && nowTank->isMoving())
				{
					nowTank->Stay(TANK_RIGHT);
				}
			}
		}

		// 坦克与子弹
		auto tank = m_tankList.at(i);
		for (int j = 0; j < tank->getBulletList().size(); j ++)
		{
			auto bullet = tank->getBulletList().at(j);
			for (int k = 0;k < m_tankList.size(); k ++)
			{
				auto tank_another = m_tankList.at(k);
				if (tank->getID() != tank_another->getID())
				{
					if (bullet->getRect().intersectsRect(tank_another->getRect()))
					{
						// 子弹消除
						m_deleteBulletList.pushBack(bullet);

						// 坦克消除
						if (k==0) {
							if (playerlives > 0) {
								playerlives--;
								PRespawn();
							}
							else {
								m_deleteTankList.pushBack(tank_another);
								GameOver();
							}

						}
						else {
							m_deleteTankList.pushBack(tank_another);
						}
						
						if (i == 0) {
							tank->killcount++;
							tank->upcount++;
						}
					}
				}
			}
		}

		// 子弹和墙
		for (int j = 0; j < tank->getBulletList().size(); j ++)
		{
			auto bullet = tank->getBulletList().at(j);
			for (int k = 0; k < m_bgList.size(); k ++)
			{
				auto brick = m_bgList.at(k);
				if (bullet->getRect().intersectsRect(brick->getRect()))
				{
					// 子弹消除
					m_deleteBulletList.pushBack(bullet);

					// 砖块消除
					m_deleteBrickList.pushBack(brick);
				}
			}
		}

		for (int j = 0; j < tank->getBulletList().size(); j++) {
			auto bullet = tank->getBulletList().at(j);
			for (int k = 0; k < m_tankList.size(); k++) {
				auto atank = m_tankList.at(k);
				for (int l = 0; l < atank->getBulletList().size(); l++) {
					auto abullet = atank->getBulletList().at(l);
					if (bullet->getRect().intersectsRect(abullet->getRect())&&i!=k) {
						m_deleteBulletList.pushBack(bullet);
						m_deleteBulletList.pushBack(abullet);
					}
				}
			}
		}

		for (int j = 0; j < tank->getBulletList().size(); j++)
		{
			auto bullet = tank->getBulletList().at(j);
			for (int k = 0; k < m_wbgList.size(); k++)
			{
				auto brick = m_wbgList.at(k);
				if (bullet->getRect().intersectsRect(brick->getRect()))
				{
					// 子弹消除
					m_deleteBulletList.pushBack(bullet);

					// 砖块消除
					//m_deleteBrickList.pushBack(brick);
				}
			}
		}


		// 清除删除子弹列表
		for (int j = 0; j < m_deleteBulletList.size(); j ++)
		{
			auto bullet = m_deleteBulletList.at(j);
			m_deleteBulletList.eraseObject(bullet);
			tank->getBulletList().eraseObject(bullet);
			bullet->Blast();
		}

		for (int j = 0; j < m_deleteUbulletList.size(); j++)
		{
			auto ubullet = m_deleteUbulletList.at(j);
			m_deleteUbulletList.eraseObject(ubullet);
			tank->getUbulletList().eraseObject(ubullet);
			ubullet->Blast();
		}

		// 清除删除砖块列表
		for (int j = 0; j < m_deleteBrickList.size(); j ++)
		{
			auto brick = m_deleteBrickList.at(j);
			m_deleteBrickList.eraseObject(brick);
			m_bgList.eraseObject(brick);
			brick->Blast();
		}

		for (int j = 0; j < m_deleteWbrickList.size(); j++) {
			auto brick = m_deleteWbrickList.at(j);
			m_deleteWbrickList.eraseObject(brick);
			m_wbgList.eraseObject(brick);
			brick->Blast();
		}

		// 清除删除坦克列表
		for (int j = 0; j < m_deleteTankList.size(); j ++)
		{
			auto tank = m_deleteTankList.at(j);
			m_deleteTankList.eraseObject(tank);
			m_tankList.eraseObject(tank);
			tank->Blast();
		}

		m_deleteWbrickList.clear();
		m_deleteUbulletList.clear();
		m_deleteBulletList.clear();
		m_deleteBrickList.clear();
		m_deleteTankList.clear();
	}
	if (m_tank->killcount == 24) {
		Victory();
	}
}

// 绘制几个回字砖块
void GameClient::createBackGround()
{
	auto map = TMXTiledMap::create("Chapter12/tank/map.tmx");
	this->addChild(map, 10);
	drawCore(Vec2(15 * 16, 33 * 16));
	drawBigBG(Vec2(15 * 16, 33 * 16));
	drawBigBG(Vec2(15 * 16, 13 * 16));
	drawBigBG(Vec2(15 * 16, 5 * 16));
	drawBigBG(Vec2(28 * 16, 35 * 16));
	drawBigBG(Vec2(37 * 16, 35 * 16));
	drawBigBG(Vec2(45 * 16, 35 * 16));
	drawBigBG(Vec2(45 * 16, 27 * 16));
	drawBigBG(Vec2(45 * 16, 20 * 16));
	drawBigBG(Vec2(27 * 16, 5 * 16));
	drawBigBG(Vec2(38 * 16, 5 * 16));
	drawWbigBG(Vec2(33 * 16, 19 * 16));
	drawWbigBG(Vec2(33 * 16, 27 * 16));
}

void GameClient::drawCore(Vec2 position) {
	m_core = Core::create(position, 0, 0);
	this->addChild(m_core, 2);
}

// 绘制单个回字砖块
void GameClient::drawBigBG(Vec2 position)
{
	for (int i = -2;i < 4;i ++)
	{
		for (int j = -2;j < 4;j ++)
		{
			if ((i == 1)&&(j == 0) || (i == 0)&&(j == 0) || (i == 1)&&(j == 1) || (i == 0)&&(j == 1))
			{
				// 中间留空形成回字
				continue;
			}
			auto brick = Brick::create(Vec2(position.x + (0.5 - i) * 16, position.y + (0.5 - j) * 16));
			m_bgList.pushBack(brick);
			this->addChild(brick, 2);
		}
	}
}

void GameClient::drawWbigBG(Vec2 position) {
	for (int i = -2; i < 4; i++)
	{
		for (int j = -2; j < 4; j++)
		{
			if ((i == 1) && (j == 0) || (i == 0) && (j == 0) || (i == 1) && (j == 1) || (i == 0) && (j == 1))
			{
				// 中间留空形成回字
				continue;
			}
			auto wbrick = Wbrick::create(Vec2(position.x + (0.5 - i) * 16, position.y + (0.5 - j) * 16));
			m_wbgList.pushBack(wbrick);
			this->addChild(wbrick, 2);
		}
	}

}

void GameClient::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{

	switch (keyCode)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_A:
		{
			m_tank->MoveLeft();
		}
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_W:
		// m_tank->MoveUP();
		{
			m_tank->MoveUP();
		}
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_S:
		// m_tank->MoveDown();
		{
			m_tank->MoveDown();
		}
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_D:
		// m_tank->MoveRight();
		{
			m_tank->MoveRight();
		}
		break;
	}
}

void GameClient::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	switch (keyCode)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_A:
		{
			m_tank->Stay(TANK_LEFT);
		}
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_W:
		{
			m_tank->Stay(TANK_UP);
		}
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_S:
		{
			m_tank->Stay(TANK_DOWN);
		}
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_D:
		{
			m_tank->Stay(TANK_RIGHT);
		}
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_F:
		{
			now = time(&now);
			if (now - m_tank->cool > 2) {
				m_tank->cool = time(&m_tank->cool);
				m_tank->Fire();
			}
		}
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_E:
	{
		m_tank->now = time(&m_tank->now);
		if (m_tank->now - m_tank->boost>20) {
			m_tank->Boost();
		}
	}
	break;
	case cocos2d::EventKeyboard::KeyCode::KEY_P:
	{
		gpause();
	}
	break;
	case cocos2d::EventKeyboard::KeyCode::KEY_R:
	{
		greboot();
	}
	break;
	case cocos2d::EventKeyboard::KeyCode::KEY_O:
	{
		gresume();
	}
	break;
	}
}

void GameClient::addTank(int id, float x, float y, int dir, int kind)
{
	m_maxTank[tankcount] = Tank::create(id, x, y, dir, kind);
	m_tankList.pushBack(m_maxTank[tankcount++]);
}

void GameClient::AIcontrol() {
	for (int i = 1; i < m_tankList.size(); i++) {
		auto tank = m_tankList.at(i);
		tank->now = time(&tank->now);
		if (tank->now - tank->boost > 1) {
			if (tank->direc >3) {
				int direc = (int)(CCRANDOM_0_1() * 7) % 6;
				tank->direc = direc%4;
			}
			switch (tank->direc) {
			case 0: {
				tank->MoveUP();
				tank->direc = 0;
			}
					break;
			case 1: {
				tank->MoveLeft();
				tank->direc = 1;
			}
					break;
			case 2: {
				tank->MoveDown();
				tank->direc = 2;
			}
					break;
			case 3: {
				tank->MoveRight();
				tank->direc = 3;
			}
					break;
			}
		}
		if (tank->now - tank->boost > 3) {
			if (tank->direc < 4) {
				tank->direc += 4;
			}
			tank->boost = time(&tank->boost);
			switch (tank->direc - 4) {
			case 0: {
				tank->Stay(TANK_DOWN);
			}
					break;
			case 1: {
				tank->Stay(TANK_UP);
			}
					break;
			case 2: {
				tank->Stay(TANK_LEFT);
			}
					break;
			case 3: {
				tank->Stay(TANK_RIGHT);
			}
					break;
			}
		}

		int firecontrol=0;
		if (tank->now - tank->cool > 2)
			firecontrol = (int)(CCRANDOM_0_1() * 2) % 3;
		if (firecontrol == 1 && tank->getBulletList().size() == 0) {
				tank->Fire();
		}
		else if(tank->cool==tank->now){
			tank->cool = time(&tank->cool);
		}
		if (tank->getBulletList().size() > 0) {
			tank->cool = time(&tank->cool);
		}
	}
}

void GameClient::greboot() {
	CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1.5f, GameClient::createScene()));
}

void GameClient::gpause() {
	auto pauselable = Label::createWithSystemFont("      GAME PAUSED\nPRESS O TO RESUME", "System", 25);
	m_pause = pauselable;
	m_pause->setVisible(true);
	m_pause->setPositionX(WINDOWWIDTH / 2);
	m_pause->setPositionY(WINDOWHEIGHT / 2);
	m_pause->enableShadow(Color4B(255, 0, 0, 255),Size(2,2));
	m_pause->setTextColor(Color4B(255,255,255,255));
	this->addChild(m_pause, 10);
	CCDirector::sharedDirector()->pause();
}

void GameClient::gresume() {
	if(m_pause!=NULL)
	m_pause->setVisible(false);
	CCDirector::sharedDirector()->resume();
}

void GameClient::GameOver() {
	auto gameover = Label::createWithSystemFont("     MISSION FAILED\nPRESS R TO RESTART", "System", 50);
	gameover->setTextColor(Color4B(255, 0, 0, 255));
	gameover->setPosition(Vec2(WINDOWWIDTH / 2, WINDOWHEIGHT / 2));
	gameover->enableShadow(Color4B(255, 255, 255, 255), Size(3, 3));
	this->addChild(gameover, 10);
}

void GameClient::Display() {
	std::string s1 = "TOTAL KILL: ";
	std::string s2 = std::to_string(m_tank->killcount);
	std::string s4 = "/24";
	std::string s3 = s1 + s2+s4;
	m_killcount->setString(s3);
	now = time(&now);
	if (20 - (now - m_tank->boost) > 0) {
		s1 = "BOOST IN:";
		s2 = std::to_string(20 - (now - m_tank->boost));
		s3 = s1 + s2;
		m_boostct->setString(s3);
		m_boostct->enableShadow(Color4B(0, 0, 255, 255), Size(0, 0));
	}
	else {
		s3 = "BOOST READY";
		m_boostct->setString(s3);
		m_boostct->enableShadow(Color4B(0, 0, 255, 255), Size(2, 2));
	}
	
	if (m_tank->getfireup()) {
		m_ammo->setString("AMMO: AC");
		m_ammo->enableShadow(Color4B(255, 0, 0, 255), Size(2, 2));
	}
	else {
		m_ammo->setString("AMMO: APC");
		m_ammo->enableShadow(Color4B(255, 0, 0, 255), Size(0, 0));
	}
	if (now - m_tank->cool <=2) {
		s1 = "RELOADING ";
		s2 = std::to_string(2 - (now - m_tank->cool));
		s3 = " S";
		s4 = s1 + s2 + s3;
		m_reload->setString(s4);
		m_reload->setTextColor(Color4B(255, 255, 255, 255));
		m_reload->setPosition(Vec2(1080, 490));
		m_reload->enableShadow(Color4B(255, 255, 0, 255), Size(2, 2));
	}
	else {
		m_reload->setString("GUN RELOADED!");
		m_reload->setTextColor(Color4B(255, 255, 255, 255));
		m_reload->setPosition(Vec2(1080, 490));
		m_reload->enableShadow(Color4B(0, 255, 0, 255), Size(2, 2));
	}
	if (playerlives == 2) {
		m_lives->setString("Lives: 2");
		m_lives->enableShadow(Color4B(255, 255, 0, 255), Size(2, 2));
	}
	else if (playerlives == 1) {
		m_lives->setString("Lives: 1");
		m_lives->enableShadow(Color4B(255, 122, 0, 255), Size(2, 2));
	}
	else if (playerlives == 0) {
		m_lives->setString("Lives: 0");
		m_lives->enableShadow(Color4B(255, 0, 0, 255), Size(2, 2));
	}
}

void GameClient::PRespawn() {
	m_tank->setPosition(Vec2(160,570));
	m_tank->firedown();
}

void GameClient::Victory() {
	auto victory = Label::createWithSystemFont("MISSION ACCOMPLISH", "System", 50);
	victory->setTextColor(Color4B(0, 255, 0, 255));
	victory->setPosition(Vec2(WINDOWWIDTH / 2, WINDOWHEIGHT / 2));
	this->addChild(victory, 10);
}