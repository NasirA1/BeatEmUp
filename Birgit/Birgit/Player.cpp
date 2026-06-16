#include "Player.h"
#include <SFML/Graphics.hpp>
#include "PlayerIdleState.h"

Player::Player(const sf::Vector2f& pos, HorizontalDirection direction)
    : m_pos(pos)
    , m_direction(direction)
    , m_vel(0, 0)
{
    m_stateMachine.changeState(*this, std::make_unique<PlayerIdleState>());
}

void Player::update(float dt)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        m_direction = HorizontalDirection::Left;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        m_direction = HorizontalDirection::Right;

    m_stateMachine.update(*this, dt);
}

void Player::changeState(std::unique_ptr<PlayerState> state)
{
    m_stateMachine.changeState(*this, std::move(state));
}

void Player::draw(sf::RenderWindow& window)
{
    m_stateMachine.draw(*this, window);
}

sf::Vector2f Player::position() const
{
    return m_pos;
}

void Player::setPosition(sf::Vector2f pos)
{
    m_pos = pos;
}