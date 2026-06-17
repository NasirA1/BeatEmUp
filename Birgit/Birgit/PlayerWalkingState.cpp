#include "PlayerWalkingState.h"

#include "ResourceManager.h"
#include "Player.h"
#include "PlayerIdleState.h"
#include "Constants.h"
#include <iostream>

PlayerWalkingState::PlayerWalkingState()
    : m_spriteRight(ResourceManager::instance().getTexture(Constants::Sprite::CuteGirlWalkRight_PNG), tileWidth, tileHeight)
    , m_spriteLeft(ResourceManager::instance().getTexture(Constants::Sprite::CuteGirlWalkLeft_PNG), tileWidth, tileHeight)
    , m_spriteCurrent(nullptr)
{
}

void PlayerWalkingState::setDirection(HorizontalDirection dir)
{
    m_spriteCurrent = dir == HorizontalDirection::Right ? &m_spriteRight : &m_spriteLeft;
}

void PlayerWalkingState::enter(Player& owner)
{
    std::cout << "PlayerWalkingState::enter\n";
    m_spriteRight.reset(owner.m_pos);
    m_spriteLeft.reset(owner.m_pos);
    setDirection(owner.m_direction);
}

void PlayerWalkingState::exit(Player& owner)
{
    //std::cout << "PlayerWalkingState::exit\n";
}

void PlayerWalkingState::update(Player& owner, float dt)
{
    setDirection(owner.m_direction);
    handleAcceleration(owner, dt);
    applyFriction(owner, dt);
    clampVelocity(owner);

    if (owner.m_vel.x == 0 && owner.m_vel.y == 0)
    {
        owner.changeState(std::make_unique<PlayerIdleState>());
    }
    else
    {
        m_spriteCurrent->update(dt);
        owner.m_pos += owner.m_vel * dt;
        m_spriteCurrent->sprite().setPosition(owner.m_pos);
    }
}

void PlayerWalkingState::draw(Player& owner, sf::RenderWindow& window)
{
    m_spriteCurrent->draw(window);
}

PlayerStateId PlayerWalkingState::stateId() const
{
    return PlayerStateId::Walking;
}

void PlayerWalkingState::handleAcceleration(Player& owner, float dt)
{
    constexpr float acceleration = 50000.f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        owner.m_vel.x = -acceleration * dt;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        owner.m_vel.x = acceleration * dt;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        owner.m_vel.y = -acceleration * dt;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        owner.m_vel.y = acceleration * dt;
}

void PlayerWalkingState::applyFriction(Player& owner, float dt)
{
    constexpr float friction = 1000.f;

    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Left) &&
        !sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        if (owner.m_vel.x > 0)
            owner.m_vel.x = std::max(0.f, owner.m_vel.x - friction * dt);
        else if (owner.m_vel.x < 0)
            owner.m_vel.x = std::min(0.f, owner.m_vel.x + friction * dt);
    }

    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Up) &&
        !sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        if (owner.m_vel.y > 0)
            owner.m_vel.y = std::max(0.f, owner.m_vel.y - friction * dt);
        else if (owner.m_vel.y < 0)
            owner.m_vel.y = std::min(0.f, owner.m_vel.y + friction * dt);
    }
}

void PlayerWalkingState::clampVelocity(Player& owner)
{
    constexpr float maxSpeed = 100.f;

    owner.m_vel.x = std::clamp(owner.m_vel.x, -maxSpeed, maxSpeed);
    owner.m_vel.y = std::clamp(owner.m_vel.y, -maxSpeed, maxSpeed);
}
