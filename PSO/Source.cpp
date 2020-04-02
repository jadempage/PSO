#include "raylib.h"
#include <stdlib.h>   
#include <vector>
#include "Header.h"
#include <stdio.h>
#include <time.h>
#include <concrt.h>
#include <random>
#include <chrono>

//Global vars b/c bad programming practice ¯\_(^_^)_/¯ 
const int screenWidth = 800;
const int screenHeight = 600;
const int fieldStartW = 160;
const int noParticles = 10; 
const int maxTurns = 50;
const int particleRad = 5;
const int minVelocity = 5;
const int maxVelocity = 10;
const int c1 = 2; //Weight of local information
const int c2 = 2; //Weight of global information

const double iCog = 1.496180; //Def cognitive weighting
const double iSoc = 1.496180; //Def social weightin
const double iInertia = 0.729844; //Def inertia

unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::mt19937 generator(seed);

#define OUTERHS       CLITERAL(Color){ 255, 204, 204, 255 }
#define MIDDLEHS       CLITERAL(Color){ 255, 128, 128, 255 }
#define INNERHS       CLITERAL(Color){ 255, 0, 0, 255 }
//Other vars



int getRand(int min, int max) {
	if (min == max) {
		return min;
	}
	if (min > max) {
		return 0;
	}
	int value = (generator() % (max - min + 1)) + min;
	return value;
}

void calculateFitness() {
	//For each particle
	//For each hotspot
	//For each spot
	//Check collision
	for (int p = 0; p < noParticles; p++) {
		for (int h = 0; h < vHotSpots.size(); h++) {
			sHotSpot curH = vHotSpots[h];
			sParticle curP = vParticles[p];
			Vector2 cParticle = curP.particleCoords;
			//Outer HS
			Vector2 cOuterHS = curH.outerCoords;
			int rOuterHS = curH.outerRadius;
			if (CheckCollisionCircles(cParticle, particleRad, cOuterHS, rOuterHS)) {
				vParticles[p].pBest = cParticle;
				vParticles[p].pBestValue = 1;
				if (globalBestValue == 0) {
					globalBestValue = 1;
					globalBestCoords = cParticle;
				}
			}
			//Middle HS
			if (curH.hasMiddle) {
				Vector2 cMiddleHS = curH.middleCoords;
				int rMiddleHS = curH.middleRadius;
				if (CheckCollisionCircles(cParticle, particleRad, cMiddleHS, rMiddleHS)) {
					vParticles[p].pBest = cParticle;
					vParticles[p].pBestValue = 2;
					if (globalBestValue == 1) {
						globalBestValue = 2;
						globalBestCoords = cParticle;
					}
				}
			}
			//Inner HS
			if (curH.hasInner) {
				Vector2 cInnerHS = curH.innerCoords;
				int rInnerHS = curH.innerRadius;
				if (CheckCollisionCircles(cParticle, particleRad, cInnerHS, rInnerHS)) {
					vParticles[p].pBest = cParticle;
					vParticles[p].pBestValue = 3;
					if (globalBestValue == 2) {
						globalBestValue = 3;
						globalBestCoords = cParticle;
					}
				}
			}
		}
	}
}

void updateVelocity() {
	/*v = v + c1 * rand * (pBest – p) + c2 * rand * (gBest – p)*/
	for (int p = 0; p < noParticles; p++) {
		sParticle curP = vParticles[p];
		Vector2 cParticle = curP.particleCoords;
		int rand1 = getRand(1, 5);
		int rand2 = getRand(1, 5);
		int pBestWeightingX = curP.pBest.x - curP.particleCoords.x;
		int pBestWeightingY = curP.pBest.y - curP.particleCoords.y;
		int gBestWeightingX = globalBestCoords.x - curP.particleCoords.x;
		int gBestWeightingY = globalBestCoords.y - curP.particleCoords.y;
		int newVelocity = (curP.pVelocity + c1 * rand1 * (pBestWeightingX + pBestWeightingY) + c2 * rand2);
		if (newVelocity > 10) newVelocity = 10;
		if (newVelocity < 0) newVelocity = 1;
		vParticles[p].pVelocity = newVelocity; 
	}
}


