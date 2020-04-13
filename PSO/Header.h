#pragma once
#include <stdbool.h>



//struct sHotSpot {
//	bool hasMiddle = false;
//	bool hasInner = false;
//	Vector2 innerCoords;
//	int innerRadius;
//	Vector2 middleCoords;
//	int middleRadius;
//	Vector2 outerCoords;
//	int outerRadius;
//};

struct sHotSpot {
	int tier;
	Vector2 coords;
	int radius;
};

struct sParticle {
	Vector2 particleCoords;
	Vector2 pBest;
	int pBestValue; 
	Vector2 pVelocity;
	bool isNewborn;
	bool isDying;
	//Days lived? 
};

struct sBarrier {
	Vector2 vStart;
	Vector2 vFin;
};

std::vector<sParticle> vParticles;
std::vector<sHotSpot> vHotSpots;
Vector2 globalBestCoords;
int globalBestValue = 0;
bool globalBestFound = false;
std::vector<sBarrier> vBarriers;