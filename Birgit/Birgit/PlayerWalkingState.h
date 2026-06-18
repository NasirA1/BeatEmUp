#pragma once
#include "Core.h"
#include "SpriteSheet.h"

class PlayerWalkingState : public PlayerState
{
public:
    constexpr static size_t tileWidth = 83;
    constexpr static size_t tileHeight = 90;

    PlayerWalkingState();
    void enter(Player& owner) override;
    void exit(Player& owner) override;
    void update(Player& owner, float dt) override;
    void draw(Player& owner, sf::RenderWindow& window) override;
    PlayerStateId stateId() const override;

private:
    void setDirection(HorizontalDirection dir);
    void handleAcceleration(Player& owner, float dt);
    void applyFriction(Player& owner, float dt);
    void clampVelocity(Player& owner);

    SpriteSheet m_spriteLeft;
    SpriteSheet m_spriteRight;
    SpriteSheet* m_spriteCurrent;
};

