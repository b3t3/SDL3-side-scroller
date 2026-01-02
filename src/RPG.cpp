#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <vector>
#include <string>
#include <array>
#include <format>

#include "animation.h"
#include "gameobject.h"

using namespace std;

struct SDLState {
	SDL_Window* window;
	SDL_Renderer* renderer;
	int width, height, logW, logH;
	const bool* keys;

	SDLState() : keys(SDL_GetKeyboardState(nullptr))
	{

	}
};

const size_t LAYER_IDX_LEVEL = 0;
const size_t LAYER_IDX_CHARACTERS = 1;
const int MAP_ROWS = 5;
const int MAP_COLS = 50;
const int TILE_SIZE = 32;


struct GameState
{
	std::array<std::vector<GameObject>, 2> layers;
	std::vector<GameObject> backgroundTiles;
	std::vector<GameObject> foregroundTiles;
	std::vector<GameObject> bullets;
	int playerIndex;
	SDL_FRect mapViewport;
	float bg2Scroll, bg3Scroll, bg4Scroll, bg5Scroll, bg6Scroll;

	GameState(const SDLState  &state)
	{
		playerIndex = -1;
		mapViewport = SDL_FRect{
		.x = 0, .y = 0,
		.w = static_cast<float>(state.logW),
		.h = static_cast<float>(state.logH)
		};
		bg2Scroll = bg3Scroll = bg4Scroll = bg5Scroll = bg6Scroll = 0;
	}
	GameObject& player() { return layers[LAYER_IDX_CHARACTERS][playerIndex]; }
};

struct Resources {
	const int ANIM_PLAYER_IDLE = 0;
	const int ANIM_PLAYER_RUNNING = 1;
	const int ANIM_PLAYER_SLIDE = 2;
	std::vector<Animation> playerAnims;
	const int ANIM_BULLET_MOVING = 0;
	const int ANIM_BULLET_HIT = 1;
	std::vector<Animation> bulletAnims;

	std::vector<SDL_Texture*> textures;
	SDL_Texture *texIdle, *texRun, *texSlide, *texGrass, *texDeepGrass, *texGrassR, *texGrassL, *texGrassConL, *texGrassConR,
		*texBg1, *texBg2, *texBg3, *texBg4, *texBg5, *texBg6, *texBullet, *texBulletHit;

	SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& filepath)
	{
		SDL_Texture* tex = IMG_LoadTexture(renderer, filepath.c_str());
		SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
		return tex;
	};

	void load(SDLState& state)
	{
		// animation initialization
		playerAnims.resize(5);
		playerAnims[ANIM_PLAYER_IDLE] = Animation(4, 0.8f);
		playerAnims[ANIM_PLAYER_RUNNING] = Animation(4, 0.5f);
		playerAnims[ANIM_PLAYER_SLIDE] = Animation(1, 1.0f);
		bulletAnims.resize(2);
		bulletAnims[ANIM_BULLET_MOVING] = Animation(4, 0.08f);
		bulletAnims[ANIM_BULLET_HIT] = Animation(4, 0.15f);


		// texture initialization
		texIdle = loadTexture(state.renderer, "data/idle.png");
		texRun = loadTexture(state.renderer, "data/run.png");
		texSlide = loadTexture(state.renderer, "data/slide.png");
		texGrass = loadTexture(state.renderer, "data/Tiles/grass1.png");
		texDeepGrass = loadTexture(state.renderer, "data/Tiles/deepGrass.png");
		texGrassR = loadTexture(state.renderer, "data/Tiles/grassR.png");
		texGrassL = loadTexture(state.renderer, "data/Tiles/grassL.png");
		texGrassConR = loadTexture(state.renderer, "data/Tiles/grassConR.png");
		texGrassConL = loadTexture(state.renderer, "data/Tiles/grassConL.png");
		texBg1 = loadTexture(state.renderer, "data/Background/j1.png");
		texBg2 = loadTexture(state.renderer, "data/Background/j2.png");
		texBg3 = loadTexture(state.renderer, "data/Background/j3.png");
		texBg4 = loadTexture(state.renderer, "data/Background/j4.png");
		texBg5 = loadTexture(state.renderer, "data/Background/j5.png");
		texBg6 = loadTexture(state.renderer, "data/Background/j6.png");
		texBullet = loadTexture(state.renderer, "data/bullet.png");
		texBulletHit = loadTexture(state.renderer, "data/j6.png");

	}

	// clear textures handler
	void unload()
	{
		for (SDL_Texture* tex : textures)
		{
			SDL_DestroyTexture(tex);
		}
	}
};


