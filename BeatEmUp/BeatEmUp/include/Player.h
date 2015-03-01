#pragma once
#include "Sprite.h"


class Player : public GameObject
{
public:
	Player(SDL_Renderer* const renderer);
	virtual void Update() override;
	virtual void Draw(SDL_Renderer* const renderer) const override;
	virtual ~Player();

	//Accessors
	__forceinline Directions GetDirection() const { return direction; }
	__forceinline RectF& Position() { return position; }

enum JumpState
{
	JS_Ground,
	JS_Jumped,
	JS_Landing
};

public:
	void Stop();
	void GoUp();
	void GoDown();
	void GoRight();
	void GoLeft();
	void SetDirection(Directions dir);
	void Jump(float xForce, float yForce);

private:
	void Translate(bool anim = true);


private:
	Sprite* walkRight;
	Sprite* walkLeft;
	Sprite* current;

	float xVel, yVel;
	SDL_Rect moveBounds;
	Directions direction;
	JumpState jumpState;
	VectF jumpLocation;
	static const float Gravity;
	static const int JumpHeight;
	static const float WalkVel;
};
