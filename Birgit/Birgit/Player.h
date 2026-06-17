#pragma once
#include "Core.h"
#include <SFML/Graphics.hpp>

enum class PlayerStateId
{
    Idle,
    Walking,
    Jumping,
    Dead
};

class Player
{
public:
    Player(const sf::Vector2f& pos = {0, 0}, HorizontalDirection direction = HorizontalDirection::Right);
    void update(float dt);
    void changeState(std::unique_ptr<PlayerState> state);
    void draw(sf::RenderWindow& window);
    sf::Vector2f position() const;
    void setPosition(sf::Vector2f pos);
    PlayerStateId currentState() const;

private:
    StateMachine<Player, PlayerStateId> m_stateMachine;
    sf::Vector2f m_pos;
    HorizontalDirection m_direction;
    sf::Vector2f m_vel;

    friend class PlayerIdleState;
    friend class PlayerWalkingState;
    friend class PlayerJumpState;
    friend class PlayerDeadState;
};

