#include "SpriteSheet.h"
#include <SFML/Graphics.hpp>

SpriteSheet::SpriteSheet(const sf::Texture& texture, size_t tileWidth, size_t tileHeight)
    : m_sprite(texture)
    , m_tileWidth(tileWidth)
    , m_tileHeight(tileHeight)
    , m_spriteCol(0)
    , m_spriteRow(0)
{
    m_sprite.setTextureRect(sf::IntRect(m_spriteCol * m_tileWidth, m_spriteRow * m_tileHeight, m_tileWidth, m_tileHeight));
    m_sprite.setScale(0.2f, 0.2f);  // 20% size
    m_sprite.setOrigin(tileWidth / 2.f, tileHeight / 2.f);
}

void SpriteSheet::reset(const sf::Vector2f pos)
{
    m_spriteCol = m_spriteRow = 0;
    m_sprite.move(pos);
}

void SpriteSheet::update(float dt)
{
    m_sprite.setTextureRect(sf::IntRect(m_spriteCol * m_tileWidth, m_spriteRow * m_tileHeight, m_tileWidth, m_tileHeight));

    // accumulate elapsed time and only advance the sprite when enough time has passed.
    constexpr float FrameDuration = 0.05f; // 10 FPS animation
    m_animationTimer += dt;
    while (m_animationTimer >= FrameDuration)
    {
        constexpr int NumFrames = 16;
        m_spriteCol = (m_spriteCol + 1) % NumFrames;
        m_animationTimer -= FrameDuration;
    }
}

void SpriteSheet::draw(sf::RenderWindow& window)
{
    window.draw(m_sprite);
}
