#include "raylib.h"
#include <stdlib.h>   
#include <vector>
#include "Header.h"
#include <stdio.h>
#include <time.h>
#include <concrt.h>
#include <random>
#include <chrono>

/*
To Do:
1.Add Buttons for "play" and "step"
2.Add 4th tier spot
3.Randomize spots completely rather than in tiers
4.Allow user to draw barriers
5.Make barriers impassible
*/

//Global vars b/c bad programming practice ¯\_(^_^)_/¯ 
const int screenWidth = 800;
const int screenHeight = 600;
const int fieldStartW = 160;
const int noParticles = 10; 
const int maxTurns = 50;
const int particleRad = 5;
const int minVelocity = -50;
const int maxVelocity = 50;
const int pLifespan = 20; //After this many rounds particles will start to die #
const int pNumberToKill = noParticles / 2;
int turns = 0;

const double iCog = 1.496180 * globalBestValue; //Def cognitive weighting
const double iSoc = 1.496180 / (globalBestValue + 1); //Def social weightin
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

void generateParticles(int qty) {
	for (int i = 0; i < qty; i++) {
		sParticle temp;
		Vector2 current;
		current.x = getRand(fieldStartW, screenWidth);
		current.y = getRand(0, screenHeight);
		Vector2 pbest;
		pbest.x = current.x;
		pbest.y = current.y;
		temp.particleCoords = current;
		temp.pBest = pbest;
		temp.pVelocity.x = minVelocity;
		temp.pVelocity.y = minVelocity;
		temp.isNewborn = true;
		temp.isDying = false;
		vParticles.push_back(temp);
	}
}

void killParticles() {
	if (pNumberToKill > vParticles.size()) {
		return;
		//Wtf
	}
	for (int p = 0; p < pNumberToKill; p++) {
		vParticles[p].isDying = true;
	}
}

void purgeParticles() {
	for (int p = 0; p < vParticles.size(); p++) {
		if (vParticles[p].isDying) {
			vParticles.erase(vParticles.begin() + p);
		}
	}
}

void calculateFitness() {
	//For each particle
	//For each hotspot
	//For each spot
	//Check collision
	for (int p = 0; p < noParticles; p++) {
		if (vParticles[p].isNewborn) {
			vParticles[p].isNewborn = false;
		}
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
				globalBestFound = true;
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
					globalBestFound = true;
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
					globalBestFound = true;
					if (globalBestValue == 2) {
						globalBestValue = 3;
						globalBestCoords = cParticle;
					}
				}
			}
		}
	}
	//What about if no one is on red yet? Can't have no gBest! 
	int radMod = 1;
	while (!globalBestFound) {
		for (int p = 0; p < noParticles; p++) {
			for (int h = 0; h < vHotSpots.size(); h++) {
				sHotSpot curH = vHotSpots[h];
				sParticle curP = vParticles[p];
				Vector2 cParticle = curP.particleCoords;
				//Outer HS
				Vector2 cOuterHS = curH.outerCoords;
				int rOuterHS = curH.outerRadius + radMod;
				if (CheckCollisionCircles(cParticle, particleRad, cOuterHS, rOuterHS)) {
					globalBestCoords = cParticle;
					globalBestFound = true;
					break;
				}
				radMod++;
			}
		}
	}
	//If the particles personal best doesn't reach anything, reset it so they don't bounce back and forth
	for (int p = 0; p < noParticles; p++) {
		if (vParticles[p].pBestValue < 1) {
			vParticles[p].pBest.x = vParticles[p].particleCoords.x;
			vParticles[p].pBest.y = vParticles[p].particleCoords.y;
		}
	}

}

void updateVelocity() {
	/*v = v + c1 * rand * (pBest – p) + c2 * rand * (gBest – p)*/
	for (int p = 0; p < noParticles; p++) {
		sParticle cur = vParticles[p];
		Vector2 oldVel = cur.pVelocity;
		Vector2 pBest = cur.pBest;
		Vector2 gBest = globalBestCoords;
		Vector2 curPos = cur.particleCoords;

		double r1 = getRand(1, 3);
		double r2 = getRand(1, 3);

		//First part of the formula
		Vector2 newVelocity;
		double vXTemp = oldVel.x;
		double vYTemp = oldVel.y;
		vYTemp *= iInertia;
		vXTemp *= iInertia;
		newVelocity.x = vXTemp;
		newVelocity.y = vYTemp;

		//Second part of the formula
		Vector2 vTemp;

		vTemp.x = pBest.x - curPos.x;
		vTemp.y = pBest.y - curPos.y;
		vTemp.x = vTemp.x * iCog;
		vTemp.y = vTemp.y * iCog;
		vTemp.x = vTemp.x * r1;
		vTemp.y = vTemp.y * r1; 
		newVelocity.x = newVelocity.x + vTemp.x;
		newVelocity.y = newVelocity.y + vTemp.y;

		//Third part of formula
		Vector2 vTemp2;

		vTemp2.x = gBest.x - curPos.x;
		vTemp2.y = gBest.y - curPos.y;
		vTemp2.x *= iSoc;
		vTemp2.y *= iSoc;
		vTemp2.x *= r2;
		vTemp2.y *= r2;
		newVelocity.x += vTemp2.x;
		newVelocity.y += vTemp2.y;

		if (newVelocity.x > maxVelocity)  newVelocity.x = maxVelocity;
		if (newVelocity.y > maxVelocity) newVelocity.y = maxVelocity;
		if (newVelocity.y < minVelocity) newVelocity.y = minVelocity;
		if (newVelocity.x < minVelocity) newVelocity.x = minVelocity;
		vParticles[p].pVelocity = newVelocity; 
	}
}


void updateCoordinates() {
	for (int p = 0; p < noParticles; p++) {

		vParticles[p].particleCoords.x += vParticles[p].pVelocity.x;
		vParticles[p].particleCoords.y += vParticles[p].pVelocity.y;

		//Stay on the screen plz
		if (vParticles[p].particleCoords.x > screenWidth) vParticles[p].particleCoords.x = screenWidth - particleRad;
		if (vParticles[p].particleCoords.x < fieldStartW) vParticles[p].particleCoords.x = fieldStartW + particleRad * 3;
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
	generateParticles(noParticles);

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
		char cTurns[20];
		sprintf(cTurns, "%d", turns);
		DrawText("Turns", textTurnsX, textTurnsY, 30, GRAY);
		DrawText(cTurns, valTurnsX, valTurnsY, 25, BLUE);

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
			if (vParticles[i].isNewborn) {
				DrawCircle(vParticles[i].particleCoords.x, vParticles[i].particleCoords.y, particleRad, SKYBLUE);
			}
			else if (vParticles[i].isDying) {
				DrawCircle(vParticles[i].particleCoords.x, vParticles[i].particleCoords.y, particleRad, GREEN);
			}
			else {
				DrawCircle(vParticles[i].particleCoords.x, vParticles[i].particleCoords.y, particleRad, BLACK);
			}
		}
		
		EndDrawing();
		Concurrency::wait(100);
		calculateFitness();
		updateVelocity();
		updateCoordinates();
		if (turns % 20 == 0) {
			killParticles();
			generateParticles(pNumberToKill);
		}
		purgeParticles();
		turns++;
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