void updateCoordinates() {
	int maxGlobalInfluence = 2;
	int maxLocalInfluence = 2;
	for (int p = 0; p < noParticles; p++) {
		int goLeft = 0;
		int goRight = 0;
		int goUp = 0;
		int goDown = 0;
		sParticle curP = vParticles[p];
		int vTravel = curP.pVelocity;
		Vector2 cParticle = curP.particleCoords;
		int influence = getRand(1, 4);
		if (influence < 3) {
			if(globalBestValue > 0){
			//Global Influence
			int bestX = globalBestCoords.x;
			int bestY = globalBestCoords.y;
			if (cParticle.x - globalBestCoords.x > 0) {
				//Go left
				goLeft = getRand(1, vTravel);
				vTravel = vTravel - goLeft;
				vParticles[p].particleCoords.x += goLeft;
			}
			else {
				//Go Right
				goRight = getRand(1, vTravel);
				vTravel = vTravel - goLeft;
				vParticles[p].particleCoords.x -= goRight;
			}
			if (cParticle.y - globalBestCoords.y > 0) {
				//Go up
				goUp = getRand(1, vTravel);
				vTravel = vTravel - goUp;
				vParticles[p].particleCoords.y += goUp;
			}
			else {
				//Go down
				goDown = getRand(1, vTravel);
				vTravel = vTravel - goDown;
				vParticles[p].particleCoords.x -= goDown;
			}
		}
			else {
				int dir = getRand(1, 4);
				switch (dir) {
				case 1:
					//Go left
					vParticles[p].particleCoords.x = +vTravel;
					break;
				case 2:
					//Go Right
					vParticles[p].particleCoords.x = -vTravel;
					break;
				case 3:
					//Go Up
					vParticles[p].particleCoords.y = +vTravel;
					break;
				case 4:
					//Go Down
					vParticles[p].particleCoords.y = -vTravel;
					break;
				}
			}
	}
		if (influence > 2 ) {
			//Local Influence
			int bestX = curP.pBest.x;
			int bestY = curP.pBest.y;
			if (curP.pBestValue == 0) {
				int dir = getRand(1, 4);
				switch (dir) {
				case 1:
					//Go left
					vParticles[p].particleCoords.x = +vTravel;
					break;
				case 2:
					//Go Right
					vParticles[p].particleCoords.x = -vTravel;
					break;
				case 3:
					//Go Up
					vParticles[p].particleCoords.y = +vTravel;
					break;
				case 4:
					//Go Down
					vParticles[p].particleCoords.y = -vTravel;
					break;
				}
			}
			if (cParticle.x - bestX > 0) {
				//Go left
				goLeft = getRand(1, vTravel);
				vTravel = vTravel - goLeft;
				vParticles[p].particleCoords.x -= goLeft;
			}
			else {
				//Go Right
				goRight = getRand(1, vTravel);
				vTravel = vTravel - goLeft;
				vParticles[p].particleCoords.x += goRight;
			}
			if (cParticle.y - bestY > 0) {
				//Go up
				goUp = getRand(1, vTravel);
				vTravel = vTravel - goUp;
				vParticles[p].particleCoords.y += goUp;
			}
			else {
				//Go down
				goDown = getRand(1, vTravel);
				vTravel = vTravel - goDown;
				vParticles[p].particleCoords.x -= goDown;
			}
		}
		//Stay on the screen plz
		if (vParticles[p].particleCoords.x > screenWidth) vParticles[p].particleCoords.x = screenWidth - particleRad;
		if (vParticles[p].particleCoords.x < fieldStartW) vParticles[p].particleCoords.x = fieldStartW + particleRad *3 ;
		if (vParticles[p].particleCoords.y < 0) vParticles[p].particleCoords.y = particleRad;
		if (vParticles[p].particleCoords.y > screenHeight) vParticles[p].particleCoords.y = screenHeight - particleRad;
	}
}

