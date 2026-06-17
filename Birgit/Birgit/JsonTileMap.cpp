#include "JsonTileMap.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include <algorithm>


bool JsonTileMap::load(const std::string& jsonPath, const std::string& texturePath)
{
	if (!m_texture.loadFromFile(texturePath))
	{
		std::cerr << "Failed to load texture: " << texturePath << "\n";
		return false;
	}

	m_texture.setSmooth(false);
	m_texture.setRepeated(false);

	std::ifstream file(jsonPath);
	if (!file)
	{
		std::cerr << "Failed to load json: " << jsonPath << "\n";
		return false;
	}

	nlohmann::json mapJson;
	file >> mapJson;

	m_tileSize = mapJson["tileSize"];
	m_mapWidth = mapJson["mapWidth"];
	m_mapHeight = mapJson["mapHeight"];

	m_layers.clear();

	m_tileIds.assign(m_mapWidth * m_mapHeight, -1);
	m_layerNames.assign(m_mapWidth * m_mapHeight, "");

	const int textureCols =
		static_cast<int>(m_texture.getSize().x) / m_tileSize;

	const auto& jsonLayers = mapJson["layers"];

	for (auto it = jsonLayers.rbegin(); it != jsonLayers.rend(); ++it)
	{
		Layer layer;
		layer.name = (*it).value("name", "");
		layer.collider = (*it).value("collider", false);
		layer.vertices.setPrimitiveType(sf::Quads);

		const auto& jsonTiles = (*it)["tiles"];

		for (const auto& jsonTile : jsonTiles)
		{
			Tile tile;
			tile.id = std::stoi(jsonTile["id"].get<std::string>());
			tile.x = jsonTile["x"];
			tile.y = jsonTile["y"];

			layer.tiles.push_back(tile);

			if (tile.x < 0 || tile.y < 0 ||
				tile.x >= m_mapWidth || tile.y >= m_mapHeight)
			{
				continue;
			}

			const int index = tile.y * m_mapWidth + tile.x;

			m_tileIds[index] = tile.id;
			m_layerNames[index] = layer.name;

			const int tilesetX = tile.id % textureCols;
			const int tilesetY = tile.id / textureCols;

			const float worldX = static_cast<float>(tile.x * m_tileSize);
			const float worldY = static_cast<float>(tile.y * m_tileSize);

			const float texX = static_cast<float>(tilesetX * m_tileSize);
			const float texY = static_cast<float>(tilesetY * m_tileSize);

			sf::Vertex quad[4];

			quad[0].position = { worldX, worldY };
			quad[1].position = { worldX + m_tileSize, worldY };
			quad[2].position = { worldX + m_tileSize, worldY + m_tileSize };
			quad[3].position = { worldX, worldY + m_tileSize };

			quad[0].texCoords = { texX, texY };
			quad[1].texCoords = { texX + m_tileSize, texY };
			quad[2].texCoords = { texX + m_tileSize, texY + m_tileSize };
			quad[3].texCoords = { texX, texY + m_tileSize };

			for (int i = 0; i < 4; ++i)
			{
				layer.vertices.append(quad[i]);
			}
		}

		m_layers.push_back(std::move(layer));
	}

	return true;
}

bool JsonTileMap::isBlockedAt(sf::Vector2f worldPos) const
{
	auto layer = layerNameAt(worldPos);
	auto tileId = tileIdAt(worldPos);
	bool blocked = layer == "Trees front" || layer == "Buildings" || layer == "Cliff" || tileId == 19 || tileId == 20;
	return blocked;
}

int JsonTileMap::tileIdAt(sf::Vector2f worldPos) const
{
	const auto [col, row] = tileCoordsAt(worldPos);

	if (col < 0 || row < 0 ||
		col >= m_mapWidth || row >= m_mapHeight)
	{
		return -1;
	}

	return m_tileIds[row * m_mapWidth + col];
}

std::string JsonTileMap::layerNameAt(sf::Vector2f worldPos) const
{
	const auto [col, row] = tileCoordsAt(worldPos);

	if (col < 0 || row < 0 ||
		col >= m_mapWidth || row >= m_mapHeight)
	{
		return "";
	}

	return m_layerNames[row * m_mapWidth + col];
}

std::pair<int, int> JsonTileMap::tileCoordsAt(sf::Vector2f worldPos) const
{
	const int col = static_cast<int>(worldPos.x) / m_tileSize;
	const int row = static_cast<int>(worldPos.y) / m_tileSize;

	return { col, row };
}

const JsonTileMap::Layer* JsonTileMap::layerByName(const std::string& name) const
{
	for (const auto& layer : m_layers)
	{
		if (layer.name == name)
			return &layer;
	}

	return nullptr;
}

void JsonTileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.texture = &m_texture;

	for (const auto& layer : m_layers)
	{
		target.draw(layer.vertices, states);
	}
}


namespace Debug
{
	void drawTileDebugOverlay(
		sf::RenderWindow& window,
		const JsonTileMap& map,
		const sf::View& camera,
		const sf::Font& font)
	{
		const int tileSize = map.tileSize();

		const sf::Vector2f center = camera.getCenter();
		const sf::Vector2f size = camera.getSize();

		const float left = center.x - size.x / 2.f;
		const float top = center.y - size.y / 2.f;
		const float right = center.x + size.x / 2.f;
		const float bottom = center.y + size.y / 2.f;

		const int startCol = std::max(0, static_cast<int>(left) / tileSize);
		const int startRow = std::max(0, static_cast<int>(top) / tileSize);

		const int endCol = std::min(
			map.mapWidth(),
			static_cast<int>(right) / tileSize + 1
		);

		const int endRow = std::min(
			map.mapHeight(),
			static_cast<int>(bottom) / tileSize + 1
		);

		sf::RectangleShape outline;
		outline.setSize({
			static_cast<float>(tileSize),
			static_cast<float>(tileSize)
			});
		outline.setFillColor(sf::Color::Transparent);
		outline.setOutlineColor(sf::Color(255, 255, 255, 120));
		outline.setOutlineThickness(1.f);

		sf::Text text;
		text.setFont(font);
		text.setCharacterSize(12);
		text.setFillColor(sf::Color::White);
		text.setOutlineColor(sf::Color::Black);
		text.setOutlineThickness(1.f);

		for (int row = startRow; row < endRow; ++row)
		{
			for (int col = startCol; col < endCol; ++col)
			{
				const float x = static_cast<float>(col * tileSize);
				const float y = static_cast<float>(row * tileSize);

				outline.setPosition(x, y);
				window.draw(outline);

				const int tileId = map.tileIdAt({
					x + tileSize / 2.f,
					y + tileSize / 2.f
					});

				text.setString(std::to_string(tileId));
				text.setPosition(x + 4.f, y + 4.f);

				window.draw(text);
			}
		}
	}
}