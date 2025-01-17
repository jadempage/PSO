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
5.Make barriers impassible
6.Turn limit
7.Add reset button (probably a PITA)
8.Add config buttons e.g. change particle qty, social and cognitive weights, inertia
Notes:
- Particles get a bit overlappy at 50. Once you get a few hundred even tiny red dots are found in the first "generation"
  Kinda hard to see what's going on when you have 1000. I've tested up to 100,000 for fun but it gets a bit chuggy obviously..
- You need to initialise a window before you can load a texture (OpenGL needs a current context!)
*/

//Global vars b/c bad programming practice �\_(^_^)_/� 
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

#define C_HS1       CLITERAL(Color){ 255, 230, 230, 255 }
#define C_HS2       CLITERAL(Color){ 255, 179, 179, 255 }
#define C_HS3       CLITERAL(Color){ 255, 128, 128, 255 }
#define C_HS4		CLITERAL(Color){ 179, 0, 0, 255 }
#define C_HS5		CLITERAL(Color){ 102, 0, 0, 255 }



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
		temp.pBestValue = 0;
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
			int hTier = curH.tier;
			int pTier = curP.pBestValue;
			Vector2 hsCoords = curH.coords;
			int rad = curH.radius;
			if (CheckCollisionCircles(cParticle, particleRad, hsCoords, rad)) {
				if (hTier > pTier) {
					vParticles[p].pBest = cParticle;
					vParticles[p].pBestValue = hTier;
					globalBestFound = true;
					if (globalBestValue < hTier) {
						globalBestValue = hTier;
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
				Vector2 coords = curH.coords;
				int rad = curH.radius + radMod;
				if (CheckCollisionCircles(cParticle, particleRad, coords, rad)) {
					globalBestCoords = cParticle;
					globalBestFound = true;
					break;
				}
			}
		}
		radMod++;
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
	/*v = v + c1 * rand * (pBest � p) + c2 * rand * (gBest � p)*/
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

float dist(double x1, double y1, double x2, double y2)
{
	//https://stackoverflow.com/questions/40938720/finding-the-distance-between-two-points-in-c
	double square_difference_x = (x2 - x1) * (x2 - x1);
	double square_difference_y = (y2 - y1) * (y2 - y1);
	double sum = square_difference_x + square_difference_y;
	double value = sqrt(sum);
	return value;
}

bool lineCircleCollision(Vector2 vCursor, float buffer, Vector2 vLineStart, Vector2 vLineEnd) {
	//http://jeffreythompson.org/collision-detection/line-point.php
	float lineLen = dist(vLineStart.x, vLineStart.y, vLineEnd.x, vLineEnd.y);
	float d1 = dist(vCursor.x, vCursor.y, vLineStart.x, vLineStart.y);
	float d2 = dist(vCursor.x, vCursor.y, vLineEnd.x, vLineEnd.y);

	if (d1 + d2 >= lineLen - buffer && d1 + d2 <= lineLen + buffer) {
		return true;
	}
	return false;

}

Vector2 lineLineCollision(Vector2 pOrig, Vector2 pRes, Vector2 vLineStart, Vector2 vLineEnd) {
	//http://jeffreythompson.org/collision-detection/line-line.php
	double x1 = pOrig.x;
	double y1 = pOrig.y;
	double x2 = pRes.x;
	double y2 = pRes.y;
	double x3 = vLineStart.x;
	double y3 = vLineStart.y;
	double x4 = vLineEnd.x;
	double y4 = vLineEnd.y;
	Vector2 ret;

	// calculate the distance to intersection point
	double uA = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3)) / ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));
	double uB = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3)) / ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));
	// if uA and uB are between 0-1, lines are colliding
	if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {
		// optionally, draw a circle where the lines meet
		double intersectionX = x1 + (uA * (x2 - x1));
		double intersectionY = y1 + (uA * (y2 - y1));
		ret.x = intersectionX;
		ret.y = intersectionY;
	}
	else {
		ret.x = -1;
		ret.y = -1; 
	}
	return ret;

}

