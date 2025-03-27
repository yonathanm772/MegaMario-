#include "Scene_Play.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"

#include <fstream>
#include <iostream>



Scene_Play::Scene_Play()
{
}

Scene_Play::Scene_Play(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine)
	, m_levelPath(levelPath)
{
	init(m_levelPath);
	
}


void Scene_Play::init(const std::string& levelPath)
{
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
	registerAction(sf::Keyboard::G, "TOGGLE_GRID");
	registerAction(sf::Keyboard::W, "JUMP");
	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::Space, "BUSTER");

	m_gridText.setCharacterSize(12);
	m_gridText.setFont(m_game->assets().getFont("Tech"));
	std::cout << "LevelPath: " << levelPath << "\n";
	loadLevel(levelPath);

}

Vec2 Scene_Play::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{

	// TODO: This function takes in a grid (x,y) position and an entity
	//		Return a Vec2 indicating where the CENTER position of the entity should be
	//		You must use the Entity's Animation size to position it correctly
	//		The size of the grid width and height is stored in m_gridSize.x and m_gridSize.y
	//		The bottom-left corner of the Animation should align with the bottom left of the grid cell

	int bottom_Left_CoordX_Grid = gridX * m_gridSize.x;
	int bottom_Left_CoordY_Grid = gridY * m_gridSize.y;

	float animation_sizeX = 0.0f;
	float animation_sizeY = 0.0f;

	if (entity->hasComponent<CAnimation>())
	{
		auto& animation = entity->getComponent<CAnimation>().animation;

		// Ensure the animation has a valid texture before accessing its size
		if (animation.getSprite().getTexture())
		{
			std::cout << "+++++++++++++++++++++++++ANIMATION SET\n";
			animation_sizeX = animation.getSize().x / 2;
			animation_sizeY = animation.getSize().y / 2;
		}
	}

	// Return the position adjusted for the grid and animation center
	return Vec2(
		animation_sizeX + bottom_Left_CoordX_Grid,
		m_game->window().getSize().y - (animation_sizeY + bottom_Left_CoordY_Grid)
	);
}

void Scene_Play::loadLevel(const std::string& filename)
{

	m_entityManager = EntityManager();
	//TODO: read in the level file and add the appropriate entities
	//		use the PlayerConfig Struct m_playerConfig to store player properties
	//		this struct is defined at the top of Scene_play.h
	// Reads in config file here

	std::cout << "Loading From File" << "\n";

	std::ifstream fin(filename);
	if (!fin) {
		std::cerr << "Error opening file!" << std::endl;
	}
	std::string assetType, asset;
	float xCoord = 0.0f,
		yCoord = 0.0f;
	while (!fin.eof())
	{
		fin >> assetType;
		if (assetType == "Tile") {
			fin >> asset >> xCoord >> yCoord;

			auto tile = m_entityManager.addEntity("tile");
			//IMPORTANT: always add the CAnimation component first so that gridToMidPixel can compute correctly
			tile->addComponent<CAnimation>(m_game->assets().getAnimation(asset), true);
			tile->addComponent<CTransform>(gridToMidPixel(xCoord, yCoord, tile));
			tile->addComponent<CBoundingBox>(m_game->assets().getAnimation(asset).getSize());
			//NOTE: Your final code should position the entity with the grid x,y position rtead from the file:

		}
		else if (assetType == "Dec") {

			fin >> asset >> xCoord >> yCoord;

			auto dec = m_entityManager.addEntity("dec");
			//IMPORTANT: always add the CAnimation component first so that gridToMidPixel can compute correctly
			dec->addComponent<CAnimation>(m_game->assets().getAnimation(asset), true);
			dec->addComponent<CTransform>(gridToMidPixel(xCoord, yCoord, dec));
			std::cout << "+++++++++++++++DEC NAME: " << dec->getComponent<CAnimation>().animation.getName() << "\n";
			std::cout << "+++++++++++++++DEC COORD: " << dec->getComponent<CTransform>().pos.x << ", " << dec->getComponent<CTransform>().pos.y << "\n";
			//NOTE: Your final code should position the entity with the grid x,y position rtead from the file:

			//std::cout << "Loaded Animation:" << assetPath << "\n";
		}
		else if (assetType == "Player") {
			fin >> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.CX
				>> m_playerConfig.CY >> m_playerConfig.SPEED >> m_playerConfig.JUMP
				>> m_playerConfig.MAXSPEED >> m_playerConfig.GRAVITY >> m_playerConfig.WEAPON;

				spawnPlayer();

		}
	}
	fin.close();
	std::cout << "Finished Loading From File" << "\n";

}

