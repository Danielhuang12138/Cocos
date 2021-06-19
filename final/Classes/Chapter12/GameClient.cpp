#include "GameClient.h"

GameClient::GameClient()
{

}

GameClient::~GameClient()
{

}

bool GameClient::init()
{
	if (!Scene::init())
	{
		return false;
	}

	// 背景
	createBackGround();

	// 玩家
	m_tank = Tank::create(110, WINDOWWIDTH/2, 100, 1, 2);
	m_tankList.pushBack(m_tank);

	// 碰撞检测
	this->scheduleUpdate();

	// 键盘事件
	auto key_listener = EventListenerKeyboard::create();
	key_listener->onKeyPressed = CC_CALLBACK_2(GameClient::onKeyPressed, this);
	key_listener->onKeyReleased = CC_CALLBACK_2(GameClient::onKeyReleased, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(key_listener, this);

	m_shouldFireList.clear(); 

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
	if (playtank->getfireup()) {
		for (int j = 0; j < playtank->getUbulletList().size(); j++) {
			auto bullet = playtank->getUbulletList().at(j);
			for (int k = 0; k < m_tankList.size(); k++) {
				auto tank_another = m_tankList.at(k);
				if (playtank->getID() != tank_another->getID()) {
					if (bullet->getRect().intersectsRect(tank_another->getRect())) {
						m_deleteUbulletList.pushBack(bullet);
						m_deleteTankList.pushBack(tank_another);
					}
				}
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
			if (nowTank->getLife() && (nowTank->getRect().intersectsRect(nowBrick->getRect())) && (nowTank->getDirection() == TANK_RIGHT))
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
						m_deleteTankList.pushBack(tank_another);
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
}

// 绘制4个回字砖块
void GameClient::createBackGround()
{
	auto map = TMXTiledMap::create("Chapter12/tank/map.tmx");
	this->addChild(map, 10);

	drawBigBG(Vec2(16 * 16, 25 * 16));
	drawWbigBG(Vec2(44 * 16, 25 * 16));
	drawBigBG(Vec2(16 * 16, 14 * 16));
	drawWbigBG(Vec2(44 * 16, 14 * 16));
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
	case cocos2d::EventKeyboard::KeyCode::KEY_K:
		{
			m_tank->Fire();
		}
		break;
	}
}