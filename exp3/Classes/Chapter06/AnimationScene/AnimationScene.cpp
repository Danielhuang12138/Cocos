#include "AnimationScene.h"


AnimationScene::AnimationScene()
{
	
}

AnimationScene::~AnimationScene()
{

}
auto LabelScore1 = Label::createWithBMFont("../Resources/fonts/futura-48.fnt", "0");
auto LabelScore2 = Label::createWithBMFont("../Resources/fonts/futura-48.fnt", "0");
bool AnimationScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();

	// 背景
	auto background = LayerColor::create(Color4B(128,128,128,255), 960, 640);
	background->setPosition(Vec2(0, 0));
	background->setTag(110);
	this->addChild(background);
	auto cloud_1 = Sprite::create("Chapter06/AnimationScene/animation/background/cloud.png");
	cloud_1->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));
	cloud_1->setTag(112);
	this->addChild(cloud_1);
	auto cloud_2 = Sprite::create("Chapter06/AnimationScene/animation/background/cloud.png");
	cloud_2->setPosition(Vec2(cloud_1->getPositionX() + cloud_1->getContentSize().width, visibleSize.height/2));
	cloud_2->setTag(113);
	this->addChild(cloud_2);

	// 背景文字VS
	auto tips_vs = Label::createWithBMFont("fonts/futura-48.fnt","PK");
	tips_vs->setPosition(Vec2(visibleSize.width/2,visibleSize.height - 80));
	this->addChild(tips_vs, 5);

	// 摇杆
	m_joystick = Joystick::create(Vec2(100,100), 50.0f, "Chapter06/AnimationScene/animation/control/j-btn.png", "Chapter06/AnimationScene/animation/control/j-bg.png");
	this->addChild(m_joystick, 4);

	// 攻击
	auto attackItem = MenuItemImage::create(
		"Chapter06/AnimationScene/animation/control/j-btn.png",
		"Chapter06/AnimationScene/animation/control/j-btn.png",
		CC_CALLBACK_1(AnimationScene::attackCallback, this));

	attackItem->setPosition(Vec2(visibleSize.width - 90, 100));
	auto act1 = FadeOut::create(1);  
	auto act2 = FadeIn::create(1);
	auto act3 = Sequence::create(act1, act2, NULL);
	auto act = RepeatForever::create(act3);  // 按钮闪烁
	attackItem->runAction(act);
	auto menu = Menu::create(attackItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 5);
	// 防御
	auto defendItem = MenuItemImage::create(
		"Chapter06/AnimationScene/animation/control/j-btn.png",
		"Chapter06/AnimationScene/animation/control/j-btn.png",
		CC_CALLBACK_1(AnimationScene::defendCallback, this));

	defendItem->setPosition(Vec2(visibleSize.width - 30, 100));
	auto act11 = FadeOut::create(1);
	auto act21 = FadeIn::create(1);
	auto act31 = Sequence::create(act11, act21, NULL);
	auto act_1 = RepeatForever::create(act31);  // 按钮闪烁
	defendItem->runAction(act_1);
	auto menu1 = Menu::create(defendItem, NULL);
	menu1->setPosition(Vec2::ZERO);
	this->addChild(menu1, 5);

	//计分板
	char str[20] = "";
	//hero得分
	_itoa(heroscore, str, 10);
	LabelScore1 = Label::createWithBMFont("fonts/futura-48.fnt", str);
	LabelScore1->setPosition(Vec2(visibleSize.width / 2 - 50, visibleSize.height - 150));
	this->addChild(LabelScore1, 1, 100);
	auto label1 = Label::createWithBMFont("fonts/futura-48.fnt", ":");
	label1->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 150));
	this->addChild(label1, 5);
	//enemy得分
	_itoa(enemyscore, str, 10);
	LabelScore2 = Label::createWithBMFont("fonts/futura-48.fnt", str);
	LabelScore2->setPosition(Vec2(visibleSize.width / 2 + 50, visibleSize.height - 150));
	this->addChild(LabelScore2, 1, 101);

	// 地图
	auto map = TMXTiledMap::create("Chapter06/AnimationScene/animation/background/background.tmx");
	if (map == NULL)
	{
		CCLOG("map load failed!");
		return false;
	}
	Size mapSize = map->getMapSize();
	Size tiledSize = map->getTileSize();
	map->setPosition(Vec2(0, 0));
	map->setTag(111);
	this->addChild(map, 2);

	// enemy
	m_enemy = Enemy::create(Vec2(visibleSize.width - 200, 150));
	this->addChild(m_enemy, 3);

	// hero
	m_player = Hero::create(Vec2(visibleSize.width/4, 100));
	this->addChild(m_player, 3);

	// AI 
	m_aimanager = AIManager::create(this);
	m_aimanager->setAI(m_enemy, m_player);

	// 碰撞检测
	m_contactListener = MyContactListener::create(this, m_player, m_enemy);

	// 血条
	m_bloodBar_h = Sprite::create("Chapter06/AnimationScene/animation/background/1-new.png");
	m_bloodBar_h->setTag(10000);
	auto blood_bg_h = Sprite::create("Chapter06/AnimationScene/animation/background/1-new-f.png");
	m_bloodBar_h->setPosition(Vec2(m_bloodBar_h->getContentSize().width/2 + 5, visibleSize.height - 50));
	blood_bg_h->setPosition(Vec2(m_bloodBar_h->getContentSize().width/2 + 5, visibleSize.height - 50));

	this->addChild(blood_bg_h, 5);
	this->addChild(m_bloodBar_h, 5);
	
	m_bloodBar_e = Sprite::create("Chapter06/AnimationScene/animation/background/2-new.png");
	m_bloodBar_e->setTag(10001);
	auto blood_bg_e = Sprite::create("Chapter06/AnimationScene/animation/background/2-new-f.png");
	m_bloodBar_e->setPosition(Vec2(visibleSize.width - m_bloodBar_e->getContentSize().width/2 - 5, visibleSize.height - 50));
	blood_bg_e->setPosition(Vec2(visibleSize.width - m_bloodBar_e->getContentSize().width/2 - 5, visibleSize.height - 50));

	this->addChild(blood_bg_e, 5);
	this->addChild(m_bloodBar_e, 5);

	// update
	this->scheduleUpdate();


	return true;
}