void Scene_Play::spawnPlayer()
{
	std::cout << "Spawning Player:"<< "\n";
	//here is a sample player entity which you can use to construct other entities
	m_player = m_entityManager.addEntity("player");
	m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
	m_player->addComponent<CTransform>(gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player));
	m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY));
	m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
	m_player->addComponent<CState>("stand");
	m_player->addComponent<CInput>();

}

void Scene_Play::SpawnBullet(std::shared_ptr<Entity> entity)
{
	std::cout << "Spawning Bullet:" << "\n";
	if (entity->getComponent<CInput>().canShoot)
	{
		std::cout << "Creating Bullet:" << "\n";
		auto b = m_entityManager.addEntity("bullet");


		auto& transform = entity->getComponent<CTransform>();
		auto& boundingBox = entity->getComponent<CBoundingBox>();

		b->addComponent<CAnimation>(m_game->assets().getAnimation("Buster"), true);
		b->addComponent<CTransform>(Vec2(transform.pos.x, transform.pos.y), Vec2(transform.scale.x * 10,0));
		b->addComponent<CBoundingBox>(Vec2(boundingBox.size.x, boundingBox.size.y));

		b->getComponent<CTransform>().scale = transform.scale;

	}
	


}

void Scene_Play::update()
{
	m_entityManager.update();
	//TODO: implement pause functionality
	/*if (!m_paused)
	{
		sMovement();
		sLifespan();
		sCollision();
		sAnimation();
	}
	sRender();*/

	sMovement();
	sLifespan();
	sCollision();
	sAnimation();
	sRender();

}

void Scene_Play::sMovement()
{
	auto& input = m_player->getComponent<CInput>();
	auto& transform = m_player->getComponent<CTransform>();
	auto& state = m_player->getComponent<CState>();
	const float jumpHoldReduction = 0.5f; // Reduces gravity while holding jump

	//e->getComponent<CTransform>().velocity.y = 0;
	Vec2 newVelocity(0, transform.velocity.y);

	if (input.up && state.isGrounded)
	{
		std::cout << "++++++++++++INSIDE UP++++++++++\n";
		std::cout << state.isGrounded <<"\n";
		//state.state = "jumping";
		newVelocity.y -= m_playerConfig.SPEED;
		
	}
	if (input.down)
	{
		newVelocity.y += m_playerConfig.SPEED;

	}
	if (input.left)
	{
		//m_player->getComponent<CState>().state = "air";

		newVelocity.x -= m_playerConfig.SPEED;
		if (state.isGrounded) state.state = "running";
	}
	if (input.right)
	{
		//m_player->getComponent<CState>().state = "air";
		newVelocity.x += m_playerConfig.SPEED;
		if (state.isGrounded) state.state = "running";

		/*std::cout << "++++++++++++INSIDE RIGHT++++++++++\n";a
		std::cout << state.isGrounded <<"\n";
		if (state.isGrounded) { state.state = "running"; }
		std::cout << "++++++++++++INSIDE AFTER++++++++++\n";
		std::cout << state.state <<"\n";*/
	}

	if (newVelocity.y <= m_playerConfig.JUMP)
	{
		
		newVelocity.y = m_playerConfig.JUMP;
		state.isGrounded = false;
		//std::cout << "++++++++++++INSIDE+++++++++++\n";
		//std::cout << state.isGrounded << "\n";
		//state.isGrounded = false;
		//state.state = "air";
	}
	else if (newVelocity.y >= m_playerConfig.MAXSPEED)
	{
		newVelocity.y = m_playerConfig.MAXSPEED;
	}

	transform.velocity = newVelocity;

	if (state.isGrounded && std::abs(newVelocity.x) < 0.01f)
	{
		state.state = "standing";
	}

	for (auto e : m_entityManager.getEntities())
	{
		auto& transformE = e->getComponent<CTransform>();
		auto& sprite = e->getComponent<CAnimation>().animation.getSprite();
		//float scaleX = std::abs(sprite.getScale().x);

			if (transformE.velocity.x > 0 && transformE.scale.x != 1)//transformE.lastDirection != 1)
			{
				
				//transformE.lastDirection = 1;
				//sprite.setScale(scaleX, sprite.getScale().y);  // Face right
				transformE.scale.x = 1;
				sprite.setScale(transformE.scale.x, transformE.scale.y);
			
			}
			else if (transformE.velocity.x < 0 && transformE.scale.x != -1)//transformE.lastDirection != -1)
			{
				//transformE.lastDirection = -1;
				//sprite.setScale(-scaleX, sprite.getScale().y); // Face left
				transformE.scale.x = -1;
				sprite.setScale(transformE.scale.x, transformE.scale.y);
				
			}

			if (e->hasComponent<CGravity>())
			{
				e->getComponent<CTransform>().velocity.y += e->getComponent<CGravity>().gravity;
	
			}

		transformE.prevPos = transformE.pos;
		transformE.pos += transformE.velocity;

	}
}

