#pragma once
#include <stdbool.h>



struct sHotSpot {
	bool hasMiddle = false;
	bool hasInner = false;
	Vector2 innerCoords;
	int innerRadius;
	Vector2 middleCoords;
	int middleRadius;
	Vector2 outerCoords;
	int outerRadius;
};

struct sParticle {
	Vector2 particleCoords;
	Vector2 pBest;
	int pBestValue; 
	int pVelocity;
};

std::vector<sParticle> vParticles;
std::vector<sHotSpot> vHotSpots;
Vector2 globalBestCoords;
int globalBestValue = 0;