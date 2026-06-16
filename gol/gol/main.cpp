#include <iostream>
#include <vector>
#include <random>
#include <array>
#include <SFML/Graphics.hpp>
#include <SFML/Main.hpp>


void draw_grid(const std::vector<std::vector<int>>& grid, sf::RenderWindow& window)
{
    const int rows = grid.size();
    const int cols = grid[0].size();
    constexpr int cellSize = 10;
    constexpr int padding = 10;

    sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));

    for (int row = 0; row < rows; ++row)
    {
        for (int col = 0; col < cols; ++col) 
        {
            cell.setPosition(padding + col * cellSize, padding + row * cellSize);
            if (grid[row][col] == 1)
            {
                cell.setFillColor(sf::Color::Black);
            }
            else
            {
                cell.setFillColor(sf::Color::White);
            }

            window.draw(cell);
        }
    }

    sf::VertexArray lines(sf::Lines);
    const float gridWidth = cols * cellSize;
    const float gridHeight = rows * cellSize;

    // Vertical lines
    const sf::Color gridLinesColour(200, 200, 200);
    for (int x = 0; x <= cols; ++x) 
    {
        float px = padding + x * cellSize;

        lines.append(sf::Vertex(sf::Vector2f(px, padding), gridLinesColour));
        lines.append(sf::Vertex(sf::Vector2f(px, padding + gridHeight), gridLinesColour));
    }

    // Horizontal lines
    for (int y = 0; y <= rows; ++y) 
    {
        float py = padding + y * cellSize;
        lines.append(sf::Vertex(sf::Vector2f(padding, py), gridLinesColour));
        lines.append(sf::Vertex(sf::Vector2f(padding + gridWidth, py), gridLinesColour));
    }

    window.draw(lines);
}


// Direction deltas
constexpr std::array<std::pair<int, int>, 8> Directions
{{
   // y, x
    {-1, 0}, // North
    {-1, 1}, // North-east
    { 0, 1}, // East
    { 1, 1},  // South-east
    { 1, 0}, // South
    { 1,-1}, // South-west
    { 0,-1}, // West
    {-1,-1},  // North-west
}};


bool is_valid(int row, int col, const std::vector<std::vector<int>>& grid)
{
    return row > -1 && row < grid.size()
        && col > -1 && col < grid[row].size();
}

std::vector<std::vector<int>> next_grid(const std::vector<std::vector<int>>& grid)
{
    std::vector<std::vector<int>> next(grid.size(), std::vector<int>(grid[0].size(), 0));

    for (int row = 0; row < grid.size(); ++row)
    {
        for (int col = 0; col < grid[0].size(); ++col)
        {
            int neighbours = 0;
            for (const auto& dir : Directions)
            {
                const auto& [dir_y, dir_x] = dir;
                auto next_row = row + dir_y;
                auto next_col = col + dir_x;
                if (is_valid(next_row, next_col, grid))
                {
                    neighbours += grid[next_row][next_col];
                }
            }

            if (grid[row][col] == 1)
            {
                next[row][col] = (neighbours == 2 || neighbours == 3) ? 1 : 0;
            }
            else
            {
                next[row][col] = (neighbours == 3) ? 1 : 0;
            }
        }
    }

    return next;
}

void init_grid(std::vector<std::vector<int>>& grid)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution dist(0.6); // 30% chance of 1
    for (auto& row : grid)
    {
        for (auto& cell : row)
        {
            cell = dist(gen);
        }
    }
}


int main(int argc, char** argv) 
{
    constexpr int rows = 40;
    constexpr int cols = 70;
    constexpr int cellSize = 10;
    constexpr int padding = 10;

    std::vector<std::vector<int>> grid(rows, std::vector<int>(cols, 0));
    init_grid(grid);

    sf::RenderWindow window(sf::VideoMode(cols * cellSize + 2 * padding, rows * cellSize + 2 * padding), "GOL");
    window.setFramerateLimit(60);

    while (window.isOpen()) 
    {
        sf::Event event;
        while (window.pollEvent(event)) 
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyReleased)
            {
                if (event.key.code == sf::Keyboard::Space)
                {
                    init_grid(grid);
                }
            }
        }

        grid = next_grid(grid);

        window.clear(sf::Color::White);
        draw_grid(grid, window);

        window.display();
    }

    return 0;
}
