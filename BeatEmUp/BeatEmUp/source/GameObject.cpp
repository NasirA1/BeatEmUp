#include "GameObject.h"


//Penetration threshold
const int PenThreshold(20);


bool GameObject::CollidesWith(const GameObject& other) const
{
	//If any of the sides from A are outside of B
	if(position.bottom() - PenThreshold <= other.position.top()) return false;
	if(position.top() + PenThreshold >= other.position.bottom()) return false;
	if(position.right() - PenThreshold <= other.position.left()) return false;
	if(position.left() + PenThreshold >= other.position.right()) return false;
	return true;
}


void GameObjectList::Update(bool removeDead)
{
	for (vector<GameObject*>::iterator it = begin(); it != end(); )
	{
		if((*it)->IsDead())
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