void updateCoordinates() {
	for (int p = 0; p < noParticles; p++) {
		//Calculate line line intersection using particles movements as line A and barrier as line B
		Vector2 curCoords = vParticles[p].particleCoords;
		Vector2 newCoords;
		Vector2 backCoords;
		newCoords.x = vParticles[p].particleCoords.x + vParticles[p].pVelocity.x;
		newCoords.y = vParticles[p].particleCoords.y + vParticles[p].pVelocity.y;
		if (vBarriers.size() > 0) {
			for (int i = 0; i < vBarriers.size(); i++) {
				Vector2 collisionRes = lineLineCollision(curCoords, newCoords, vBarriers[i].vStart, vBarriers[i].vFin);
				if (collisionRes.x == -1) {
					vParticles[p].particleCoords = newCoords;
				}
				else {
					if (curCoords.x > newCoords.x) {
						backCoords.x = collisionRes.x + 10;
					}
					else {
						backCoords.x = collisionRes.x - 10;
					}
					if (curCoords.y > newCoords.y) {
						backCoords.y = collisionRes.y + 10;
					}
					else {
						backCoords.y = collisionRes.y - 10;
					}
					vParticles[p].particleCoords = backCoords;
					break;
				}
			}
		}
		else {
			vParticles[p].particleCoords = newCoords;
		}



		//Stay on the screen plz
		if (vParticles[p].particleCoords.x > screenWidth) vParticles[p].particleCoords.x = screenWidth - particleRad;
		if (vParticles[p].particleCoords.x < fieldStartW) vParticles[p].particleCoords.x = fieldStartW + particleRad * 3;
		if (vParticles[p].particleCoords.y < 0) vParticles[p].particleCoords.y = particleRad;
		if (vParticles[p].particleCoords.y > screenHeight) vParticles[p].particleCoords.y = screenHeight - particleRad;
	}
}

sHotSpot createHotSpot() {
	//HS 1 most common, 5 least common
	sHotSpot retHotspot;
	Vector2 tempCoords;
	int tier = getRand(1, 50);
	//Make tiers proportionately rare
	if (tier <= 20) {
		retHotspot.tier = 1;
	}
	else if (tier <= 30) {
		retHotspot.tier = 2;
	}
	else if (tier <= 38) {
		retHotspot.tier = 3;
	}
	else if (tier <= 45) {
		retHotspot.tier = 3;
	}
	else {
		retHotspot.tier = 4;
	}

	int rad = getRand(retHotspot.tier, retHotspot.tier * 20);
	int xPos = getRand(fieldStartW + rad, screenWidth - rad);
    int yPos = getRand(rad, screenHeight + rad);
	retHotspot.coords.x = xPos;
	retHotspot.coords.y = yPos;
	retHotspot.radius = rad; 
	return retHotspot;
}

