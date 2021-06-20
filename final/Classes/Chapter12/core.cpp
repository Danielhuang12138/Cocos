#include "core.h"

Core::Core()
{

}

Core::~Core()
{

}

Core* Core::create(Vec2 position, float speed, int dir)
{
	Core* pRet = new(std::nothrow) Core();
	if (pRet && pRet->init(position, 0, dir))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

bool Core::init(Vec2 position, float speed, int dir)
{
	if (!BaseObject::init())
	{
		return false;
	}
	m_texture = Director::getInstance()->getTextureCache()->addImage("Chapter12/tank/tile.png");
	m_sprite = Sprite::createWithTexture(m_texture, Rect(160, 0, 32, 32));

	m_sprite->setPosition(Vec2::ZERO);
	this->addChild(m_sprite);

	this->setLife(1);
	this->setPosition(position);
	this->setDirection(dir);
	this->setSpeed(speed);
	this->scheduleUpdate();

	m_rect = Rect(position.x - 16, position.y - 16, 32, 32);  // rect
	return true;
}

void Core::Blast()
{
	this->setVisible(false);   // 子弹消失
	setLife(0);
	auto explode = Sprite::create("Chapter12/tank/uexplode2.png");
	this->getParent()->addChild(explode, 10);
	explode->setPosition(this->getPosition());  // 显示爆炸
	explode->runAction(Sequence::create(
		DelayTime::create(0.1f),
		FadeOut::create(0.1f),                   // 爆炸消失
		CallFunc::create(CC_CALLBACK_0(Core::deleteObj, this, explode)),
		NULL
	));
	m_texture = Director::getInstance()->getTextureCache()->addImage("Chapter12/tank/tile.png");
	auto fallen = Sprite::createWithTexture(m_texture, Rect(192, 0, 32, 32));
	this->getParent()->addChild(fallen, 9);
	fallen->setPosition(this->getPosition());
}

void Core::deleteObj(Sprite* obj)
{
	//obj->removeFromParent();
	//this->removeFromParent();
}
