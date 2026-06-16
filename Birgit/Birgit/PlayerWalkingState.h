#pragma once
#include "Core.h"
#include "SpriteSheet.h"

class PlayerWalkingState : public PlayerState
{
public:
    constexpr static size_t tileWidth = 418u;
    constexpr static size_t tileHeight = 456u;

    PlayerWalkingState();
    void enter(Player& owner) override;
    void exit(Player& owner) override;
    void update(Player& owner, float dt) override;
    void draw(Player& owner, sf::RenderWindow& window) override;

private:
    void setDirection(HorizontalDirection dir);
    void handleAcceleration(Player& owner, float dt);
    void applyFriction(Player& owner, float dt);
    void clampVelocity(Player& owner);

    SpriteSheet m_sprite_left;
    SpriteSheet m_sprite_right;
    SpriteSheet* m_sprite_current;
};

