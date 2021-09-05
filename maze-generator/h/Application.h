#ifndef APPLICATION_H
#define APPLICATION_H

#include "Globals.h"
#include "PixelGrid.h"

#include <SFML/Graphics.hpp>
#include <stack>
#include <iostream>

class Application
{
public:
	Application();
	~Application();
	void run();

private:
	enum
	{
		PATH_N = 0x01,
		PATH_E = 0x02,
		PATH_S = 0x04,
		PATH_W = 0x08,
		VISITED = 0x10
	};

	sf::RenderWindow m_window;
	bool m_exitGame{ false };
	sf::Font m_font;
	int *m_maze;
	int  m_visitedCells;
	std::stack<std::pair<int, int>> m_stack;
	int m_pathWidth;
	PixelGrid m_pixelGrid{ Globals::MAZE_WIDTH * 2, Globals::MAZE_HEIGHT * 2};
	int m_updateRate = 30;
	sf::Time m_timePerFrame;

	void processEvents();
	void update();
	void generateMaze();
	void initialiseMaze();
	void draw();
	void drawString(int t_x, int t_y, std::string t_string, sf::Color t_colour, int t_size = 20);
};

#endif // !APPLICATION_H