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
    
    sf::Sprite& sprite() { return m_sprite;  }
    int currentSprite() const { return m_spriteCol; }  // Note Single row sprites currently
    size_t spriteCount() const { return m_spriteCount; }

private:
    sf::Sprite m_sprite;

public:
    const std::size_t m_tileWidth;
    const std::size_t m_tileHeight;

private:
    size_t m_spriteCount;
    int m_spriteCol;
    int m_spriteRow;
    float m_animationTimer = 0.f;
};
