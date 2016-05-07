#include "GameObject.h"
#include "Game.h"




bool GameObject::CollidedWith(const RectF& other, const int penThresholdX
		, const int penThresholdY, const int penThresholdZ) const
{
	//If any of the sides from A are outside of B
	if(position.bottom() - penThresholdY <= other.top()) return false;
	if(position.top() + penThresholdY >= other.bottom()) return false;
	if(position.right() - penThresholdX <= other.left()) return false;
	if(position.left() + penThresholdX >= other.right()) return false;
	if(SDL_abs( (int)position.z - (int)other.z ) > penThresholdZ)  return false;

	return true;
}


bool GameObject::CollidedWith(const GameObject* const other, const int penThresholdX
		, const int penThresholdY, const int penThresholdZ) const
{
	//null case
	if(!other) return false;

	return CollidedWith(other->position, penThresholdX, penThresholdY, penThresholdZ);
}


void GameObject::AdjustZToGameDepth() 
{ 
	position.z = position.y - GAME.MoveBounds.top();
}


void GameObjectList::Update()
{
	//Garbage collect dead objects
	//otherwise, update
	for (auto it = begin(); it != end(); )
	{
		if((*it)->IsMarkedForGC())
		{
			util::Delete(*it);
			it = erase(it);
		}
		else
		{
			(*it)->Update();
			++it;
		}
	}
}


void GameObjectList::Draw(SDL_Renderer& renderer) const
{
	for (const auto& actor : *this)
		actor->Draw(renderer);
}


GameObjectList::~GameObjectList()
{
	while(!empty())
	{
		util::Delete(back());
		pop_back();
	}
}
