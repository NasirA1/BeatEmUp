#pragma once
#include <SFML/Graphics.hpp>
#include <cstdlib>

class SpriteSheet
{
public:
    SpriteSheet(const sf::Texture& texture, std::size_t tileWidth, std::size_t tileHeight);
    void reset(const sf::Vector2f pos);
    void update(float dt);
    void draw(sf::RenderWindow& window);

    sf::Sprite m_sprite;
    const std::size_t m_tileWidth;
    const std::size_t m_tileHeight;
    int m_spriteCol;
    int m_spriteRow;
    float m_animationTimer = 0.f;
};
