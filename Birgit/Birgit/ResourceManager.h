#pragma once

#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <stdexcept>

class ResourceManager
{
public:
    static ResourceManager& instance()
    {
        static ResourceManager manager;
        return manager;
    }

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    sf::Texture& getTexture(const std::string& filename);
    void clear();

private:
    ResourceManager() = default;
    ~ResourceManager() = default;

private:
    std::unordered_map<std::string, sf::Texture> m_textures;
};
