#include "Application.h"

/// <summary>
/// Application constructor.
/// </summary>
Application::Application() : m_window{ sf::VideoMode{ Globals::SCREEN_WIDTH, Globals::SCREEN_HEIGHT, 32 }, "Maze Generator", sf::Style::Default }
{
	// Maze parameters
	m_maze = new int[Globals::MAZE_WIDTH * Globals::MAZE_HEIGHT];

	initialiseMaze();

	// Load font
	m_font.loadFromMemory(Globals::DEFAULT_FONT, (size_t)75864 * sizeof(uint8_t));
}

/// <summary>
/// Application destructor.
/// </summary>
Application::~Application()
{
	delete[] m_maze;
}

/// <summary>
/// Run.
/// </summary>
void Application::run()
{
	sf::Clock f_clock;
	sf::Time f_timeSinceLastUpdate = sf::Time::Zero;
	m_timePerFrame = sf::seconds(1.f / m_updateRate);

	while (m_window.isOpen() && !m_exitGame)
	{
		f_timeSinceLastUpdate += f_clock.restart();

		while (f_timeSinceLastUpdate > m_timePerFrame)
		{
			f_timeSinceLastUpdate -= m_timePerFrame;
			processEvents();
			update();
			draw();
		}		
	}
}

/// <summary>
/// Process events.
/// </summary>
void Application::processEvents()
{
	sf::Event f_event;

	while (m_window.pollEvent(f_event))
	{
		if (sf::Event::Closed == f_event.type)
		{
			m_window.close();
		}

		if (sf::Event::KeyPressed == f_event.type)
		{
			if (sf::Keyboard::Escape == f_event.key.code)
			{
				m_exitGame = true;
			}

			if (sf::Keyboard::Up == f_event.key.code)
			{
				m_updateRate += 30;

				if (m_updateRate > 300)
				{
					m_updateRate = 300;
				}

				m_timePerFrame = sf::seconds(1.f / m_updateRate);
			}
			else if (sf::Keyboard::Down == f_event.key.code)
			{
				m_updateRate -= 30;

				if (m_updateRate < 30)
				{
					m_updateRate = 30;
				}

				m_timePerFrame = sf::seconds(1.f / m_updateRate);
			}
		}

		if (sf::Event::KeyReleased == f_event.type)
		{
			if (sf::Keyboard::Space == f_event.key.code)
			{
				initialiseMaze();
			}
		}
	}
}

/// <summary>
/// Update.
/// </summary>
void Application::update()
{
	generateMaze();
}

/// <summary>
/// Generate a new maze.
/// </summary>
void Application::generateMaze()
{
	// Lambda function to calculate index
	auto offset = [&](int x, int y)
	{
		return (m_stack.top().second + y) * Globals::MAZE_WIDTH + (m_stack.top().first + x);
	};

	// Execute algorithm
	if (m_visitedCells < Globals::MAZE_WIDTH * Globals::MAZE_HEIGHT)
	{
		// Create a set of unvisted neighbours
		std::vector<int> f_neighbours;

		// North neighbour
		if (m_stack.top().second > 0 && (m_maze[offset(0, -1)] & VISITED) == 0)
		{
			f_neighbours.push_back(0);
		}

		// East neighbour
		if (m_stack.top().first < Globals::MAZE_WIDTH - 1 && (m_maze[offset(1, 0)] & VISITED) == 0)
		{
			f_neighbours.push_back(1);
		}

		// South neighbour
		if (m_stack.top().second < Globals::MAZE_HEIGHT - 1 && (m_maze[offset(0, 1)] & VISITED) == 0)
		{
			f_neighbours.push_back(2);
		}

		// West neighbour
		if (m_stack.top().first > 0 && (m_maze[offset(-1, 0)] & VISITED) == 0)
		{
			f_neighbours.push_back(3);
		}

		// Are there any neighbours available?
		if (!f_neighbours.empty())
		{
			// Choose one available neighbour at random
			int next_cell_dir = f_neighbours[rand() % f_neighbours.size()];

			// Create a path between the neighbour and the current cell
			switch (next_cell_dir)
			{
				case 0: // North
				{
					m_maze[offset(0, -1)] |= VISITED | PATH_S;
					m_maze[offset(0, 0)] |= PATH_N;
					m_stack.push(std::make_pair((m_stack.top().first + 0), (m_stack.top().second - 1)));
					break;
				}

				case 1: // East
				{
					m_maze[offset(+1, 0)] |= VISITED | PATH_W;
					m_maze[offset(0, 0)] |= PATH_E;
					m_stack.push(std::make_pair((m_stack.top().first + 1), (m_stack.top().second + 0)));
					break;
				}

				case 2: // South
				{
					m_maze[offset(0, +1)] |= VISITED | PATH_N;
					m_maze[offset(0, 0)] |= PATH_S;
					m_stack.push(std::make_pair((m_stack.top().first + 0), (m_stack.top().second + 1)));
					break;
				}

				case 3: // West
				{
					m_maze[offset(-1, 0)] |= VISITED | PATH_E;
					m_maze[offset(0, 0)] |= PATH_W;
					m_stack.push(std::make_pair((m_stack.top().first - 1), (m_stack.top().second + 0)));
					break;
				}
			}

			m_visitedCells++;
		}
		else
		{
			// No available neighbours so backtrack
			m_stack.pop();
		}
	}
}

