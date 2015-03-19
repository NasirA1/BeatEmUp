#include "GameObject.h"
#include "Game.h"




bool GameObject::CollidedWith(const GameObject* const other, const int penThresholdX
		, const int penThresholdY, const int penThresholdZ) const
{
	//null case
	if(!other) return false;

	//If any of the sides from A are outside of B
	if(position.bottom() - penThresholdY <= other->position.top()) return false;
	if(position.top() + penThresholdY >= other->position.bottom()) return false;
	if(position.right() - penThresholdX <= other->position.left()) return false;
	if(position.left() + penThresholdX >= other->position.right()) return false;
	if(SDL_abs( (int)position.z - (int)other->position.z ) > penThresholdZ)  return false;

	return true;
}


void GameObject::AdjustZToGameDepth() 
{ 
	position.z = position.y - GAME.MoveBounds.top();
}



void GameObjectList::Update()
{
	//Garbage collect dead objects (except Player - special case)
	for (vector<GameObject*>::iterator it = begin(); it != end(); )
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


void GameObjectList::Draw(SDL_Renderer* const renderer) const
{
	for (vector<GameObject*>::const_iterator it = begin(); it != end(); ++it)
		(*it)->Draw(renderer);
}


GameObjectList::~GameObjectList()
{
	while(!empty())
	{
		util::Delete(back());
		pop_back();
	}
}