// forward declare funcs
bool initialize(SDLState& state);
void cleanup(SDLState& state);
void drawObject(const SDLState& state, GameState& gs, GameObject& obj, float width, float height, float deltaTime);
void update(const SDLState& state, GameState& gs, Resources& res, GameObject& obj, float deltaTime);
void createTiles(const SDLState& state, GameState& gs, Resources& res);
void checkCollision(const SDLState& state, GameState& gs, Resources& res, GameObject& a, GameObject& b, float deltaTime);
void collisionResponse(const SDLState& state, GameState& gs, Resources& res, SDL_FRect& rectA, SDL_FRect& rectB, SDL_FRect& rectC, GameObject& objA, GameObject& objB, float deltaTime);
void handleKeyInput(const SDLState& state, GameState& gs, GameObject& obj, SDL_Scancode key, bool keyPressed);
void drawParalaxBackground(SDL_Renderer* renderer, SDL_Texture* texture, float camDeltaX, float& scrollPos, float scrollFactor, float deltaTime);

int main(int argc, char* argv[])
{
	SDLState state;
	state.height = 900;
	state.width = 1600;
	state.logH = 320;
	state.logW = 640;

	if (!initialize(state)) {
		return 1;
	}

	//assets
	Resources res;
	res.load(state);

	// game data
	GameState gs(state);
	createTiles(state, gs, res);
	uint64_t prevTime = SDL_GetTicks();
	bool running = true;

	while (running)
	{
		uint64_t nowTime = SDL_GetTicks();
		float deltaTime = (nowTime - prevTime) / 1000.0f;
		SDL_Event event{ 0 };
		while (SDL_PollEvent(&event)) {
			switch (event.type)
			{
			case SDL_EVENT_QUIT:
			{
				running = false;
				break;
			}
			case SDL_EVENT_WINDOW_RESIZED:
			{
				state.width = event.window.data1;
				state.height = event.window.data2;
				break;
			}
			case SDL_EVENT_KEY_DOWN:
			{
				handleKeyInput(state, gs, gs.player(), event.key.scancode, true);
				break;
			}
			case SDL_EVENT_KEY_UP:
			{
				handleKeyInput(state, gs, gs.player(), event.key.scancode, false);
				break;
			}
			}
		}

		SDL_RenderClear(state.renderer);

		gs.mapViewport.x = (gs.player().position.x + TILE_SIZE/2)- gs.mapViewport.w / 2;
		static float lastCamX = gs.mapViewport.x;
		float camDeltaX = gs.mapViewport.x - lastCamX;
		lastCamX = gs.mapViewport.x;


		// background
		SDL_RenderTexture(state.renderer, res.texBg1, nullptr, nullptr);
		SDL_RenderTexture(state.renderer, res.texBg2, nullptr, nullptr);
		drawParalaxBackground(state.renderer, res.texBg3, camDeltaX, gs.bg3Scroll, 0.150f, deltaTime);
		drawParalaxBackground(state.renderer, res.texBg4, camDeltaX, gs.bg4Scroll, 0.150f, deltaTime);
		drawParalaxBackground(state.renderer, res.texBg5, camDeltaX, gs.bg5Scroll, 0.075f, deltaTime);
		drawParalaxBackground(state.renderer, res.texBg6, camDeltaX, gs.bg6Scroll, 0.3f, deltaTime);

		// update all objects
		for (auto& layer : gs.layers)
		{
			for (GameObject& obj : layer)
			{
				update(state, gs, res, obj, deltaTime);
				// Only animate objects that actually have animations
				if (!obj.animations.empty() && obj.currentAnimation >= 0 && obj.currentAnimation < obj.animations.size())
				{
					obj.animations[obj.currentAnimation].step(deltaTime);
				}
			}
		}
		// bullet physics
		for (GameObject& bullet : gs.bullets)
		{
			update(state, gs, res, bullet, deltaTime);
			if (!bullet.animations.empty() && bullet.currentAnimation >= 0 && bullet.currentAnimation < bullet.animations.size())
			{
				bullet.animations[bullet.currentAnimation].step(deltaTime);
			}
		}

		// draw all objects
		for (auto& layer : gs.layers)
		{
			for (GameObject& obj : layer)
			{
				drawObject(state, gs, obj, TILE_SIZE, TILE_SIZE, deltaTime);
			}
		}

		// draw bullets
		for (GameObject& bullet : gs.bullets)
		{
			drawObject(state, gs, bullet, bullet.collider.w, bullet.collider.h, deltaTime);
		}

		// draw foreground tiles
		for (GameObject& obj : gs.foregroundTiles)
		{
			SDL_FRect dst{
				.x = obj.position.x - gs.mapViewport.x,
				.y = obj.position.y ,
				.w = static_cast<float> (obj.texture->w),
				.h = static_cast<float> (obj.texture->h),
			};
			SDL_RenderTexture(state.renderer, obj.texture, nullptr, &dst);
		}

		// draw background tiles
		for (GameObject& obj : gs.backgroundTiles)
		{
			SDL_FRect dst{
				.x = obj.position.x - gs.mapViewport.x,
				.y = obj.position.y ,
				.w = static_cast<float> (obj.texture->w),
				.h = static_cast<float> (obj.texture->h),
			};
			SDL_RenderTexture(state.renderer, obj.texture, nullptr, &dst);
		}

		// debug info
		SDL_SetRenderDrawColor( state.renderer, 200, 200, 200, 200);
		SDL_RenderDebugText( state.renderer, 5, 5, std::format("State {}",static_cast<int> ( gs.player().data.player.state)).c_str() );
		SDL_RenderDebugText( state.renderer,5, 20,std::format("grounded {} velY {:.2f}", gs.player().velocity.x, gs.player().velocity.y).c_str() );

		SDL_RenderPresent(state.renderer);
		prevTime = nowTime;
	}

	res.unload();
	cleanup(state);
	return 0;
}

