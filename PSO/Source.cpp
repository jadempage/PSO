#include "raylib.h"
#include <stdlib.h>   
#include <vector>
#include "Header.h"
#include <stdio.h>
#include <time.h>
#include <concrt.h>

//Global vars b/c bad programming practice ¯\_(^_^)_/¯ 
const int screenWidth = 800;
const int screenHeight = 600;
const int fieldStartW = 160;
#define OUTERHS       CLITERAL(Color){ 255, 204, 204, 255 }
#define MIDDLEHS       CLITERAL(Color){ 255, 128, 128, 255 }
#define INNERHS       CLITERAL(Color){ 255, 0, 0, 255 }
//Other vars



int getRand(int max, int min) {
	srand((int)time(0));
	int randNum = rand() % (max - min + 1) + min;
	Concurrency::wait(10);
	return randNum;
}

hotSpot createHotSpots() {

	hotSpot retHotspot;
	coords innerCords;
	coords middleCords;
	coords outerCords;
	//Outer Spot
	int numHSpots = getRand(4, 1);
	for (int i = 0; i < numHSpots; i++) {
		if (i == 0) {
			int outerRad = getRand(3, 50);
			int xPos = getRand(screenWidth - outerRad, fieldStartW + outerRad);
			int yPos = getRand(screenHeight + outerRad, outerRad);
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
			int middleRad = getRand(2, retHotspot.outerRadius/2);
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
			int innerRad = getRand(1, retHotspot.middleRadius/2 );
			retHotspot.innerCoords = innerCords;
			retHotspot.innerRadius = innerRad;
		}
	}
	return retHotspot;
}


int main() {

	//Static vars
	const int textTurnsX = 20;
	const int textTurnsY = 20;
	const int valTurnsX = 20;
	const int valTurnsY = 60;
	const int textBarsX = 20;
	const int textBarsY = 120;
	const int valBarsX = 20;
	const int valBarsY = 160;

	

	//Generate Hotspots
	int numHSpots = getRand(5, 2);
	for (int i = 0; i < numHSpots; i++) {
		hotSpot temp = createHotSpots();
		hotSpots.push_back(temp);
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
		for (int i = 0; i < hotSpots.size(); i++) {
			//Draw outer circle
			DrawCircle(hotSpots[i].outerCoords.x, hotSpots[i].outerCoords.y, hotSpots[i].outerRadius, OUTERHS);
		}

		for (int i = 0; i < hotSpots.size(); i++) {
			//Draw middle circle
			if (hotSpots[i].hasMiddle) {
				DrawCircle(hotSpots[i].middleCoords.x, hotSpots[i].middleCoords.y, hotSpots[i].middleRadius, MIDDLEHS);
			}
			//Draw inner circle
			if (hotSpots[i].hasInner) {
				DrawCircle(hotSpots[i].innerCoords.x, hotSpots[i].innerCoords.y, hotSpots[i].innerRadius, INNERHS);
			}
		} 
		for (int i = 0; i < hotSpots.size(); i++) {
			//Draw inner circle
			if (hotSpots[i].hasInner) {
				DrawCircle(hotSpots[i].innerCoords.x, hotSpots[i].innerCoords.y, hotSpots[i].innerRadius, INNERHS);
			}
		}

		EndDrawing();
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