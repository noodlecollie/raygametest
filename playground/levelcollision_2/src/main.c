#include <stdio.h>
#include "gamelib/external/raylibheaders.h"
#include "raygui.h"

#include "gamelib/trace.h"
#include "gamelib/gameutil.h"
#include "gamelib/world.h"
#include "gamelib/entity/entity.h"
#include "gamelib/entity/terraincomponent.h"
#include "gamelib/entity/physicscomponent.h"
#include "gamelib/entity/logiccomponent.h"
#include "gamelib/physics.h"

typedef struct GuiValues
{
	float levelScale;
} GuiValues;

typedef struct PlayerData
{
	bool onGround;
} PlayerData;

static inline bool PanKeyPressed()
{
	return IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_SPACE);
}

static inline bool SurfaceNormalIsGround(Vector2 normal)
{
	return Vector2DotProduct(normal, (Vector2){ 0.0f, -1.0f }) > 0.5f;
}

static inline bool CheckIfStandingOnGround(PhysicsComponent* physComp, TerrainComponent* terrain)
{
	if ( !physComp || !terrain )
	{
		return false;
	}

	Vector2 delta = (Vector2){ 0.0f, 2.0f * PHYSICS_CONTACT_ADJUST_DIST };
	Rectangle hull = PhysicsComponent_GetWorldCollisionHull(physComp);
	TraceResult result = TraceRectangleMovementAgainstTerrain(hull, delta, terrain, physComp->collisionMask);

	return result.collided && SurfaceNormalIsGround(result.contactNormal);
}

static void PlayerPreThink(LogicComponent* component)
{
	PlayerData* data = (PlayerData*)component->userData;

	// Before physics sim, assume the player will not be on the ground.
	data->onGround = false;
}

