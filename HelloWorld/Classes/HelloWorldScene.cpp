#include "HelloWorldScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();
    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}
auto label = Label::createWithSystemFont("0", "Consolas", 30, Size(100, 100), TextHAlignment::RIGHT);
bool HelloWorld::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
	//添加分数层
	auto labelScore =
		Label::createWithSystemFont("得分：", "Consolas", 30, Size(100, 100));
	labelScore->setPosition(Vec2(60, 200));
	this->addChild(labelScore, 1);
	score = 0;
	label = Label::createWithSystemFont("0", "Consolas", 30, Size(100, 100), TextHAlignment::RIGHT);
	label->setPosition(Vec2(130, 200));
	this->addChild(label, 1, 100);

	//添加颜色层
	auto colorLayer = LayerColor::create(Color4B(128, 125, 200, 255), 480, visibleSize.height);
	colorLayer->setPosition(Vec2(200, 0));
	this->addChild(colorLayer);

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

	//移动精灵
	bean = Sprite::create("Chapter10/bean.png");
	bean->setPosition(Point(visibleSize.width/2, 100));
	this->addChild(bean, 6);

	// 摇杆
	m_joystick = Joystick::create(Vec2(100, 100), 50.0f, "Chapter10/j-btn.png", "Chapter10/j-bg.png");
	this->addChild(m_joystick, 4);
    
	this->schedule(schedule_selector(HelloWorld::addBall1), 1.0f);
	this->schedule(schedule_selector(HelloWorld::addBall2), 2.0f);
	this->schedule(schedule_selector(HelloWorld::addBall3), 3.0f);
	this->scheduleUpdate();

    return true;
}
void HelloWorld::updateScore(Sprite*ball) {
	char str[20];
	if (ball->getTag() == 1)
		score = score + 10;
	else if (ball->getTag() == 5)
		score = score + 50;
	else if (ball->getTag() == 10)
		score = score + 100;
	itoa(score, str, 10);
	label->setString(str);
}
void HelloWorld::addBall1(float dt){
	auto ball1 = Sprite::create("Chapter10/ball.png");//使用图片创建小球
	ball1->setPosition(Point(207 + rand() % 460, visibleSize.height));//设置小球的初始位置，这里在x方向使用了随机函数rand使得小球在随机位置出现
	ball1->setTag(1);
	this->addChild(ball1, 5);
	this->ballVector.pushBack(ball1);//将小球放进Vector数组
	auto moveTo = MoveTo::create(rand() % 5, Point(ball1->getPositionX(), -10));//移动动作
	auto actionDone = CallFunc::create(CC_CALLBACK_0(HelloWorld::removeBall, this, ball1));//当小球移动到屏幕下方时回调removeBall函数，移除小球
	auto sequence = Sequence::create(moveTo, actionDone, nullptr);
	ball1->runAction(sequence);//执行动作
}
void HelloWorld::addBall2(float dt){
	auto ball2 = Sprite::create("Chapter10/ball1.png");
	ball2->setPosition(Point(207+rand() % 460, visibleSize.height));
	ball2->setTag(5);
	this->addChild(ball2, 5);
	this->ballVector.pushBack(ball2);
	auto moveTo = MoveTo::create(rand() % 5, Point(ball2->getPositionX(), -10));
	auto actionDone = CallFunc::create(CC_CALLBACK_0(HelloWorld::removeBall, this, ball2));
	auto sequence = Sequence::create(moveTo, actionDone, nullptr);
	ball2->runAction(sequence);
}
void HelloWorld::addBall3(float dt){
	auto ball3 = Sprite::create("Chapter10/ball2.png");
	ball3->setPosition(Point(207+rand() % 460, visibleSize.height));
	ball3->setTag(10);
	this->addChild(ball3, 5);
	this->ballVector.pushBack(ball3);
	auto moveTo = MoveTo::create(rand() % 5, Point(ball3->getPositionX(), -10));
	auto actionDone = CallFunc::create(CC_CALLBACK_0(HelloWorld::removeBall, this, ball3));
	auto sequence = Sequence::create(moveTo, actionDone, nullptr);
	ball3->runAction(sequence);
}
void HelloWorld::removeBall(Sprite* ball){
	this->removeChild(ball);
	ballVector.eraseObject(ball);
}
void HelloWorld::update(float dt){
	for (auto ball:ballVector)
	{
		//进行碰撞检测
		if (bean->getBoundingBox().intersectsRect(ball->getBoundingBox()))
		{
			HelloWorld::updateScore(ball);
			auto actionDown = CallFunc::create(CC_CALLBACK_0(HelloWorld::removeBall, this, ball));
			ball->runAction(actionDown);
		}
	}
	
	// 控制角色移动
	float angle = m_joystick->getAngle();
	float xx = 6 * cos(angle);
	float yy = 6 * sin(angle);
	if (xx > 0) {
		if (bean->getPositionX() + bean->getContentSize().width / 2 + xx <= 680){
			bean->setPositionX(bean->getPositionX() + xx);
		}
		else {
			bean->setPositionX(680 - bean->getContentSize().width / 2);
		}
	}
	else if (xx < 0) {
		if (bean->getPositionX() - bean->getContentSize().width / 2 + xx >= 200) {
			bean->setPositionX(bean->getPositionX() + xx);
		}
		else {
			bean->setPositionX(200 + bean->getContentSize().width / 2);
		}
	}
	if (yy > 0) {
		if (bean->getPositionY() + bean->getContentSize().height / 2 + yy <= 768) {
			bean->setPositionY(bean->getPositionY() + yy);
		}
		else {
			bean->setPositionY(768 - bean->getContentSize().height / 2);
		}
	}
	else if (yy < 0) {
		if (bean->getPositionY() - bean->getContentSize().height / 2 + yy >= 0) {
			bean->setPositionY(bean->getPositionY() + yy);
		}
		else {
			bean->setPositionY(0 + bean->getContentSize().height / 2);
		}
	}
	/*if (m_joystick->getDirection() == Joystick_dir::_RIGHT
		&&bean->getPositionX()+bean->getContentSize().width/2<=680)
	{
		bean->setPositionX(bean->getPositionX()+4);
	}
	if (m_joystick->getDirection() == Joystick_dir::_LEFT
		&&bean->getPositionX()-bean->getContentSize().width/2>=200)
	{
		bean->setPositionX(bean->getPositionX() - 4);
	}
	if (m_joystick->getDirection() == Joystick_dir::_UPSI)
		//&&bean->getPositionY() - bean->getContentSize().height / 2 <= 200)
	{
		bean->setPositionY(bean->getPositionY() + 4);
	}
	if (m_joystick->getDirection() == Joystick_dir::_DOWN)
		//&&bean->getPositionY() - bean->getContentSize().height / 2 >= 0)
	{
		bean->setPositionY(bean->getPositionY() - 4);
	}
	if (m_joystick->getDirection() == Joystick_dir::_STOP)
	{
		bean->setPositionX(bean->getPositionX());
	}*/
}
void HelloWorld::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
