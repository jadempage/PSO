#pragma once


struct coords {
	int x;
	int y;
};

struct hotSpot {
	coords innerCoords;
	int innerW;
	int innerH;
	coords middleCoords;
	int middleW;
	int middleH;
	coords outerCoords;
	int outerW;
	int outerH;
};