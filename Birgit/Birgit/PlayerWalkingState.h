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

    bool m_running;
    SpriteSheet m_spriteWalkLeft;
    SpriteSheet m_spriteWalkRight;
    SpriteSheet m_spriteRunLeft;
    SpriteSheet m_spriteRunRight;
    SpriteSheet* m_spriteCurrent;
    
    static constexpr float walkingFriction = 1000.f;
    static constexpr float walkingAcceleration = 5000.f;
    static constexpr float walkingMaxSpeed = 50.f;
    static constexpr float runningFriction = walkingFriction;
    static constexpr float runningAcceleration = 3 * walkingAcceleration;
    static constexpr float runningMaxSpeed = 3 * walkingMaxSpeed;
};