sHotSpot createHotSpots() {

	sHotSpot retHotspot;
	Vector2 innerCords;
	Vector2 middleCords;
	Vector2 outerCords;
	//Outer Spot
	int numHSpots = getRand(1, 5);
	for (int i = 0; i < numHSpots; i++) {
		if (i == 0) {
			int outerRad = getRand(10, 100);
			int xPos = getRand(fieldStartW + outerRad, screenWidth - outerRad);
			int yPos = getRand(outerRad, screenHeight + outerRad);
			outerCords.x = xPos;
			outerCords.y = yPos;
			retHotspot.outerCoords = outerCords;
			retHotspot.outerRadius = outerRad;
		}
		if (i == 1) {
			retHotspot.hasMiddle = true;
			int xMax = retHotspot.outerCoords.x + 11;
			int xMin = retHotspot.outerCoords.x - 11;
			int yMax = retHotspot.outerCoords.y + 11;
			int yMin = retHotspot.outerCoords.y - 11;
			if (xMax > screenWidth) xMax = screenWidth - 20;
			if (xMax < fieldStartW) xMin = fieldStartW + 20;
			if (yMax > screenHeight) yMax = screenHeight - 20;
			if (yMin < 0) yMin = 20;
			middleCords.x = getRand(xMin, xMax);
			middleCords.y = getRand(yMin, yMax);
			int middleRad = getRand(7, retHotspot.outerRadius);
			retHotspot.middleCoords = middleCords;
			retHotspot.middleRadius = middleRad;
		}
		if (i == 2) {
			retHotspot.hasInner = true;
			int xMax = retHotspot.middleCoords.x + 11;
			int xMin = retHotspot.middleCoords.x - 11;
			int yMax = retHotspot.middleCoords.y + 11;
			int yMin = retHotspot.middleCoords.y - 11;
			if (xMax > screenWidth) xMax = screenWidth - 20;
			if (xMax < fieldStartW) xMin = fieldStartW + 20;
			if (yMax > screenHeight) yMax = screenHeight - 20;
			if (yMin < 0) yMin = 0;
			innerCords.x = getRand(xMin, xMax);
			innerCords.y = getRand(yMin, yMax);
			int innerRad = getRand(5, retHotspot.middleRadius);
			retHotspot.innerCoords = innerCords;
			retHotspot.innerRadius = innerRad;
		}
	}
	return retHotspot;
}


int main() {
	srand(time(NULL));
	//Static vars
	const int textTurnsX = 20;
	const int textTurnsY = 20;
	const int valTurnsX = 20;
	const int valTurnsY = 60;
	const int textBarsX = 20;
	const int textBarsY = 120;
	const int valBarsX = 20;
	const int valBarsY = 160;
	Vector2 globalBestCoords = {};
	

	//Generate Hotspots
	int numHSpots = getRand(2, 5);
	for (int i = 0; i < numHSpots; i++) {
		sHotSpot temp = createHotSpots();
		vHotSpots.push_back(temp);
	}

	//Set up particles
	for (int i = 0; i < noParticles; i++) {
		sParticle temp;
		Vector2 current;
		current.x = getRand(fieldStartW, screenWidth);
		current.y = getRand(0, screenHeight);
		Vector2 pbest;
		pbest.x = current.x;
		pbest.y = current.y;
		temp.particleCoords = current;
		temp.pBest = pbest;
		temp.pVelocity = minVelocity;
		vParticles.push_back(temp);
	}

	//Init window
	InitWindow(screenWidth, screenHeight, "Particle Swarm Optimisation");
	SetTargetFPS(60);

	//Main Drawing Loop
	while (!WindowShouldClose()) {
		//Update

		//Draw
		BeginDrawing();

		ClearBackground(RAYWHITE); 
		DrawLine(fieldStartW, 0, fieldStartW, screenHeight, BLACK);

		//Display Turns Text
		DrawText("Turns", textTurnsX, textTurnsY, 30, GRAY);
		DrawText("x", valTurnsX, valTurnsY, 25, BLUE);

		//Display Barriers Text
		DrawText("Barriers", textBarsX, textBarsY, 30, GRAY);
		DrawText("x", valBarsX, valBarsY, 25, BLUE);

		//Display Hotspots
		for (int i = 0; i < vHotSpots.size(); i++) {
			//Draw outer circle
			DrawCircle(vHotSpots[i].outerCoords.x, vHotSpots[i].outerCoords.y, vHotSpots[i].outerRadius, OUTERHS);
		}

		for (int i = 0; i < vHotSpots.size(); i++) {
			//Draw middle circle
			if (vHotSpots[i].hasMiddle) {
				DrawCircle(vHotSpots[i].middleCoords.x, vHotSpots[i].middleCoords.y, vHotSpots[i].middleRadius, MIDDLEHS);
			}
			//Draw inner circle
			if (vHotSpots[i].hasInner) {
				DrawCircle(vHotSpots[i].innerCoords.x, vHotSpots[i].innerCoords.y, vHotSpots[i].innerRadius, INNERHS);
			}
		} 
		for (int i = 0; i < vHotSpots.size(); i++) {
			//Draw inner circle
			if (vHotSpots[i].hasInner) {
				DrawCircle(vHotSpots[i].innerCoords.x, vHotSpots[i].innerCoords.y, vHotSpots[i].innerRadius, INNERHS);
			}
		}

		//Draw Particles
		for (int i = 0; i < noParticles; i++) {
			DrawCircle(vParticles[i].particleCoords.x, vParticles[i].particleCoords.y, particleRad, BLACK);
		}

		EndDrawing();
		Concurrency::wait(100);
		calculateFitness();
		updateVelocity();
		updateCoordinates();
	}
	//DeInit
	CloseWindow();
	return 0;
}