void Scene_Play::sLifespan()
{
	// TODO: Check lifespawn of entities that have them, and destroy them if the go over
}

void Scene_Play::sCollision()
{
	//REMEMBER: SFML's (0,0) position is on the TOP-LEFT corner
	//			This means jumping will have a negative y-component
	//			and gravity will have a positive y-component
	//			Also, something BELOW something else will have a y value GREATER than it
	//			Also, something ABOVE something else will have a y vallue LESS than it.

	//TODO: Implement Physics::GetOverlap() function, us it inside this function
	for (auto e : m_entityManager.getEntities())
	{
		if (e->tag() == "player")
		{
			bool isColliding = false;  // Track if the player collides with any tile
			auto& transform = e->getComponent<CTransform>();
			auto& state = e->getComponent<CState>();

			for (auto t : m_entityManager.getEntities("tile"))
			{
				Physics physics;
				Vec2 overlap = physics.getOverlap(e, t);
				Vec2 prevOverlap = physics.getPreviousOverlap(e, t);

				if (overlap.x > 0 && overlap.y > 0)  // Ensure valid collision
				{
					isColliding = true;  // Player is colliding with at least one tile

					// Handle different collision directions
					if (prevOverlap.y > 0 && transform.prevPos.x > t->getComponent<CTransform>().prevPos.x)
					{
						transform.pos.x += overlap.x;
					}
					else if (prevOverlap.y > 0 && transform.prevPos.x < t->getComponent<CTransform>().prevPos.x)
					{
						transform.pos.x -= overlap.x;
					}
					else if (prevOverlap.x > 0 && transform.prevPos.y < t->getComponent<CTransform>().prevPos.y)
					{
						transform.pos.y -= overlap.y;
						transform.velocity.y = 0;
						state.isGrounded = true;
						e->getComponent<CInput>().canJump = true;
					}
					else if (prevOverlap.x > 0 && transform.prevPos.y > t->getComponent<CTransform>().prevPos.y)
					{
						transform.pos.y += overlap.y;
						transform.velocity.y = 0;
						t->getComponent<CAnimation>().repeat = false;
						t->getComponent<CAnimation>().animation.hasEnded();
					}
				}
			}

			// If the player is NOT colliding with anything, set the state to "jumping"
			if (!isColliding)
			{
				state.state = "jumping";
			}
			// If the player is on the ground AND not moving, set the state to "standing"

			std::cout << "**************CURRENT STATE**************\n";
			std::cout << state.state << "\n";
		}

		if (e->tag() == "bullet")
		{
				for (auto t : m_entityManager.getEntities("tile"))
				{
					Physics physics;
					Vec2 overlap = physics.getOverlap(e, t);
					if (overlap.x > 0 && overlap.y)
					{
						if (t->getComponent<CAnimation>().animation.getName() == "Brick")
						{
							t->destroy();
						}
						e->destroy();
					}
					//TODO: Implement bullet  / tile collision
					//		Destroy the tile if it has a Brick animation
					//TODO: Implement player / tile collisions and resolutions
					//		Update the CState component of the player to store whether
					//		it is currently on the ground or in the air. This will be 
					//		used by the Animation system
					//TODO: Check to see if the player has fallen down a hole (y >height())
					//TODO: Don't let the player walk off the left side of the map
				}
		}

	}
	

}

