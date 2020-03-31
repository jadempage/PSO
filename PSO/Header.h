#pragma once
#include <stdbool.h>


struct coords {
	int x;
	int y;
};

struct hotSpot {
	bool hasMiddle = false;
	bool hasInner = false;
	coords innerCoords;
	int innerRadius;
	coords middleCoords;
	int middleRadius;
	coords outerCoords;
	int outerRadius;
};

std::vector<hotSpot> hotSpots;