void AnimationScene::pkres() {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	char str[20];
	if (dead == 0) {
		//hero赢
		if (m_enemy->getLife() <= 0) {
			heroscore += 5;
			_itoa(heroscore, str, 10);
			LabelScore1->setString(str);
			//round win
			if (heroscore < 15) {
				//reboot
				this->scheduleOnce(schedule_selector(AnimationScene::reboot), 3.0f);
				auto LabelHerowin = Label::createWithBMFont("fonts/futura-48.fnt", "ROUND WIN");
				LabelHerowin->setScale(1.0);
				LabelHerowin->setPosition(Vec2(visibleSize.width / 2 - 360, visibleSize.height - 110));
				this->addChild(LabelHerowin, 5);
				dead = 1;
			}
			else {
				//victory
				auto LabelVictory = Label::createWithBMFont("fonts/futura-48.fnt", "VICTORY");
				LabelVictory->setScale(1.5);
				LabelVictory->setPosition(Vec2(visibleSize.width / 2, visibleSize.height/2));
				this->addChild(LabelVictory, 5);
				dead = 1;
			}
		}
		else if (m_player->getLife() <= 0) {
			enemyscore += 3;
			_itoa(enemyscore, str, 10);
			LabelScore2->setString(str);
			if (enemyscore < 15) {
				//reboot
				this->scheduleOnce(schedule_selector(AnimationScene::reboot), 3.0f);
				auto LabelHerolose = Label::createWithBMFont("fonts/futura-48.fnt", "ROUND LOST");
				LabelHerolose->setScale(1.0);
				LabelHerolose->setPosition(Vec2(visibleSize.width / 2 + 350, visibleSize.height - 110));
				this->addChild(LabelHerolose, 5);
				dead = 1;
			}
			else {
				//lost
				auto LabelHerolost = Label::createWithBMFont("fonts/futura-48.fnt", "YOU LOSE");
				LabelHerolost->setScale(1.5);
				LabelHerolost->setPosition(Vec2(visibleSize.width / 2, visibleSize.height /2));
				this->addChild(LabelHerolost, 5);
				dead = 1;
			}
		}
	}
}
Scene* AnimationScene::createScene()
{
	auto scene = Scene::create();
	auto layer = AnimationScene::create();
	scene->addChild(layer);
	return scene;
}

void AnimationScene::update(float delta)
{
	// 背景云动画
	pkres();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto cloud_1 = (Sprite*)getChildByTag(112);
	auto cloud_2 = (Sprite*)getChildByTag(113); 
	if (cloud_1->getPositionX() > -(cloud_1->getContentSize().width - visibleSize.width))
	{
		cloud_1->setPositionX(cloud_1->getPositionX() - 1);
	} else
	{
		cloud_1->setPositionX(visibleSize.width/2);
	}
	//cloud_2->setPositionX(cloud_1->getPositionX() + cloud_1->getContentSize().width/2);

	// 更新血条
	float temp_e = float(m_enemy->getLife()) / m_enemy->getMaxLife();
	float x = 44 + (354-88-44)*(1-temp_e);
	m_bloodBar_e->setTextureRect(Rect(x,0,354 - x,m_bloodBar_e->getContentSize().height));
	m_bloodBar_e->setPositionX(visibleSize.width - m_bloodBar_e->getContentSize().width/2 - 5);

	float temp_h = float(m_player->getLife()) / m_player->getMaxLife();
	m_bloodBar_h->setTextureRect(Rect(0,0,88+(354-88-44)* temp_h,m_bloodBar_h->getContentSize().height));
	m_bloodBar_h->setPositionX(m_bloodBar_h->getContentSize().width/2 + 5);

	// 控制角色移动
	if (m_joystick->getDirection() == Joystick_dir::_RIGHT)
	{
		m_player->play(MOVERIGHT);
	} else if (m_joystick->getDirection() == Joystick_dir::_LEFT)
	{
		m_player->play(MOVELEFT);
	} else if (m_joystick->getDirection() == Joystick_dir::_STOP)
	{
		m_player->play(STAND);
	}
}

void AnimationScene::attackCallback(Ref* pSender)
{
	m_player->play(ATTACK);
	//m_enemy->play(ATTACK);
}

void AnimationScene::defendCallback(Ref* pSender) {
	m_player->play(DEFEND);
}

void AnimationScene::reboot(float t) {
	//reboot
	CCDirector::sharedDirector()->replaceScene(CCTransitionFadeDown::create(1.5f, AnimationScene::createScene()));
	//dead = 0;
}

