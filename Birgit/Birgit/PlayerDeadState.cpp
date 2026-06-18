#include "PlayerDeadState.h"

#include "Player.h"
#include "ResourceManager.h"
#include "PlayerIdleState.h"
#include "Constants.h"

#include <iostream>


PlayerDeadState::PlayerDeadState()
    : m_spriteRight(ResourceManager::instance().getTexture(Constants::Sprite::CuteGirlDeadRight_PNG), tileWidth, tileHeight, false, false)
    , m_spriteLeft(ResourceManager::instance().getTexture(Constants::Sprite::CuteGirlDeadLeft_PNG), tileWidth, tileHeight, true, false)
    , m_spriteCurrent(nullptr)
{
}

void PlayerDeadState::setDirection(HorizontalDirection dir)
{
    m_spriteCurrent = dir == HorizontalDirection::Right ? &m_spriteRight : &m_spriteLeft;
}

void PlayerDeadState::enter(Player& owner)
{
    std::cout << "PlayerDeadState::enter\n";
    m_spriteRight.reset(owner.m_pos);
    m_spriteLeft.reset(owner.m_pos);
    setDirection(owner.m_direction);
}

void PlayerDeadState::exit(Player& owner)
{
    //std::cout << "PlayerIdleState::exit\n";
}

void PlayerDeadState::update(Player& owner, float dt)
{
    m_spriteCurrent->update(dt);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1))
    {
        owner.setPosition({ 1280.f / 2.f, 350.f });
        owner.changeState(std::make_unique<PlayerIdleState>());
    }
}

void PlayerDeadState::draw(Player& owner, sf::RenderWindow& window)
{
    m_spriteCurrent->draw(window);
}

PlayerStateId PlayerDeadState::stateId() const
{
    return PlayerStateId::Dead;
}
