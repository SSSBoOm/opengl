#pragma once
#include <vector>

class Maze
{
public:
  struct Cell
  {
    bool up, down, left, right;
    bool visited;
  };

  Maze(int width, int height);
  void generate();
  const Cell &getCell(int x, int y) const;
  int width() const;
  int height() const;

private:
  int w, h;
  std::vector<std::vector<Cell>> cells;
};
