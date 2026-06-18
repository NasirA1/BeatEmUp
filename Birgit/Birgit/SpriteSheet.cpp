#include "SpriteSheet.h"
#include <SFML/Graphics.hpp>
#include <iostream>

SpriteSheet::SpriteSheet(const sf::Texture& texture, size_t tileWidth, size_t tileHeight, bool playReverse, bool loop)
    : m_sprite(texture)
    , m_tileWidth(tileWidth)
    , m_tileHeight(tileHeight)
    , m_playReverse(playReverse)
    , m_loop(loop)
    , m_finished(false)
    , m_spriteCount(m_sprite.getTexture()->getSize().x / m_tileWidth)
    , m_spriteCol(0)
    , m_spriteRow(0)
{
    m_sprite.setTextureRect(sf::IntRect(m_spriteCol * static_cast<int>(m_tileWidth), 
        m_spriteRow * static_cast<int>(m_tileHeight), static_cast<int>(m_tileWidth), static_cast<int>(m_tileHeight)));
    m_sprite.setOrigin(tileWidth / 2.f, tileHeight / 2.f);
}

void SpriteSheet::reset(const sf::Vector2f pos)
{
    m_finished = false;

    if (m_playReverse)
    {
        m_spriteCol = static_cast<int>(m_spriteCount) - 1;
        m_spriteRow = 0;
    }
    else
    {
        m_spriteCol = m_spriteRow = 0;
    }

    m_sprite.setPosition(pos);
}

void SpriteSheet::update(float dt)
{
    // accumulate elapsed time and only advance the sprite when enough time has passed.
    constexpr float FrameDuration = 0.05f; // 5 FPS animation

    if (m_finished)
    {
        return;
    }

    m_animationTimer += dt;

    while (m_animationTimer >= FrameDuration)
    {
        if (m_playReverse)
        {
            if (m_spriteCol == 0)
            {
                if (m_loop)
                {
                    m_spriteCol = static_cast<int>(m_spriteCount) - 1;
                }
                else
                {
                    m_finished = true;
                    break;
                }
            }
            else
            {
                --m_spriteCol;
            }
        }
        else
        {
            if (m_spriteCol == static_cast<int>(m_spriteCount) - 1)
            {
                if (m_loop)
                {
                    m_spriteCol = 0;
                }
                else
                {
                    m_finished = true;
                    break;
                }
            }
            else
            {
                ++m_spriteCol;
            }
        }

        m_animationTimer -= FrameDuration;
    }

    m_sprite.setTextureRect(sf::IntRect(
        m_spriteCol * static_cast<int>(m_tileWidth),
        m_spriteRow * static_cast<int>(m_tileHeight),
        static_cast<int>(m_tileWidth),
        static_cast<int>(m_tileHeight)
    ));
}

void SpriteSheet::draw(sf::RenderWindow& window)
{
    window.draw(m_sprite);
}
