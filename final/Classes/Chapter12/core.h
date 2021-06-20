#ifndef __CORE_H__
#define __CORE_H__

#include "BaseObject.h"
#include "Global.h"


class Core : public BaseObject
{
public:
	Core();
	~Core();
	static Core* create(Vec2 position, float speed, int dir);

	// 爆炸时已自动设置life为0
	void Blast();
	Rect getRect() { return m_rect; };

private:
	virtual bool init(Vec2 position, float speed, int dir);
	//void update(float delta);
	void deleteObj(Sprite* obj);

private:
	Sprite*     m_sprite;       // 图片精灵
	Texture2D*  m_texture;      // 文理
	Rect        m_rect;			// 碰撞矩形
};
#endif