//int main(void)
//{
//	// Initialization
//	//--------------------------------------------------------------------------------------
//	const int screenWidth = 800;
//	const int screenHeight = 450;
//
//	InitWindow(screenWidth, screenHeight, "raylib [shapes] example - basic shapes drawing");
//
//	SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
//	//--------------------------------------------------------------------------------------
//
//	// Main game loop
//	while (!WindowShouldClose())    // Detect window close button or ESC key
//	{
//		// Update
//		//----------------------------------------------------------------------------------
//		// TODO: Update your variables here
//		//----------------------------------------------------------------------------------
//
//		// Draw
//		//----------------------------------------------------------------------------------
//		BeginDrawing();
//
//		ClearBackground(RAYWHITE);
//
//		DrawText("some basic shapes available on raylib", 20, 20, 20, DARKGRAY);
//
//		DrawCircle(screenWidth / 4, 120, 35, DARKBLUE);
//
//		DrawRectangle(screenWidth / 4 * 2 - 60, 100, 120, 60, RED);
//		DrawRectangleLines(screenWidth / 4 * 2 - 40, 320, 80, 60, ORANGE);  // NOTE: Uses QUADS internally, not lines
//		DrawRectangleGradientH(screenWidth / 4 * 2 - 90, 170, 180, 130, MAROON, GOLD);
//
//		DrawTriangle((Vector2) { screenWidth / 4 * 3, 80 },
//			(Vector2) {
//			screenWidth / 4 * 3 - 60, 150
//		},
//			(Vector2) {
//			screenWidth / 4 * 3 + 60, 150
//		}, VIOLET);
//
//		DrawPoly((Vector2) { screenWidth / 4 * 3, 320 }, 6, 80, 0, BROWN);
//
//		DrawCircleGradient(screenWidth / 4, 220, 60, GREEN, SKYBLUE);
//
//		// NOTE: We draw all LINES based shapes together to optimize internal drawing,
//		// this way, all LINES are rendered in a single draw pass
//		DrawLine(18, 42, screenWidth - 18, 42, BLACK);
//		DrawCircleLines(screenWidth / 4, 340, 80, DARKBLUE);
//		DrawTriangleLines((Vector2) { screenWidth / 4 * 3, 160 },
//			(Vector2) {
//			screenWidth / 4 * 3 - 20, 230
//		},
//			(Vector2) {
//			screenWidth / 4 * 3 + 20, 230
//		}, DARKBLUE);
//		EndDrawing();
//		//----------------------------------------------------------------------------------
//	}
//
//	// De-Initialization
//	//--------------------------------------------------------------------------------------
//	CloseWindow();        // Close window and OpenGL context
//	//--------------------------------------------------------------------------------------
//
//	return 0;
//}