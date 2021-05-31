#include "Joystick.h"

Joystick::Joystick()
{

}

Joystick::~Joystick()
{

}
// 初始化 aPoint是摇杆中心 aRadius是摇杆半径 aJsSprite是摇杆控制点 aJsBg是摇杆背景
bool Joystick::init(Vec2 aPoint , float aRadius , char* aJsSprite, char* aJsBg)
{
	if (!Layer::init())
	{
		return false;
	}
	m_radius = aRadius;//摇杆半径
	m_centerPoint = aPoint;//摇杆控制中心
	m_currentPoint = m_centerPoint;

	m_jsSprite = Sprite::create(aJsSprite);//摇杆控制点
	m_jsSprite->setPosition(m_centerPoint);

	auto _aJsBg = Sprite::create(aJsBg);//摇杆背景
	_aJsBg->setPosition(m_centerPoint);
	_aJsBg->setTag(88);
	this->addChild(_aJsBg);
	this->addChild(m_jsSprite);
	this->scheduleUpdate(); 

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(Joystick::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(Joystick::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(Joystick::onTouchEnded, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}

Joystick* Joystick::create(Vec2 aPoint, float aRadius, char* aJsSprite, char* aJsBg)
{
	Joystick *pRet = new(std::nothrow) Joystick();
	if (pRet && pRet->init(aPoint, aRadius, aJsSprite, aJsBg)) {
		pRet->autorelease();
		return pRet;
	}
	else {
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

// 获取摇杆当前方向
Joystick_dir Joystick::getDirection()
{
	if ((m_currentPoint - m_centerPoint).x > 0)
	{
		return Joystick_dir::_RIGHT;
	}if ((m_currentPoint - m_centerPoint).x < 0)
	{
		return Joystick_dir::_LEFT;
	}
	if ((m_currentPoint - m_centerPoint).y > 0)
	{
		return Joystick_dir::_UPSI;
	}if ((m_currentPoint - m_centerPoint).y < 0)
	{
		return Joystick_dir::_DOWN;
	}
	return Joystick_dir::_STOP;
}

// 获取摇杆力度
float Joystick::getVelocity()
{
	return m_centerPoint.getDistance(m_currentPoint);
}
//获取角度
float Joystick::getAngle() {
	return angle;
}

// 更新摇杆按钮位置
void Joystick::update(float dt)
{ 
	m_jsSprite->setPosition(m_jsSprite->getPosition() + (m_currentPoint - m_jsSprite->getPosition()) * 0.5);
	if (m_currentPoint.y >= m_centerPoint.y)
		angle = acos((m_currentPoint.x - m_centerPoint.x) / Joystick::getVelocity());
	else
		angle = -acos((m_currentPoint.x - m_centerPoint.x) / Joystick::getVelocity());
}


bool Joystick::onTouchBegan(Touch *pTouch, Event *pEvent)
{
	auto touchPoint = pTouch->getLocation();
	if (touchPoint.getDistance(m_centerPoint) > m_radius){
		return false;
	}
	m_currentPoint = touchPoint;
	return true;
}

void Joystick::onTouchMoved(Touch *pTouch, Event *pEvent)
{
	auto touchPoint = pTouch->getLocation();
	if (touchPoint.getDistance(m_centerPoint) > m_radius)
	{
		m_currentPoint = m_centerPoint + (touchPoint - m_centerPoint).getNormalized() * m_radius;
	}else {
		m_currentPoint = touchPoint;
	}
}

void Joystick::onTouchEnded(Touch *pTouch, Event *pEvent)
{
	m_currentPoint = m_centerPoint;
}
