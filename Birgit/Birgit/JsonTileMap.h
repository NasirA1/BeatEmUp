#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>


class JsonTileMap : public sf::Drawable
{
public:
    struct Tile
    {
        int id = -1;
        int x = 0;
        int y = 0;
    };

    struct Layer
    {
        std::string name;
        bool collider = false;
        std::vector<Tile> tiles;
        sf::VertexArray vertices;
    };

public:
    bool load(const std::string& jsonPath, const std::string& texturePath);
    bool isBlockedAt(sf::Vector2f worldPos) const;
    int tileIdAt(sf::Vector2f worldPos) const;
    std::string layerNameAt(sf::Vector2f worldPos) const;
    std::pair<int, int> tileCoordsAt(sf::Vector2f worldPos) const;
    const Layer* layerByName(const std::string& name) const;

    const std::vector<Layer>& layers() const { return m_layers; }
    int pixelWidth() const { return m_mapWidth * m_tileSize; }
    int pixelHeight() const { return m_mapHeight * m_tileSize; }
    int mapWidth() const { return m_mapWidth; }
    int mapHeight() const { return m_mapHeight; }
    int tileSize() const { return m_tileSize; }

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    sf::Texture m_texture;
    std::vector<Layer> m_layers;
    std::vector<int> m_tileIds;
    std::vector<std::string> m_layerNames;
    int m_tileSize = 64;
    int m_mapWidth = 0;
    int m_mapHeight = 0;
};

namespace Debug
{
    void drawTileDebugOverlay(
        sf::RenderWindow& window, 
        const JsonTileMap& map, 
        const sf::View& camera, 
        const sf::Font& font
    );
}
