#pragma once
#include "Core.h"
#include <SFML/Graphics.hpp>


class Player
{
public:
    Player(const sf::Vector2f& pos = {0, 0}, HorizontalDirection direction = HorizontalDirection::Right);
    void update(float dt);
    void changeState(std::unique_ptr<PlayerState> state);
    void draw(sf::RenderWindow& window);
    sf::Vector2f position() const;
    void setPosition(sf::Vector2f pos);

private:
    StateMachine<Player> m_stateMachine;
    sf::Vector2f m_pos;
    HorizontalDirection m_direction;
    sf::Vector2f m_vel;

    friend class PlayerIdleState;
    friend class PlayerWalkingState;
};