// initialize main parameters
bool initialize(SDLState& state) {
	bool initSucces = true;

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Erro initializing SDL3", nullptr);
		initSucces = false;
	}

	// window
	state.window = SDL_CreateWindow("RPG", state.width, state.height, SDL_WINDOW_RESIZABLE);
	if (!state.window) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Erro initializing window", nullptr);
		cleanup(state);
		initSucces = false;
	}

	// renderer
	state.renderer = SDL_CreateRenderer(state.window, nullptr);
	if (!state.renderer) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Erro initializing renderer", state.window);
		cleanup(state);
		initSucces = false;
	}
	SDL_SetRenderVSync(state.renderer, 1);

	// window presentatiton
	SDL_SetRenderLogicalPresentation(state.renderer, state.logW, state.logH, SDL_LOGICAL_PRESENTATION_STRETCH);
	return initSucces;
}

// cleanup handler
void cleanup(SDLState& state) {
	SDL_DestroyWindow(state.window);
	SDL_DestroyRenderer(state.renderer);
	SDL_Quit();
}

// draw screen object handler
void drawObject(const SDLState& state, GameState& gs, GameObject& obj, float width, float height, float deltaTime)
{

	SDL_FRect src{
		.x = 0,
		.y = 0,
		.w = width,
		.h = height
	};

	if (!obj.animations.empty() && obj.currentAnimation >= 0 && obj.currentAnimation < obj.animations.size())
	{
		src.x = obj.animations[obj.currentAnimation].currentFrame() * width;
	}

	SDL_FRect dst{
		.x = obj.position.x - gs.mapViewport.x,
		.y = obj.position.y,
		.w = width,
		.h = height
	};

	SDL_FlipMode flipMode = obj.direction == 1 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

	SDL_RenderTextureRotated( state.renderer, obj.texture, &src, &dst, 0, nullptr, flipMode );
}