static void PlayerPhysicsCollided(LogicComponent* component, const OnPhysicsCollidedArgs* args)
{
	PlayerData* data = (PlayerData*)component->userData;
	Entity* thisEntity = LogicComponent_GetOwnerEntity(component);

	if ( thisEntity == args->initiator )
	{
		data->onGround = CheckIfStandingOnGround(Entity_GetPhysicsComponent(thisEntity), Entity_GetTerrainComponent(args->recipient));
	}
}

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	int screenWidth = 800;
	int screenHeight = 450;

	SetTraceLogLevel(LOG_DEBUG);
	InitWindow(screenWidth, screenHeight, "Level Collision");

	const Vector2 dpiScale = GetWindowScaleDPI();
	screenWidth = (int)((float)screenWidth * dpiScale.x);
	screenHeight = (int)((float)screenHeight * dpiScale.y);
	SetWindowSize(screenWidth, screenHeight);
	GuiSetStyle(DEFAULT, TEXT_SIZE, (int)((float)GuiGetStyle(DEFAULT, TEXT_SIZE) * dpiScale.x));

	GuiValues guiValues = { 0 };
	guiValues.levelScale = 30;

	const Rectangle guiBounds = { 0.0f, 0.0f, 240.0f * dpiScale.x, 140.0f * dpiScale.y };

	World* world = World_Create();
	world->gravity = 0.0f;

	Entity* terrainEnt = World_CreateEntity(world);
	TerrainComponent* terrain = Entity_CreateTerrainComponent(terrainEnt);
	terrain->scale = (float)guiValues.levelScale;

	TerrainComponent_LoadLayer(terrain, 0, "res/maps/test.png");
	Vector2i levelDim = TerrainComponent_GetLayerDimensions(terrain, 0);

	Camera2D camera = { 0 };
	camera.target = (Vector2){ ((float)levelDim.x / 2.0f) * terrain->scale, ((float)levelDim.y / 2.0f) * terrain->scale };
	camera.offset = (Vector2){ (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;

	Vector2 beginPos = Vector2Zero();
	Vector2 endPos = Vector2Zero();

	Entity* playerEnt = World_CreateEntity(world);
	PhysicsComponent* playerPhys = Entity_CreatePhysicsComponent(playerEnt);
	playerPhys->collisionHull = (Rectangle){ -5.0f, -10.0f, 10.0f, 20.0f };
	playerPhys->collisionMask = 0xFFFFFFFF;
	playerPhys->enabled = true;
	playerPhys->movementType = PHYSMOVE_SLIDE;

	LogicComponent* playerLogic = Entity_AddLogicComponent(playerEnt);
	playerLogic->userData = MemAlloc(sizeof(PlayerData));
	playerLogic->callbacks.onPreThink = &PlayerPreThink;
	playerLogic->callbacks.onPhysicsCollided = &PlayerPhysicsCollided;

	SetTargetFPS(60);

	while ( !WindowShouldClose() )
	{
		terrain->scale = (float)guiValues.levelScale;

		bool mouseIsInGuiArea = CheckCollisionPointRec(GetMousePosition(), guiBounds);

		if ( !mouseIsInGuiArea )
		{
			float wheelDelta = GetMouseWheelMove() * 0.25f;

			if ( wheelDelta >= 0.0f )
			{
				camera.zoom *= 1.0f + wheelDelta;
			}
			else
			{
				camera.zoom *= 1.0f / (1 - wheelDelta);
			}

			if ( camera.zoom < 0.125f )
			{
				camera.zoom = 0.125f;
			}
			else if ( camera.zoom > 10.0f )
			{
				camera.zoom = 10.0f;
			}

			Vector2 panDrag = Vector2Zero();

			if ( IsMouseButtonDown(MOUSE_BUTTON_MIDDLE) || (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && PanKeyPressed()) )
			{
				panDrag = GetMouseDelta();
			}

			camera.target = Vector2Subtract(camera.target, Vector2Scale(panDrag, 1.0f / camera.zoom));
		}

		if ( IsKeyPressed(KEY_R) )
		{
			camera.target = Vector2Zero();
			camera.zoom = 1.0f;
		}

		if ( !mouseIsInGuiArea )
		{
			if ( IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !PanKeyPressed() )
			{
				beginPos = GetScreenToWorld2D(GetMousePosition(), camera);
			}

			if ( IsMouseButtonDown(MOUSE_BUTTON_RIGHT) )
			{
				endPos = GetScreenToWorld2D(GetMousePosition(), camera);
			}
		}

		Vector2 traceDelta = Vector2Subtract(endPos, beginPos);

		playerEnt->position = beginPos;
		playerPhys->velocity = traceDelta;

		Rectangle beginHull = PhysicsComponent_GetWorldCollisionHull(playerPhys);
		float deltaTime = GetFrameTime();

		if ( deltaTime > 0.0f )
		{
			// Scale velocity up so that it'll be scaled down again by the simulation.
			playerPhys->velocity = Vector2Scale(playerPhys->velocity, 1.0f / deltaTime);
			World_Update(world);
			playerPhys->velocity = Vector2Scale(playerPhys->velocity, deltaTime);
		}

		BeginDrawing();

		ClearBackground(LIGHTGRAY);

		BeginMode2D(camera);

		Vector2i dims = TerrainComponent_GetLayerDimensions(terrain, 0);

		for ( int y = 0; y < dims.y; ++y )
		{
			for ( int x = 0; x < dims.x; ++x )
			{
				Rectangle blockRect = TerrainComponent_GetBlockWorldRectByCoOrds(terrain, (Vector2i){ x, y });
				Color blockColour = TerrainComponent_GetBlockColourByCoOrds(terrain, 0, (Vector2i){ x, y });
				DrawRectangleRec(blockRect, blockColour);
			}

			if ( Vector2Length(traceDelta) > 0.0f )
			{
				DrawRectangleLinesEx(beginHull, 1.0f / camera.zoom, GREEN);
				DrawCircle((int)beginPos.x, (int)beginPos.y, 3.0f, GREEN);

				Rectangle endHull = PhysicsComponent_GetWorldCollisionHull(playerPhys);
				Vector2 endHullMid = RectangleMid(endHull);
				DrawRectangleLinesEx(endHull, 1.0f / camera.zoom, ((PlayerData*)playerLogic->userData)->onGround ? YELLOW : BLUE);
				DrawCircle((int)(endHullMid.x), (int)(endHullMid.y), 3.0f, ((PlayerData*)playerLogic->userData)->onGround ? YELLOW : BLUE);

				DrawCircle((int)endPos.x, (int)endPos.y, 3.0f, GREEN);

				DrawLineEx(beginPos, endPos, 1.0f / camera.zoom, BLACK);
				DrawLineEx(endHullMid, Vector2Add(endHullMid, playerPhys->velocity), 1.0f / camera.zoom, YELLOW);
			}
		}

		EndMode2D();

		DrawRectangleLinesEx(guiBounds, dpiScale.x, BLACK);

		int fontSize = (int)(10.0f * dpiScale.y);
		int leftMargin = (int)(10.0f * dpiScale.x);

		int controlHeight = (int)(15.0f * dpiScale.y);
		int controlLeftMargin = (int)(70.0f * dpiScale.x);

		Rectangle guiRect = { (float)controlLeftMargin, 0.0f, 130.0f * dpiScale.x, (float)controlHeight };

		char buffer[64];

		snprintf(buffer, sizeof(buffer), "Left mouse: start point (%d, %d)", (int)beginPos.x, (int)beginPos.y);
		buffer[sizeof(buffer) - 1] = '\0';
		DrawText(buffer, leftMargin, (int)(10.0f * dpiScale.y), fontSize, BLACK);

		snprintf(buffer, sizeof(buffer), "Right mouse: end point (%d, %d)", (int)endPos.x, (int)endPos.y);
		buffer[sizeof(buffer) - 1] = '\0';
		DrawText(buffer, leftMargin, (int)(30.0f * dpiScale.y), fontSize, BLACK);

		DrawText("Middle mouse: pan level", leftMargin, (int)(50.0f * dpiScale.y), fontSize, BLACK);

		guiRect.y = 70.0f * dpiScale.y;
		snprintf(buffer, sizeof(buffer), "%.2f", guiValues.levelScale);
		buffer[sizeof(buffer) - 1] = '\0';
		guiValues.levelScale = GuiSlider(guiRect, "Level scale: ", buffer, guiValues.levelScale, 1, 100);

		EndDrawing();
	}

	World_Destroy(world);
	CloseWindow();

	return 0;
}
