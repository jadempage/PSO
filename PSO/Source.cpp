#include "raylib.h"
#include <stdlib.h>   
#include <vector>
#include "Header.h"
#include <stdio.h>
#include <time.h>

//Global vars b/c bad programming practice ¯\_(?)_/¯ 
const int screenWidth = 800;
const int screenHeight = 600;
const int fieldStartW = 160;

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

	//Other vars
	std::vector<hotSpot> hotSpots; 

	//Generate Hotspots
	srand(time(NULL)); //Pseudo Random Seed
	int numHSpots = rand() % 10 + 3; //Max 3
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
		//Display Turns
		DrawText("Turns", textTurnsX, textTurnsY, 30, GRAY);
		DrawText("x", valTurnsX, valTurnsY, 25, BLUE);
		////Display Barriers
		DrawText("Barriers", textBarsX, textBarsY, 30, GRAY);
		DrawText("x", valBarsX, valBarsY, 25, BLUE);

		EndDrawing();
	}
	//DeInit
	CloseWindow();
	return 0;
}

hotSpot createHotSpots() {
	srand(time(NULL)); //Pseudo Random Seed
	//Outer Spot
	int numHSpots = rand() % 10 + 3; //Max 3


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