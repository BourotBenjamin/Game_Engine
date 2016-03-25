#ifndef WORLD_H
#define WORLD_H
class World
{
public:
	int* getMaps() { return maps; };
	void setMaps(int* maps) { this->maps = maps;  };
	const int WIDTH_ZONE = 100, HEIGHT_ZONE = 100, ZONES_X = 100, ZONES_Y = 100, NB_MAX_OBJ = 100;
private:
	int* maps;
};
#endif
