#include "PlayerJumpState.h"

#include "Player.h"
#include "ResourceManager.h"
#include "Constants.h"
#include "PlayerIdleState.h"

#include <iostream>


PlayerJumpState::PlayerJumpState()
    : m_spriteRight(ResourceManager::instance().getTexture(Constants::Sprite::CuteGirlJumpRight_PNG), tileWidth, tileHeight)
    , m_spriteLeft(ResourceManager::instance().getTexture(Constants::Sprite::CuteGirlJumpLeft_PNG), tileWidth, tileHeight)
    , m_spriteCurrent(nullptr)
    , m_groundPos {}
{
}

void PlayerJumpState::setDirection(HorizontalDirection dir)
{
    m_spriteCurrent = dir == HorizontalDirection::Right ? &m_spriteRight : &m_spriteLeft;
}

void PlayerJumpState::enter(Player& owner)
{
    std::cout << "PlayerJumpState::enter\n";
    m_spriteRight.reset(owner.m_pos);
    m_spriteLeft.reset(owner.m_pos);
    setDirection(owner.m_direction);
    m_groundPos = owner.m_pos;
}

void PlayerJumpState::exit(Player& owner)
{
    //std::cout << "PlayerJumpState::exit\n";
    owner.m_pos.y = m_groundPos.y;
}

void PlayerJumpState::update(Player& owner, float dt)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        setDirection(HorizontalDirection::Left);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        setDirection(HorizontalDirection::Right);
    }

    m_spriteCurrent->sprite().setPosition(owner.m_pos);
    m_spriteCurrent->update(dt);

    // TODO what to do?!
}

void PlayerJumpState::draw(Player& owner, sf::RenderWindow& window)
{
    m_spriteCurrent->draw(window);
}

PlayerStateId PlayerJumpState::stateId() const
{
    return PlayerStateId::Jumping;
}
