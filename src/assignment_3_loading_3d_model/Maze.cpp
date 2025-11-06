#include "Maze.hpp"
#include <random>
#include <algorithm>

Maze::Maze(int width, int height)
    : w(width), h(height), cells(height, std::vector<Cell>(width))
{
  // Initialize all walls as closed and unvisited
  for (int y = 0; y < h; ++y)
    for (int x = 0; x < w; ++x)
    {
      cells[y][x].up = true;
      cells[y][x].down = true;
      cells[y][x].left = true;
      cells[y][x].right = true;
      cells[y][x].visited = false;
    }
}

int Maze::width() const { return w; }
int Maze::height() const { return h; }

const Maze::Cell &Maze::getCell(int x, int y) const
{
  return cells[y][x];
}

void Maze::generate()
{
  std::mt19937 rng(std::random_device{}());
  std::vector<std::pair<int, int>> stack;
  stack.push_back({0, 0});
  cells[0][0].visited = true;

  while (!stack.empty())
  {
    auto [cx, cy] = stack.back();

    // Collect unvisited neighbors
    std::vector<std::pair<int, int>> neighbors;
    if (cy > 0 && !cells[cy - 1][cx].visited)
      neighbors.push_back({cx, cy - 1});
    if (cy < h - 1 && !cells[cy + 1][cx].visited)
      neighbors.push_back({cx, cy + 1});
    if (cx > 0 && !cells[cy][cx - 1].visited)
      neighbors.push_back({cx - 1, cy});
    if (cx < w - 1 && !cells[cy][cx + 1].visited)
      neighbors.push_back({cx + 1, cy});

    if (!neighbors.empty())
    {
      auto [nx, ny] = neighbors[rng() % neighbors.size()];

      // Knock down walls between (cx, cy) and (nx, ny)
      if (nx == cx && ny == cy - 1)
      {
        cells[cy][cx].up = false;
        cells[ny][nx].down = false;
      }
      else if (nx == cx && ny == cy + 1)
      {
        cells[cy][cx].down = false;
        cells[ny][nx].up = false;
      }
      else if (nx == cx - 1 && ny == cy)
      {
        cells[cy][cx].left = false;
        cells[ny][nx].right = false;
      }
      else if (nx == cx + 1 && ny == cy)
      {
        cells[cy][cx].right = false;
        cells[ny][nx].left = false;
      }

      cells[ny][nx].visited = true;
      stack.push_back({nx, ny});
    }
    else
    {
      stack.pop_back();
    }
  }
}