int main() {
	int noBarriers = 0;
	bool isPaused = false; 
	bool isStep = false;
	bool addingBarriers = false;
	bool removingBarriers = false; 
	bool doneFirstPoint = false;
	bool doneSecondPoint = false; 
	srand(time(NULL));
	//Static vars
	const int textTurnsX = 20;
	const int textTurnsY = 20;
	const int valTurnsX = 50;
	const int valTurnsY = 60;
	const int textBarsX = 10;
	const int textBarsY = 150;
	const int valBarsX = 70;
	const int valBarsY = 210;
	const int infoTextX = 10;
	const int infoTextY = 350;
	Vector2 globalBestCoords = {};
	

	//Generate Hotspots
	int numHSpots = getRand(3, 30);
	for (int i = 0; i < numHSpots; i++) {
		sHotSpot temp = createHotSpot();
		vHotSpots.push_back(temp);
	}

	//Set up particles
	generateParticles(noParticles);

	//Init window
	InitWindow(screenWidth, screenHeight, "Particle Swarm Optimisation");
	SetTargetFPS(60);

	//Btn Textures
	Texture2D btnPlay = LoadTexture("Icons/btnPlay.png");
	Texture2D btnStep = LoadTexture("Icons/btnStep.png");
	Texture2D btnPause = LoadTexture("Icons/btnPause.png");
	Texture2D btnBarAdd = LoadTexture("Icons/btnDraw.png");
	Texture2D btnBarRem = LoadTexture("Icons/btnErase.png");

	//Init buttons
	//Pause button
	Rectangle btnPauseRec = { 0,  0, btnPause.width, btnPause.height };
	Vector2 cBtnPause;
	cBtnPause.x = 10;
	cBtnPause.y = 90;
	Rectangle btnPauseRecBounds = { cBtnPause.x, cBtnPause.y, btnPause.width, btnPause.height };
	//Play button
	Rectangle btnPlayRec = { 0, 0, btnPlay.width, btnPlay.height };
	Vector2 cBtnPlay; 
	cBtnPlay.x = 50;
	cBtnPlay.y = 90;
	Rectangle btnPlayRecBounds = { cBtnPlay.x, cBtnPlay.y, btnPlay.width, btnPlay.height };
	//Step button
	Rectangle btnStepRec = { 0, 0, btnStep.width, btnStep.height };
	Vector2 cBtnStep;
	cBtnStep.x = 90;
	cBtnStep.y = 90;
	Rectangle btnStepRecBounds = { cBtnStep.x, cBtnStep.y, btnStep.width, btnStep.height };
	//-Bar button
	Rectangle btnBarRemRec = { 0, 0, btnBarRem.width, btnBarRem.height };
	Vector2 cBtnRem;
	cBtnRem.x = 10;
	cBtnRem.y = 190;
	Rectangle btnRemRecBounds = { cBtnRem.x, cBtnRem.y, btnBarRem.width, btnBarRem.height };
	//+Bar button
	Rectangle btnBarAddRec = { 0, 0, btnBarAdd.width, btnBarAdd.height };
	Vector2 cBtnAdd;
	cBtnAdd.x = 10;
	cBtnAdd.y = 230;
	Rectangle btnAddRecBounds = { cBtnAdd.x, cBtnAdd.y, btnBarAdd.width, btnBarAdd.height };


	//Init mouse
	Vector2 mousePoint = { 0.0f, 0.0f };

	//Main Drawing Loop
	while (!WindowShouldClose()) {
		//Update mouse
		mousePoint = GetMousePosition();

		if (CheckCollisionPointRec(mousePoint, btnPlayRecBounds)) {
			if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) isPaused = false;
		}
		if (CheckCollisionPointRec(mousePoint, btnPauseRecBounds)) {
			if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) isPaused = true;
		}
		if (CheckCollisionPointRec(mousePoint, btnStepRecBounds)) {
			if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) isStep = true;
		}
		if (CheckCollisionPointRec(mousePoint, btnAddRecBounds)) {
			if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
				isPaused = true;
				addingBarriers = true;
			}
		}
		if (CheckCollisionPointRec(mousePoint, btnRemRecBounds)) {
			if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
				isPaused = true;
				removingBarriers = true;
			}
		}

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
			char cBars[20];
			sprintf(cBars, "%d", noBarriers);
			DrawText("Barriers", textBarsX, textBarsY, 30, GRAY);
			DrawText(cBars, valBarsX, valBarsY, 25, BLUE);

			//Buttons
			DrawTextureRec(btnPlay, btnPlayRec, cBtnPlay, WHITE); // Draw button frame
			DrawTextureRec(btnPause, btnPauseRec, cBtnPause, WHITE); // Draw button frame
			DrawTextureRec(btnStep, btnStepRec, cBtnStep, WHITE); // Draw button frame
			DrawTextureRec(btnBarAdd, btnBarAddRec, cBtnAdd, WHITE); // Draw button frame
			DrawTextureRec(btnBarRem, btnBarRemRec, cBtnRem, WHITE); // Draw button frame

			//Display Hotspots - draw all lesser colours first so higher tier colours can overlap 
			for (int i = 0; i < vHotSpots.size(); i++) {
				if (vHotSpots[i].tier == 1) {
					DrawCircle(vHotSpots[i].coords.x, vHotSpots[i].coords.y, vHotSpots[i].radius, C_HS1);
				}
			}
			for (int i = 0; i < vHotSpots.size(); i++) {
				if (vHotSpots[i].tier == 2) {
					DrawCircle(vHotSpots[i].coords.x, vHotSpots[i].coords.y, vHotSpots[i].radius, C_HS2);
				}
			}
			for (int i = 0; i < vHotSpots.size(); i++) {
				if (vHotSpots[i].tier == 3) {
					DrawCircle(vHotSpots[i].coords.x, vHotSpots[i].coords.y, vHotSpots[i].radius, C_HS3);
				}
			}
			for (int i = 0; i < vHotSpots.size(); i++) {
				if (vHotSpots[i].tier == 4) {
					DrawCircle(vHotSpots[i].coords.x, vHotSpots[i].coords.y, vHotSpots[i].radius, C_HS4);
				}
			}
			for (int i = 0; i < vHotSpots.size(); i++) {
				if (vHotSpots[i].tier == 5) {
					DrawCircle(vHotSpots[i].coords.x, vHotSpots[i].coords.y, vHotSpots[i].radius, C_HS5);
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

			//Draw Barriers
			for (int i = 0; i < vBarriers.size(); i++) {
				sBarrier cur = vBarriers[i];
				DrawLineEx(cur.vStart, cur.vFin, 3, BLACK);
				
			}

			//Add Barriers
			if (addingBarriers) {
				sBarrier temp;
				if (!doneFirstPoint) {
					mousePoint = GetMousePosition();
					DrawText("Right click to", infoTextX, infoTextY, 20, GRAY);
					DrawText("set start pos", infoTextX, infoTextY + 30, 20, GRAY);
					if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
						temp.vStart.x = mousePoint.x;
						temp.vStart.y = mousePoint.y;
						DrawCircle(temp.vStart.x, temp.vStart.y, 3, PURPLE);
						doneFirstPoint = true;
					}
				}
				if (!doneSecondPoint && doneFirstPoint) {
					Concurrency::wait(500);
					DrawRectangle(infoTextX, infoTextY, 100, 100, RAYWHITE);
					mousePoint = GetMousePosition();
					DrawText("Left click to", infoTextX, infoTextY, 20, GRAY);
					DrawText("set end pos", infoTextX, infoTextY + 30, 20, GRAY);
					if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
						mousePoint = GetMousePosition();
						temp.vFin.x = mousePoint.x;
						temp.vFin.y = mousePoint.y;
						DrawCircle(temp.vFin.x, temp.vFin.y, 3, PURPLE);
						doneSecondPoint = true;
					}
				}
				if (doneFirstPoint && doneSecondPoint) {
					vBarriers.push_back(temp);
					noBarriers++;
					doneFirstPoint = false;
					doneSecondPoint = false;
					addingBarriers = false;
					isPaused = false; 
				}
			}

			//Rem barriers
			if (removingBarriers) {
				for (int i = 0; i < vBarriers.size(); i++) {
					sBarrier temp = vBarriers[i];
					if (lineCircleCollision(mousePoint, 0.1, vBarriers[i].vStart, vBarriers[i].vFin)) {
						sBarrier cur = vBarriers[i];
						DrawLineEx(cur.vStart, cur.vFin, 3, GREEN);
						if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
							vBarriers.erase(vBarriers.begin() + i);
							isPaused = false;
							removingBarriers = false;
						}
					}

					//if (CheckCollisionPointRec(mousePoint, btnPauseRecBounds)) {
					//	if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) isPaused = true;
					//}
				}
			}


			EndDrawing();
			Concurrency::wait(100);
			
			if (!isPaused || (isPaused && isStep)) {
				calculateFitness();
				updateVelocity();
				updateCoordinates();
				if (turns % 20 == 0) {
					killParticles();
					generateParticles(pNumberToKill);
				}
				purgeParticles();
				turns++;
				isStep = false;
			}

		}
	//DeInit
	CloseWindow();
	return 0;
}
 