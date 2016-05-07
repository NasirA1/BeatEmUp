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


bool GameObject::CollidedWith(const GameObject& other, const int penThresholdX
		, const int penThresholdY, const int penThresholdZ) const
{
	return CollidedWith(other.position, penThresholdX, penThresholdY, penThresholdZ);
}


void GameObject::AdjustZToGameDepth() 
{ 
	position.z = position.y - GAME.MoveBounds.top();
}