// synch handler
void update(const SDLState& state, GameState& gs, Resources& res, GameObject& obj, float deltaTime)
{
	if(obj.dynamic)
	{
		obj.velocity += glm::vec2(0, 500) * deltaTime;
	}
	if (obj.type == ObjectType::player)
	{
		float currentDirection = 0;
		if (state.keys[SDL_SCANCODE_A])
		{
			currentDirection += -1;
		}
		if (state.keys[SDL_SCANCODE_D])
		{
			currentDirection += 1;
		}
		if (currentDirection)
		{
			obj.direction = currentDirection;
		}
		Timer& weaponTimer = obj.data.player.weaponTimer;
		weaponTimer.step(deltaTime);

		switch (obj.data.player.state)
		{
			case PlayerState::idle:
			{
				if (currentDirection)
				{
					obj.data.player.state = PlayerState::running;
				}
				else {
					if (obj.velocity.x)
					{
						const float factor = obj.velocity.x > 0 ? -1.0f : 1.0f;
						float amount = factor * obj.acceleration.x * deltaTime;
						if (std::abs(obj.velocity.x) < std::abs(amount))
						{
							obj.velocity.x = 0;
						}
						else
						{
							obj.velocity.x += amount;
						}
					}
				}
				if (state.keys[SDL_SCANCODE_F])
				{
					weaponTimer.reset();

					GameObject bullet;
					bullet.type = ObjectType::bullet;
					bullet.direction = obj.direction;
					bullet.texture = res.texBullet;
					bullet.currentAnimation = res.ANIM_BULLET_MOVING;
					bullet.dynamic = false;

					float bw, bh;
					SDL_GetTextureSize(res.texBullet, &bw, &bh);

					bullet.collider = { 0, 0, bw, bh };

					bullet.position = obj.position + glm::vec2(
						obj.direction > 0 ? obj.collider.w : -bw,
						4.0f
					);

					bullet.velocity = glm::vec2(obj.direction * 200.0f, 0);
					bullet.animations = res.bulletAnims;

					gs.bullets.push_back(bullet);
	

				}
			obj.texture = res.texIdle;
			obj.currentAnimation = res.ANIM_PLAYER_IDLE;
			break;
			}
			case PlayerState::running:
			{
				if (!currentDirection)
				{
					obj.data.player.state = PlayerState::idle;
				}
				if (obj.velocity.x * obj.direction < 0 && obj.grounded)
				{
					obj.texture = res.texSlide;
					obj.currentAnimation = res.ANIM_PLAYER_SLIDE;
				}
				else
				{
					obj.texture = res.texRun;
					obj.currentAnimation = res.ANIM_PLAYER_RUNNING;
				}
				break;
			}
			case PlayerState::jumping:
			{
				obj.texture = res.texRun;
				obj.currentAnimation = res.ANIM_PLAYER_RUNNING;
				break;
			}
		}
		obj.velocity += currentDirection * obj.acceleration * deltaTime;
		if (std::abs(obj.velocity.x) > obj.maxSpeedX)
		{
			obj.velocity.x = currentDirection * obj.maxSpeedX;
		}
		obj.position += obj.velocity * deltaTime;

		bool foundGround = false;

		for (auto& layer : gs.layers)
		{
			for (GameObject& objB : layer)
			{
				if (&obj != &objB)
				{
					checkCollision(state, gs, res, obj, objB, deltaTime);

					SDL_FRect sensor
					{
						.x = obj.position.x + obj.collider.x,
						.y = obj.position.y + obj.collider.y + obj.collider.h,
						.w = obj.collider.w,
						.h = 1
					};
					SDL_FRect rectB{
						.x = objB.position.x + objB.collider.x,
						.y = objB.position.y + objB.collider.y,
						.w = objB.collider.w,
						.h = objB.collider.h,
					};

					if (SDL_HasRectIntersectionFloat(&sensor, &rectB))
					{
						foundGround = true;
					}
				}
			}
			if (obj.grounded != foundGround)
			{
				obj.grounded = foundGround;
			}
			if (obj.type == ObjectType::player &&obj.data.player.state == PlayerState::jumping && obj.grounded && obj.velocity.y >= 0)
			{
				obj.data.player.state = PlayerState::running;
			}


		}
	}
	if (obj.type == ObjectType::bullet)
	{
		obj.position += obj.velocity * deltaTime;
	}


}

