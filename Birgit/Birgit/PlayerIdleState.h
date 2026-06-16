#pragma once
#include "Core.h"
#include "SpriteSheet.h"

class PlayerIdleState : public PlayerState
{
public:
    constexpr static size_t tileWidth = 418;
    constexpr static size_t tileHeight = 456;

    PlayerIdleState();
    void enter(Player& owner) override;
    void exit(Player& owner) override;
    void update(Player& owner, float dt) override;
    void draw(Player& owner, sf::RenderWindow& window) override;

private:
    void setDirection(HorizontalDirection dir);

    SpriteSheet m_sprite_left;
    SpriteSheet m_sprite_right;
    SpriteSheet* m_sprite_current;
};
