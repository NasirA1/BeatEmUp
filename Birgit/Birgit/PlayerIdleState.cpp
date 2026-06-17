#include "PlayerIdleState.h"

#include "Player.h"
#include "ResourceManager.h"
#include "PlayerWalkingState.h"
#include "Constants.h"

#include <iostream>


PlayerIdleState::PlayerIdleState()
    : m_spriteRight(ResourceManager::instance().getTexture(Constants::Sprite::CuteGirlIdleRight_PNG), tileWidth, tileHeight)
    , m_spriteLeft(ResourceManager::instance().getTexture(Constants::Sprite::CuteGirlIdleLeft_PNG), tileWidth, tileHeight, true)
    , m_spriteCurrent(nullptr)
{
}

void PlayerIdleState::setDirection(HorizontalDirection dir)
{
    m_spriteCurrent = dir == HorizontalDirection::Right ? &m_spriteRight : &m_spriteLeft;
}

void PlayerIdleState::enter(Player& owner)
{
    std::cout << "PlayerIdleState::enter\n";
    m_spriteRight.reset(owner.m_pos);
    m_spriteLeft.reset(owner.m_pos);
    setDirection(owner.m_direction);
}

void PlayerIdleState::exit(Player& owner)
{
    //std::cout << "PlayerIdleState::exit\n";
}

void PlayerIdleState::update(Player& owner, float dt)
{
    m_spriteCurrent->update(dt);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        owner.changeState(std::make_unique<PlayerWalkingState>());
    }
}

void PlayerIdleState::draw(Player& owner, sf::RenderWindow& window)
{
    m_spriteCurrent->draw(window);
}

PlayerStateId PlayerIdleState::stateId() const
{
    return PlayerStateId::Idle;
}
