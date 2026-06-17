#include "Player.h"
#include "ResourceManager.h"
#include "PlayerWalkingState.h"
#include "PlayerIdleState.h"

#include <iostream>


PlayerIdleState::PlayerIdleState()
    : m_sprite_right(ResourceManager::instance().getTexture("resources/sprites/cutegirl_idle_right.png"), tileWidth, tileHeight)
    , m_sprite_left(ResourceManager::instance().getTexture("resources/sprites/cutegirl_idle_left.png"), tileWidth, tileHeight)
    , m_sprite_current(nullptr)
{
}

void PlayerIdleState::setDirection(HorizontalDirection dir)
{
    m_sprite_current = dir == HorizontalDirection::Right ? &m_sprite_right : &m_sprite_left;
}

void PlayerIdleState::enter(Player& owner)
{
    std::cout << "PlayerIdleState::enter\n";
    m_sprite_right.reset(owner.m_pos);
    m_sprite_left.reset(owner.m_pos);
    setDirection(owner.m_direction);
}

void PlayerIdleState::exit(Player& owner)
{
    //std::cout << "PlayerIdleState::exit\n";
}

void PlayerIdleState::update(Player& owner, float dt)
{
    m_sprite_current->update(dt);

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
    m_sprite_current->draw(window);
}
