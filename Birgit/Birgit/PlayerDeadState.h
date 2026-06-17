#pragma once
#include "Core.h"
#include "SpriteSheet.h"

class PlayerDeadState : public PlayerState
{
public:
    constexpr static size_t tileWidth = 601;
    constexpr static size_t tileHeight = 510;

    PlayerDeadState();
    void enter(Player& owner) override;
    void exit(Player& owner) override;
    void update(Player& owner, float dt) override;
    void draw(Player& owner, sf::RenderWindow& window) override;
    PlayerStateId stateId() const override;

private:
    void setDirection(HorizontalDirection dir);

    SpriteSheet m_spriteLeft;
    SpriteSheet m_spriteRight;
    SpriteSheet* m_spriteCurrent;
};

