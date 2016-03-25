#pragma once
class World
{
public:
	World(){ maps = new int; };
	~World();
	static int* maps;
	static int* getMaps() { return maps; };
	const static int WIDTH_ZONE = 100, HEIGHT_ZONE = 100, ZONES_X = 100, ZONES_Y = 100;
};