// collision handler
void collisionResponse(const SDLState& state, GameState& gs, Resources& res, SDL_FRect& rectA, SDL_FRect& rectB, SDL_FRect& rectC, GameObject& objA, GameObject& objB, float deltaTime)
{
	if (objA.type == ObjectType::player) 
	{
		switch (objB.type)
		{
			case ObjectType::level:
			{
				if (rectC.w < rectC.h)
				{
					if (rectA.x < rectB.x)
						objA.position.x -= rectC.w; 
					else
						objA.position.x += rectC.w;  

					//objA.velocity.x = 0; possible stutter when moving
				}

				else
				{
					if (objA.velocity.y > 0)
					{
						objA.position.y -= rectC.h;
					}
					else if(objA.velocity.y < 0)
					{
						objA.position.y += rectC.h;
					}
					objA.velocity.y = 0;
				 }
			}
		}
	}
}

// collision box detector
void checkCollision(const SDLState& state, GameState& gs, Resources& res, GameObject &a, GameObject &b, float deltaTime) 
{
	SDL_FRect rectA
	{
		.x = a.position.x + a.collider.x, .y = a.position.y + a.collider.y,
		.w = a.collider.w, .h = a.collider.h

	};
	SDL_FRect rectB
	{
		.x = b.position.x + b.collider.x, .y = b.position.y + b.collider.y,
		.w = b.collider.w, .h = b.collider.h
	};
	SDL_FRect rectC{0};
	if (SDL_GetRectIntersectionFloat(&rectA, &rectB, &rectC))
	{
		collisionResponse(state, gs, res, rectA, rectB, rectC, a, b, deltaTime);
	}
}