void Scene_Play::sDoAction(const Action& action)
{	
	
	std::cout << "Action Type: " << action.type() << "\n";
	std::cout << "Action Name: " << action.name() << "\n";
	if (action.type() == "START")
	{
	
			 if (action.name() == "TOGGLE_TEXTURE")		{ m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION")	{ m_drawCollision = !m_drawCollision; }
		else if (action.name() == "TOGGLE_GRID")		{ m_drawGrid = !m_drawGrid; }
		else if (action.name() == "PAUSE")				{ setPaused(!m_paused); }
		else if (action.name() == "QUIT")				{ onEnd(); }
		else if (action.name() == "JUMP")
		{

			m_player->getComponent<CInput>().up = true;
			m_player->getComponent<CInput>().canJump = false;
	
		}
		else if (action.name() == "DOWN")
		{
			m_player->getComponent<CInput>().down = true;
		}
		else if (action.name() == "LEFT")
		{

				m_player->getComponent<CInput>().left = true;
				m_player->getComponent<CState>().isRunning = true;
				/*if (m_player->getComponent<CState>().isGrounded)
				{
					m_player->getComponent<CState>().state = "running";
				}*/
		}
		else if (action.name() == "RIGHT")
		{	
				 
				 m_player->getComponent<CInput>().right = true;
				 m_player->getComponent<CState>().isRunning = true;
				 //m_player->getComponent<CState>().state = "air";
				 /*if (m_player->getComponent<CState>().isGrounded)
				 {
					 m_player->getComponent<CState>().state = "running";
				 }*/
		}
		else if (action.name() == "BUSTER")
		{
			SpawnBullet(m_player);
			m_player->getComponent<CInput>().canShoot = false;
		}
	}
	else if (action.type() == "END")
	{
		if (action.name() == "JUMP")
		{
			m_player->getComponent<CInput>().up = false;
			//m_player->getComponent<CState>().state = "air";
		}
		else if (action.name() == "DOWN")
		{
			m_player->getComponent<CInput>().down = false;
		}
		else if (action.name() == "LEFT")
		{
			m_player->getComponent<CState>().isRunning = false;
			m_player->getComponent<CInput>().left = false;
			/*if (m_player->getComponent<CState>().isGrounded && !m_player->getComponent<CInput>().right)
			{
				m_player->getComponent<CState>().state = "standing";
			}*/
		}
		else if (action.name() == "RIGHT")
		{
			m_player->getComponent<CState>().isRunning = false;
			m_player->getComponent<CInput>().right = false;
			/*if (m_player->getComponent<CState>().isGrounded && !m_player->getComponent<CInput>().left)
			{
				m_player->getComponent<CState>().state = "standing";
			}*/
		}
		else if (action.name() == "BUSTER")
		{
			m_player->getComponent<CInput>().canShoot = true;
		}
	}
}

void Scene_Play::sAnimation()
{ 
	// Store the current scale
	auto& currentAnimation = m_player->getComponent<CAnimation>().animation;
	auto& state = m_player->getComponent<CState>();
	auto& input = m_player->getComponent<CInput>();
	sf::Vector2f currentScale = currentAnimation.getSprite().getScale();
	if (state.state == "jumping"
		&& !input.canJump )
	{
		if (currentAnimation.getName() != "Air")
		{
			currentAnimation = m_game->assets().getAnimation("Air");
		}
		//add the speed variable to the current frame
		//std::cout << "FRAME COUNT BEFORE " << animation.animation.getFrameCount() << "\n";
		//std::cout << "SPEED AFTER " << animation.animation.getSpeed() << "\n";
		//std::cout << "TEXTURE SIZE X " << animation.animation.getSize().x << "\n";
		//std::cout << "TEXTURE SIZE Y " << animation.animation.getSize().y << "\n";
	}
	else if (state.state == "running")
	{
		if (currentAnimation.getName() != "Run")
		{
			currentAnimation = m_game->assets().getAnimation("Run");
		}
		//add the speed variable to the current frame
		//std::cout << "FRAME COUNT BEFORE " << animation.animation.getFrameCount() << "\n";
		//std::cout << "SPEED AFTER " << animation.animation.getSpeed() << "\n";
		//std::cout << "TEXTURE SIZE X " << animation.animation.getSize().x << "\n";
		//std::cout << "TEXTURE SIZE Y " << animation.animation.getSize().y << "\n";
	}
	else if(state.state == "standing")
	{
		if (currentAnimation.getName() != "Stand")
		{
			currentAnimation = m_game->assets().getAnimation("Stand");
		}
		//add the speed variable to the current frame
	}

	for (auto e : m_entityManager.getEntities())
	{
		if (e->getComponent<CAnimation>().has)
		{
			if (!e->getComponent<CAnimation>().repeat)
			{
				if (e->getComponent<CAnimation>().animation.getName() == "Question")
				{
					e->addComponent<CAnimation>(m_game->assets().getAnimation("Question2"), true);
				}
				else
					e->destroy();
				
				
			}
			else
				e->getComponent<CAnimation>().animation.update();
		}
	}
	//TODO: set the animation of the player based on its CState component
	//TODO: for each entity with an animation, call entity->getComponent<CAnimation>().animation.update()
	//		if the animation is not repeated, and it has ended, destroy entity
}

void Scene_Play::onEnd()
{
	//TODO: When the scene ends, change back to the meny scene
	//		use m_game->changeScene(correct params);
}

void Scene_Play::sRender()
{

	// color the background darker so you know that the game is paused
	if (!m_paused)
		m_game->window().clear(sf::Color(100, 100, 255));
	else
		m_game->window().clear(sf::Color(50, 50, 150));

	// set the viewport of the window to be cented on the player if its far enough right
	auto& pPos = m_player->getComponent<CTransform>().pos;
	float windowCenterX = std::max(m_game->window().getSize().x / 2.0f, pPos.x);
	sf::View view = m_game->window().getView();
	view.setCenter(windowCenterX, m_game->window().getSize().y - view.getCenter().y);
	m_game->window().setView(view);

	//draw all Entity textures / animations 
	if (m_drawTextures)
	{
		for (auto e : m_entityManager.getEntities())
		{
			auto& transform = e->getComponent<CTransform>();

			if (e->hasComponent<CAnimation>())
			{
				auto& animation = e->getComponent<CAnimation>().animation;
				float scaleX, scaleY, currentXScale, currentYScale;
					// If the entity has a bounding box, fit the texture to it
				if (e->hasComponent<CBoundingBox>())
				{
					auto& boundingBox = e->getComponent<CBoundingBox>();
					sf::Vector2u textureSize (animation.getSize().x, animation.getSize().y);
					scaleX = boundingBox.size.x / textureSize.x;
					scaleY = boundingBox.size.y / textureSize.y;

					currentXScale = animation.getSprite().getScale().x; // Preserve left/right direction
					currentYScale = animation.getSprite().getScale().y;
					float directionX = transform.scale.x;
					animation.getSprite().setScale(scaleX* directionX, scaleY);

					
				}
				
				animation.getSprite().setRotation(transform.angle);
				animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
				//animation.getSprite().setScale(transform.scale.x, transform.scale.y);
				m_game->window().draw(animation.getSprite());
			}
		}
	}

	// draw all entity collision bounding boxes with rectangleshape
	if (m_drawCollision)
	{
		
		for (auto e : m_entityManager.getEntities())
		{
			if (e->hasComponent<CBoundingBox>())
			{
				
				auto& box = e->getComponent<CBoundingBox>();
				auto& transform = e->getComponent < CTransform>();
				sf::RectangleShape rect;
				rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
				rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
				rect.setPosition(transform.pos.x, transform.pos.y);
				rect.setFillColor(sf::Color(0, 0, 0, 0));
				rect.setOutlineColor(sf::Color(255, 255, 255, 255));
				rect.setOutlineThickness(1);
				m_game->window().draw(rect);
				
			}


		}
	}

	//draw the grid so that students can easily debug
	if (m_drawGrid)
	{
		float leftX = m_game->window().getView().getCenter().x - width() / 2;
		float rightX = leftX + width() + m_gridSize.x;
		float nextGridX = leftX - ((int)leftX % (int)m_gridSize.x);

		for (float x = nextGridX; x < rightX; x += m_gridSize.x)
		{
			drawLine(Vec2(x, 0), Vec2(x, height()));
		}

		for (float y = 0; y < height(); y += m_gridSize.y)
		{

			drawLine(Vec2(leftX, height() - y), Vec2(rightX, height() - y));

			for (float x = nextGridX; x < rightX; x += m_gridSize.x)
			{
				
				std::string xCell = std::to_string((int)x / (int)m_gridSize.x);
				std::string yCell = std::to_string((int)y / (int)m_gridSize.y);
				m_gridText.setString("(" + xCell + "," + yCell + ")");
				m_gridText.setPosition(x + 3, height() - y - m_gridSize.y + 2);
				m_game->window().draw(m_gridText);
			}

		}
	}
	m_game->window().display();
}
// fix running, and jumpin along with their animations.

