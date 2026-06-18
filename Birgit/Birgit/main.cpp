#include "Player.h"
#include "JsonTileMap.h"
#include <SFML/Graphics.hpp>
#include <cassert>
#include <fstream>
#include <iostream>
#include "Constants.h"
#include "PlayerJumpState.h"
#include "PlayerDeadState.h"


constexpr size_t SCREEN_WIDTH = 1280;
constexpr size_t SCREEN_HEIGHT = 736;


int main(int argc, char* argv[])
{
    // Enable antialiasing by requesting 8 levels (or 4, depending on GPU support)
    sf::ContextSettings settings;
    // note: slows down performance. set to 0 when fps too low!
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Sara's Game", sf::Style::Default, settings);
	window.setFramerateLimit(60);
    
    Player player({ 1280.f / 2.f, 350.f });
    sf::Clock clock;

    JsonTileMap map;
    if (!map.load(Constants::Tileset::TinySwordsMap_JSON, Constants::Tileset::TinySwordsMap_PNG))
    {
        return 1;
    }

    const int WORLD_WIDTH = map.pixelWidth();
    const int WORLD_HEIGHT = map.pixelHeight();

    sf::View camera;
    camera.setSize({ SCREEN_WIDTH, SCREEN_HEIGHT });
    camera.setCenter({ SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f });
    window.setView(camera);

    #pragma region DEBUG
#ifdef _DEBUG
    sf::RectangleShape tileHighlight;
    tileHighlight.setSize({
        static_cast<float>(map.tileSize()),
        static_cast<float>(map.tileSize())
        });
    tileHighlight.setFillColor(sf::Color::Transparent);
    tileHighlight.setOutlineColor(sf::Color::Red);
    tileHighlight.setOutlineThickness(2.f);
    sf::Font debugFont;
    if (!debugFont.loadFromFile("C:\\Windows\\Fonts\\arial.ttf"))
    {
        std::cerr << "Failed to load debug font\n";
        return 1;
    }
    bool showDebugGrid = false;
#endif
#pragma endregion

    auto clampCameraToWorld = [WORLD_WIDTH, WORLD_HEIGHT, &player](sf::View& camera, sf::Vector2f target)
    {
        const float halfW = camera.getSize().x / 2.f;
        const float halfH = camera.getSize().y / 2.f;

        target.x = std::clamp(target.x, halfW, static_cast<float>(WORLD_WIDTH) - halfW);
        target.x = std::round(target.x);
        target.y = std::clamp(target.y, halfH, static_cast<float>(WORLD_HEIGHT) - halfH);
        target.y = std::round(target.y);

        camera.setCenter(target);
    };

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyReleased)
            {
                if (event.key.code == sf::Keyboard::Key::Escape)
                {
                    window.close();
                }
                if (event.key.code == sf::Keyboard::F3)
                {
                    #pragma region DEBUG
#ifdef _DEBUG
                    showDebugGrid = !showDebugGrid;
#endif
#pragma endregion
                }
                if (event.key.code == sf::Keyboard::Space)
                {
                    player.changeState(std::make_unique<PlayerJumpState>());
                }
            }
        }

        sf::Vector2f oldPos = player.position();
        player.update(dt);

        // Bounds handling
        auto feet = player.position();
        feet.y += 50;
        if (map.isBlockedAt(feet))
        {
            player.setPosition(oldPos);
        }
        if (map.layerNameAt(feet) == "Background" 
            && player.currentState() != PlayerStateId::Dead && player.currentState() != PlayerStateId::Jumping)
        {
            player.changeState(std::make_unique<PlayerDeadState>());
        }

        #pragma region DEBUG
#ifdef _DEBUG
        feet = player.position();
        feet.y += 50;
        auto [col, row] = map.tileCoordsAt(feet);
        int tileId = map.tileIdAt(feet);
        std::cout
            << "Tile=(" << col << "," << row << ") "
            << "TileId=" << tileId
            << "Layer=" << map.layerNameAt(feet)
            << "      \r";
        std::cout.flush();
#endif
#pragma endregion

        clampCameraToWorld(camera, player.position());
        window.setView(camera);
        window.clear();
        window.draw(map);
        
        #pragma region DEBUG
#ifdef _DEBUG
        if (showDebugGrid)
        {
            Debug::drawTileDebugOverlay(window, map, camera, debugFont);
            tileHighlight.setPosition({
                static_cast<float>(col * map.tileSize()),
                static_cast<float>(row * map.tileSize())
                });
            if (tileId != -1)
            {
                window.draw(tileHighlight);
            }
        }
#endif
#pragma endregion

        player.draw(window);

        window.display();

        //float fps = 1.0f / dt;
        //window.setTitle("Birgit - FPS: " + std::to_string(static_cast<int>(fps)));
    }

	std::cout << "Goodbye!" << std::endl;
	return 0;
}