// tile set handler
void createTiles(const SDLState& state, GameState& gs, Resources& res)
{
	/*
	1 Grass
	2 Deep grass
	3 Right Corner
	4 Left Corner
	5 Right Corner Connect
	6 Left Corner Connect
	7  Player
	*/
	short map[MAP_ROWS][MAP_COLS] = {
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{0, 4, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1, 1, 1, 1, 1, 3, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{1, 6, 2, 5, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 6, 2, 2, 2, 2, 2, 5, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
	};
	short foreground[MAP_ROWS][MAP_COLS] = {
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
	};
	short background[MAP_ROWS][MAP_COLS] = {
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
	};

	const auto loadMap = [&state, &gs, &res](short layer[MAP_ROWS][MAP_COLS])
		{
			const auto createObject = [&state](int r, int c, SDL_Texture* tex, ObjectType type)
				{
					GameObject o;
					o.type = type;
					o.position = glm::vec2(c * TILE_SIZE, state.logH - (MAP_ROWS - r) * TILE_SIZE);
					o.texture = tex;
					o.collider = { .x = 0, .y = 0, .w = TILE_SIZE, .h = TILE_SIZE };
					return o;
				};

			for (int r = 0; r < MAP_ROWS; r++)
			{
				for (int c = 0; c < MAP_COLS; c++)
				{
					switch (layer[r][c])
					{
					case 1:
					{
						GameObject o = createObject(r, c, res.texGrass, ObjectType::level);
						gs.layers[LAYER_IDX_LEVEL].push_back(o);;
						break;
					}
					case 2:
					{
						GameObject o = createObject(r, c, res.texDeepGrass, ObjectType::level);
						gs.layers[LAYER_IDX_LEVEL].push_back(o);;
						break;
					}
					case 3:
					{
						GameObject o = createObject(r, c, res.texGrassR, ObjectType::level);
						gs.layers[LAYER_IDX_LEVEL].push_back(o);;
						break;
					}
					case 4:
					{
						GameObject o = createObject(r, c, res.texGrassL, ObjectType::level);
						gs.layers[LAYER_IDX_LEVEL].push_back(o);;
						break;
					}
					case 5:
					{
						GameObject o = createObject(r, c, res.texGrassConR, ObjectType::level);
						gs.layers[LAYER_IDX_LEVEL].push_back(o);;
						break;
					}
					case 6:
					{
						GameObject o = createObject(r, c, res.texGrassConL, ObjectType::level);
						gs.layers[LAYER_IDX_LEVEL].push_back(o);;
						break;
					}
					case 7:
					{
						GameObject player = createObject(r, c, res.texIdle, ObjectType::player);
						player.position = glm::vec2(
							c * TILE_SIZE,
							state.logH - (MAP_ROWS - r) * TILE_SIZE
						);
						player.data.player = PlayerData();
						player.texture = res.texIdle;
						player.animations = res.playerAnims;
						player.currentAnimation = res.ANIM_PLAYER_IDLE;
						player.acceleration = glm::vec2(300, 0);
						player.maxSpeedX = 100;
						player.dynamic = true;
						player.collider = {
							.x = 6,
							.y = 6,
							.w = 20,
							.h = 26
						};
						gs.layers[LAYER_IDX_CHARACTERS].push_back(player);
						gs.playerIndex = gs.layers[LAYER_IDX_CHARACTERS].size() - 1;
						break;
					}
					}
				}
			}
		};
	loadMap(map);
	loadMap(foreground);
	loadMap(background);
	assert(gs.playerIndex != -1);

}

// input listener
void handleKeyInput(const SDLState& state, GameState& gs, GameObject& obj, SDL_Scancode key, bool keyPressed)
{
	const float JUMP_FORCE = -200.0f;
	if (obj.type == ObjectType::player)
	{
		switch (obj.data.player.state)
		{
			case PlayerState::idle:
			{
				if (key == SDL_SCANCODE_SPACE && keyPressed && obj.grounded)
				{
					obj.data.player.state = PlayerState::jumping;
					obj.velocity.y += JUMP_FORCE;
				}
				break;
			}
			case PlayerState::running:
			{
				if (key == SDL_SCANCODE_SPACE && keyPressed && obj.grounded)
				{
					obj.data.player.state = PlayerState::jumping;
					obj.velocity.y += JUMP_FORCE;
				}
				break;
			}
		}
	}
}


// handle the paralax background
void drawParalaxBackground(SDL_Renderer* renderer, SDL_Texture* texture, float camDeltaX, float& scrollPos, float scrollFactor, float deltaTime)
{
	scrollPos -= camDeltaX * scrollFactor;

	if (scrollPos <= -texture->w)
		scrollPos += texture->w;

	SDL_FRect dst{
		scrollPos,
		0,
		texture->w * 2.0f,
		(float)texture->h
	};

	SDL_RenderTextureTiled(renderer, texture, nullptr, 1, &dst);
}
