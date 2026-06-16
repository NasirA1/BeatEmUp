#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>

class Player
{
public:
    Player(const sf::Vector2f& position, const sf::Vector2u& worldSize)
        : sprite(20.0)
        , vel(0, 0)
        , worldSize(worldSize)
    {
        sprite.setFillColor(sf::Color::Black);
        sprite.setPosition(position);
    }

    void update(float dt)
    {
        handleAcceleration(dt);
        applyFriction(dt);
        clampVelocity();
        sprite.move(vel * dt);
        keepInsideWorld();
    }

    void draw(sf::RenderWindow& window)
    {
        window.draw(sprite);
    }

private:
    void handleAcceleration(float dt)
    {
        constexpr float acceleration = 500.f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            vel.x -= acceleration * dt;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            vel.x += acceleration * dt;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            vel.y -= acceleration * dt;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            vel.y += acceleration * dt;
    }

    void applyFriction(float dt)
    {
        constexpr float friction = 1000.f;

        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Left) &&
            !sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            if (vel.x > 0)
                vel.x = std::max(0.f, vel.x - friction * dt);
            else if (vel.x < 0)
                vel.x = std::min(0.f, vel.x + friction * dt);
        }

        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Up) &&
            !sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            if (vel.y > 0)
                vel.y = std::max(0.f, vel.y - friction * dt);
            else if (vel.y < 0)
                vel.y = std::min(0.f, vel.y + friction * dt);
        }
    }

    void clampVelocity()
    {
        constexpr float maxSpeed = 1000.f;

        vel.x = std::clamp(vel.x, -maxSpeed, maxSpeed);
        vel.y = std::clamp(vel.y, -maxSpeed, maxSpeed);
    }

    void keepInsideWorld()
    {
        auto pos = sprite.getPosition();
        const float diameter = sprite.getRadius() * 2.f;

        if (pos.x < 0.f)
        {
            pos.x = 0.f;
            vel.x = 0.f;
        }
        else if (pos.x + diameter > worldSize.x)
        {
            pos.x = worldSize.x - diameter;
            vel.x = 0.f;
        }

        if (pos.y < 0.f)
        {
            pos.y = 0.f;
            vel.y = 0.f;
        }
        else if (pos.y + diameter > worldSize.y)
        {
            pos.y = worldSize.y - diameter;
            vel.y = 0.f;
        }

        sprite.setPosition(pos);
    }

    sf::CircleShape sprite;
    sf::Vector2f vel;
    sf::Vector2u worldSize;
};


int main(int argc, char* argv[])
{
	std::cout << "hello" << std::endl;

    // Enable antialiasing by requesting 8 levels (or 4, depending on GPU support)
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Smooth", sf::Style::Default, settings);
	window.setFramerateLimit(60);
    
    Player player({ (window.getSize().x / 2) - 10.0f, (window.getSize().y / 2) - 10.0f }, window.getSize());
    sf::Clock clock;

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        player.update(dt);

        window.clear(sf::Color::White);
        player.draw(window);

        window.display();
    }

	std::cout << "Goodbye!" << std::endl;
	return 0;
}
