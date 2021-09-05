#ifndef GLOBALS_H
#define GLOBALS_H

#include <cstdint>

class Globals
{
public:
	static const int SCREEN_WIDTH = 1280;
	static const int SCREEN_HEIGHT = 720;

	static const int WORLD_WIDTH = 320;
	static const int WORLD_HEIGHT = 180;

	static const int MAZE_WIDTH = 100;
	static const int MAZE_HEIGHT = 50;

	static const uint8_t DEFAULT_FONT[];
};

#endif // !GLOBALS_H
