#include "PlayerJumpState.h"

#include "Player.h"
#include "ResourceManager.h"
#include "Constants.h"
#include "PlayerIdleState.h"

#include <iostream>


PlayerJumpState::PlayerJumpState()
    : m_spriteRight(ResourceManager::instance().getTexture(Constants::Sprite::CuteGirlJumpRight_PNG), tileWidth, tileHeight)
    , m_spriteLeft(ResourceManager::instance().getTexture(Constants::Sprite::CuteGirlJumpLeft_PNG), tileWidth, tileHeight, true)
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
    m_velocityY = JumpVelocity;
}

void PlayerJumpState::exit(Player& owner)
{
    //std::cout << "PlayerJumpState::exit\n";
    owner.m_pos.y = m_groundPos.y;
}

void PlayerJumpState::update(Player& owner, float dt)
{
    float moveX = 0.f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        setDirection(HorizontalDirection::Left);
        owner.m_direction = HorizontalDirection::Left;
        moveX = -JumpMoveSpeed;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        setDirection(HorizontalDirection::Right);
        owner.m_direction = HorizontalDirection::Right;
        moveX = JumpMoveSpeed;
    }

    owner.m_pos.x += moveX * dt;

    m_velocityY += Gravity * dt;
    owner.m_pos.y += m_velocityY * dt;

    if (owner.m_pos.y >= m_groundPos.y)
    {
        owner.m_pos.y = m_groundPos.y;
        owner.changeState(std::make_unique<PlayerIdleState>());
        return;
    }

    m_spriteCurrent->sprite().setPosition(owner.m_pos);
    m_spriteCurrent->update(dt);}

void PlayerJumpState::draw(Player& owner, sf::RenderWindow& window)
{
    m_spriteCurrent->draw(window);
}

PlayerStateId PlayerJumpState::stateId() const
{
    return PlayerStateId::Jumping;
}
