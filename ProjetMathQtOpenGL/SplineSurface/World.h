#ifndef WORLD_H
#define WORLD_H
class World
{
public:
	World();
	~World();
	int* getMaps() { return new int; };
	void setMaps(int* maps) { World::maps = maps;  };
	const int WIDTH_ZONE = 100, HEIGHT_ZONE = 100, ZONES_X = 100, ZONES_Y = 100;
private:
	int* maps;
};
#endif
