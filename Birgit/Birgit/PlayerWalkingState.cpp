#include "PlayerWalkingState.h"

#include "ResourceManager.h"
#include "Player.h"
#include "PlayerIdleState.h"
#include "Constants.h"
#include <iostream>

PlayerWalkingState::PlayerWalkingState()
    : m_running(false)
    , m_spriteWalkRight(ResourceManager::instance().getTexture(Constants::Sprite::CuteGirlWalkRight_PNG), tileWidth, tileHeight)
    , m_spriteWalkLeft(ResourceManager::instance().getTexture(Constants::Sprite::CuteGirlWalkLeft_PNG), tileWidth, tileHeight, true)
    , m_spriteRunRight(ResourceManager::instance().getTexture(Constants::Sprite::CuteGirlRunRight_PNG), tileWidth, tileHeight)
    , m_spriteRunLeft(ResourceManager::instance().getTexture(Constants::Sprite::CuteGirlRunLeft_PNG), tileWidth, tileHeight, true)
    , m_spriteCurrent(nullptr)
{
}

void PlayerWalkingState::setDirection(HorizontalDirection dir)
{
    if (m_running)
    { 
        m_spriteCurrent = dir == HorizontalDirection::Right ? &m_spriteRunRight : &m_spriteRunLeft;
    }
    else
    {
        m_spriteCurrent = dir == HorizontalDirection::Right ? &m_spriteWalkRight : &m_spriteWalkLeft;
    }
}

void PlayerWalkingState::enter(Player& owner)
{
    std::cout << "PlayerWalkingState::enter\n";
    m_spriteWalkRight.reset(owner.m_pos);
    m_spriteWalkLeft.reset(owner.m_pos);
    m_spriteRunRight.reset(owner.m_pos);
    m_spriteRunLeft.reset(owner.m_pos);
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
    return m_running? PlayerStateId::Running :PlayerStateId::Walking;
}

void PlayerWalkingState::handleAcceleration(Player& owner, float dt)
{
    const auto acceleration = m_running ? runningAcceleration : walkingAcceleration;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        owner.m_vel.x = -acceleration * dt;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        owner.m_vel.x = acceleration * dt;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        owner.m_vel.y = -acceleration * dt;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        owner.m_vel.y = acceleration * dt;

    if ( (std::abs(owner.m_vel.x) > 0 || std::abs(owner.m_vel.y) > 0) 
        && sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        m_running = true;
        setDirection(owner.m_direction);
    }
    else
    {
        m_running = false;
        setDirection(owner.m_direction);
    }
}

void PlayerWalkingState::applyFriction(Player& owner, float dt)
{
    const auto friction = m_running ? runningFriction : walkingFriction;

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
    const auto maxSpeed = m_running ? runningMaxSpeed : walkingMaxSpeed;

    owner.m_vel.x = std::clamp(owner.m_vel.x, -maxSpeed, maxSpeed);
    owner.m_vel.y = std::clamp(owner.m_vel.y, -maxSpeed, maxSpeed);
}
