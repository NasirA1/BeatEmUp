#include "SpriteSheet.h"
#include <SFML/Graphics.hpp>
#include <iostream>

SpriteSheet::SpriteSheet(const sf::Texture& texture, size_t tileWidth, size_t tileHeight, bool playReverse)
    : m_sprite(texture)
    , m_tileWidth(tileWidth)
    , m_tileHeight(tileHeight)
    , m_playReverse(playReverse)
    , m_spriteCount(m_sprite.getTexture()->getSize().x / m_tileWidth)
    , m_spriteCol(0)
    , m_spriteRow(0)
{
    m_sprite.setTextureRect(sf::IntRect(m_spriteCol * m_tileWidth, m_spriteRow * m_tileHeight, m_tileWidth, m_tileHeight));
    m_sprite.setScale(0.2f, 0.2f);  // 20% size
    m_sprite.setOrigin(tileWidth / 2.f, tileHeight / 2.f);
}

void SpriteSheet::reset(const sf::Vector2f pos)
{
    if (m_playReverse)
    {
        m_spriteCol = m_spriteCount - 1;
        m_spriteRow = 0;
    }
    else
    {
        m_spriteCol = m_spriteRow = 0;
    }
    //m_sprite.move(pos);
    m_sprite.setPosition(pos);
}

void SpriteSheet::update(float dt)
{
    // accumulate elapsed time and only advance the sprite when enough time has passed.
    constexpr float FrameDuration = 0.05f; // 5 FPS animation

    m_animationTimer += dt;

    while (m_animationTimer >= FrameDuration)
    {
        if (m_playReverse)
        {
            if (m_spriteCol == 0)
                m_spriteCol = m_spriteCount - 1;
            else
                --m_spriteCol;
        }
        else
        {
            m_spriteCol = (m_spriteCol + 1) % m_spriteCount;
        }

        m_animationTimer -= FrameDuration;
    }

    m_sprite.setTextureRect(sf::IntRect(
        m_spriteCol * m_tileWidth,
        m_spriteRow * m_tileHeight,
        m_tileWidth,
        m_tileHeight
    ));
}

void SpriteSheet::draw(sf::RenderWindow& window)
{
    window.draw(m_sprite);
}
