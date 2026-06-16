#include "ResourceManager.h"

sf::Texture& ResourceManager::getTexture(const std::string& filename)
{
    auto it = m_textures.find(filename);

    if (it != m_textures.end())
    {
        return it->second;
    }

    sf::Texture texture;

    if (!texture.loadFromFile(filename))
    {
        throw std::runtime_error("Failed to load texture: " + filename);
    }

    auto [insertedIt, success] =
        m_textures.emplace(filename, std::move(texture));

    return insertedIt->second;
}

void ResourceManager::clear()
{
    m_textures.clear();
}