/// <summary>
/// Initialise the maze.
/// </summary>
void Application::initialiseMaze()
{
	// Clear maze and stack
	memset(m_maze, 0x00, Globals::MAZE_WIDTH * Globals::MAZE_HEIGHT * sizeof(int));
	
	while (!m_stack.empty())
	{
		m_stack.pop();
	}

	// Random starting cell
	m_pathWidth = 1;
	int x = rand() % Globals::MAZE_WIDTH;
	int y = rand() % Globals::MAZE_HEIGHT;
	m_stack.push(std::make_pair(x, y));
	m_maze[y * Globals::MAZE_WIDTH + x] = VISITED;
	m_visitedCells = 1;
}

/// <summary>
/// Draw.
/// </summary>
void Application::draw()
{
	m_window.clear(sf::Color(6, 140, 42));

	// Draw maze
	for (int x = 0; x < Globals::MAZE_WIDTH; x++)
	{
		for (int y = 0; y < Globals::MAZE_HEIGHT; y++)
		{
			for (int py = 0; py < m_pathWidth; py++)
			{
				for (int px = 0; px < m_pathWidth; px++)
				{
					if (m_maze[y * Globals::MAZE_WIDTH + x] & VISITED)
					{
						m_pixelGrid.setPixel(x * (m_pathWidth + 1) + px, y * (m_pathWidth + 1) + py, sf::Color(26, 235, 78));
					}
					else
					{
						m_pixelGrid.setPixel(x * (m_pathWidth + 1) + px, y * (m_pathWidth + 1) + py, sf::Color(0, 95, 25));
					}
				}
			}

			// Draw passageways between cells
			for (int p = 0; p < m_pathWidth; p++)
			{
				if (m_maze[y * Globals::MAZE_WIDTH + x] & PATH_S)
				{
					m_pixelGrid.setPixel(x * (m_pathWidth + 1) + p, y * (m_pathWidth + 1) + m_pathWidth, sf::Color(26, 235, 78));
				}					

				if (m_maze[y * Globals::MAZE_WIDTH + x] & PATH_E)
				{
					m_pixelGrid.setPixel(x * (m_pathWidth + 1) + m_pathWidth, y * (m_pathWidth + 1) + p, sf::Color(26, 235, 78));
				}					
			}
		}
	}

	// Draw Unit - the top of the stack
	for (int py = 0; py < m_pathWidth; py++)
	{
		for (int px = 0; px < m_pathWidth; px++)
		{
			m_pixelGrid.setPixel(m_stack.top().first * (m_pathWidth + 1) + px, m_stack.top().second * (m_pathWidth + 1) + py, sf::Color::Green);
		}
	}			

	sf::Sprite f_pixelGridSprite(m_pixelGrid.getPixelBuffer());
	sf::RectangleShape f_rectangleBack;

	f_pixelGridSprite.scale(5, 5);
	f_pixelGridSprite.setPosition((Globals::SCREEN_WIDTH / 2) - (f_pixelGridSprite.getGlobalBounds().width / 2), (Globals::SCREEN_HEIGHT / 2) - (f_pixelGridSprite.getGlobalBounds().height / 2) - 50);

	f_rectangleBack.setSize(sf::Vector2f(f_pixelGridSprite.getGlobalBounds().width + 5, f_pixelGridSprite.getGlobalBounds().height + 5));
	f_rectangleBack.setPosition((Globals::SCREEN_WIDTH / 2) - (f_rectangleBack.getGlobalBounds().width / 2) - 2, (Globals::SCREEN_HEIGHT / 2) - (f_rectangleBack.getGlobalBounds().height / 2) - 50 - 2);
	f_rectangleBack.setFillColor(sf::Color(sf::Color(0, 36, 9)));

	m_window.draw(f_rectangleBack);
	m_window.draw(f_pixelGridSprite);
	
	drawString(10, Globals::SCREEN_HEIGHT - 30, "UPDATE RATE: " + std::to_string(m_updateRate / 30) + "x SPEED", sf::Color::Yellow);
	drawString(Globals::SCREEN_WIDTH - 185, Globals::SCREEN_HEIGHT - 30, "MAZE GENERATOR", sf::Color::White);

	m_window.display();
}

/// <summary>
/// Draws a string to a given position.
/// </summary>
/// <param name="t_x">The X position.</param>
/// <param name="t_y">The Y position.</param>
/// <param name="t_string">The string to draw.</param>
/// <param name="t_colour">The colour of the string.</param>
/// <param name="t_size">The character size (default value of 20).</param>
void Application::drawString(int t_x, int t_y, std::string t_string, sf::Color t_colour, int t_size)
{
	sf::Text f_text;

	f_text.setCharacterSize(t_size);
	f_text.setFont(m_font);
	f_text.setString(t_string);
	f_text.setPosition(t_x, t_y);
	f_text.setFillColor(t_colour);

	m_window.draw(f_text);
}