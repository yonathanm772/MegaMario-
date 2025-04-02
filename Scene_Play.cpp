#include "Scene_Play.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"

#include <fstream>
#include <iostream>
#include "Scene_Menu.h"

bool IsInside(Vec2 pos, std::shared_ptr<Entity> e)
{
	auto ePos = e->getComponent<CTransform>().pos;
	auto size = e->getComponent<CAnimation>().animation.getSize();

	float dx = fabs(pos.x - ePos.x);
	float dy = fabs(pos.y - ePos.y);

	//return (dx <= size.x / 2) && (dy <= size.y / 2);

	bool inside = (dx <= size.x / 2) && (dy <= size.y / 2);
	std::cout << "Mouse Click at (" << pos.x << ", " << pos.y << "), "
		<< "Entity Position: (" << ePos.x << ", " << ePos.y << "), "
		<< "Size: (" << size.x << ", " << size.y << "), "
		<< "Inside?: " << inside << "\n";

	return inside;
}

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
	int bottom_Left_CoordY_Grid = height() - (gridY * m_gridSize.y);
	Vec2 size;
	if (entity->hasComponent<CAnimation>())
	{
		size = entity->getComponent<CAnimation>().animation.getSize();
	}

	// Return the position adjusted for the grid and animation center
	return Vec2(bottom_Left_CoordX_Grid + size.x / 2, bottom_Left_CoordY_Grid - size.y / 2);
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
			tile->addComponent<CDraggable>();
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
			dec->addComponent<CDraggable>();
			//NOTE: Your final code should position the entity with the grid x,y position rtead from the file:

			//std::cout << "Loaded Animation:" << assetPath << "\n";
		}
		else if (assetType == "Player") {
			fin >> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.CX
				>> m_playerConfig.CY >> m_playerConfig.SPEED >> m_playerConfig.JUMP
				>> m_playerConfig.MAXSPEED >> m_playerConfig.GRAVITY >> m_playerConfig.WEAPON;

				spawnPlayer();

		}
		else if (assetType == "Enemy") {
			fin >> m_enemyConfig.X >> m_enemyConfig.Y >> m_enemyConfig.CX
				>> m_enemyConfig.CY >> m_enemyConfig.SPEED >> m_playerConfig.MAXSPEED
				>> m_enemyConfig.GRAVITY;

				//spawnEnemy();

		}
	}
	fin.close();
	std::cout << "Finished Loading From File" << "\n";

}

void Scene_Play::spawnPlayer()
{
	if (m_player) { m_player->destroy(); }
	std::cout << "Spawning Player:"<< "\n";
	//here is a sample player entity which you can use to construct other entities
	m_player = m_entityManager.addEntity("player");
	m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Run"), true);
	m_player->addComponent<CTransform>(gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player));
	m_player->addComponent<CInput>();
	m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY));
	m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
	m_player->addComponent<CState>("stand");
	

}

void Scene_Play::spawnEnemy()
{
	std::cout << "Spawning Enemy:" << "\n";
	//here is a sample player entity which you can use to construct other entities
	auto entity = m_entityManager.addEntity("enemy");
	entity->addComponent<CAnimation>(m_game->assets().getAnimation("GoombaWalk"), true);
	entity->addComponent<CTransform>(gridToMidPixel(m_enemyConfig.X, m_enemyConfig.Y, entity), Vec2(m_enemyConfig.SPEED,0));
	entity->addComponent<CBoundingBox>(Vec2(m_enemyConfig.CX, m_enemyConfig.CY));
	entity->addComponent<CGravity>(m_enemyConfig.GRAVITY);
	entity->addComponent<CInput>();

}

void Scene_Play::SpawnBullet(std::shared_ptr<Entity> entity)
{
		
		auto b = m_entityManager.addEntity("bullet");
		auto& transform = entity->getComponent<CTransform>();
		auto& boundingBox = entity->getComponent<CBoundingBox>();

		b->addComponent<CTransform>(transform.pos, Vec2(transform.scale.x * 12, 0), transform.scale,0);
		b->addComponent<CAnimation>(m_game->assets().getAnimation(m_playerConfig.WEAPON), true);
		b->addComponent<CBoundingBox>(m_game->assets().getAnimation(m_playerConfig.WEAPON).getSize()/2);
		b->addComponent<CLifespan>(60,(int)m_currentFrame);

}

void Scene_Play::hitBlock(std::shared_ptr<Entity> entity)
{
	// Ensure the block has an animation component
	if (!entity->hasComponent<CAnimation>())
		return;

	auto& animation = entity->getComponent<CAnimation>();

	if (animation.animation.getName() == "Brick")
	{
		entity->addComponent<CAnimation>(m_game->assets().getAnimation("Explosion"), true);
		entity->removeComponent<CBoundingBox>();
		animation.repeat = false;
	}
	else if (animation.animation.getName() == "Question")
	{

		auto coin = m_entityManager.addEntity("coin");
		coin->addComponent<CAnimation>(m_game->assets().getAnimation("Coin"), true);
		coin->addComponent<CTransform>(entity->getComponent<CTransform>().pos + Vec2(0, -64)); // Position coin above the block
		coin->addComponent<CLifespan>(30, (int)m_currentFrame); // Destroy the coin after 1 second

		entity->addComponent<CAnimation>(m_game->assets().getAnimation("Question2"), true);
		
	}
}

void Scene_Play::update()
{
	m_entityManager.update();
	//TODO: implement pause functionality
	if (!m_paused)
	{
		sMovement();
		sLifespan();
		sCollision();
		sAnimation();
		sDragAndDrop();
		m_currentFrame++;
	}

}

void Scene_Play::sMovement()
{
	auto& input = m_player->getComponent<CInput>();
	auto& transform = m_player->getComponent<CTransform>();
	auto& state = m_player->getComponent<CState>();
	Vec2 previousPlayerSpeed = transform.velocity;
	Vec2 playerInputSpeed(0, transform.velocity.y);
	//e->getComponent<CTransform>().velocity.y = 0;
	

	if (input.up)
	{
		state.state = "air";
		
	}
	if (input.down)
	{
		playerInputSpeed.y += m_playerConfig.SPEED;

	}
	if (input.left)
	{
		playerInputSpeed.x -= m_playerConfig.SPEED;
		if (state.isGrounded) state.state = "ground";
	}
	if (input.right)
	{
		playerInputSpeed.x += m_playerConfig.SPEED;
		if (state.isGrounded) state.state = "ground";
	}


	if (input.shoot && input.canShoot)
	{

		
		SpawnBullet(m_player);
		input.canShoot = false;
	}

	if (playerInputSpeed.y <= m_playerConfig.JUMP)
	{
		
		playerInputSpeed.y = m_playerConfig.JUMP;
		state.isGrounded = false;
		//std::cout << "++++++++++++INSIDE+++++++++++\n";
		//std::cout << state.isGrounded << "\n";
		//state.isGrounded = false;
		//state.state = "air";
	}
	else if (playerInputSpeed.y >= m_playerConfig.MAXSPEED)
	{
		playerInputSpeed.y = m_playerConfig.MAXSPEED;
	}

	transform.velocity = playerInputSpeed;

	if (state.isGrounded && std::abs(playerInputSpeed.x) < 0.01f)
	{
		state.state = "standing";
	}

	for (auto e : m_entityManager.getEntities())
	{
		auto& transformE = e->getComponent<CTransform>();
		auto& sprite = e->getComponent<CAnimation>().animation.getSprite();
		auto& boundingBox = e->getComponent<CBoundingBox>();
		float left = boundingBox.halfSize.x;
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
			if (e->tag() == "enemy")
			{
				if (transformE.pos.x < boundingBox.halfSize.x)
				{
					transformE.pos.x = left;
					transformE.velocity.x *= -1;

				}
			}
			

		transformE.prevPos = transformE.pos;
		transformE.pos += transformE.velocity;

	}

}

void Scene_Play::sLifespan()
{
	// TODO: Check lifespawn of entities that have them, and destroy them if the go over
	// Loop through all entities
	//auto bullets = m_entityManager.getEntities("bullet");
	for (auto e : m_entityManager.getEntities())
	{
		auto& lifespan = e->getComponent<CLifespan>();
		// Check if the entity has a lifespan component
		if (lifespan.has)
		{
			if (e->tag() == "bullet")
			{
				m_player->getComponent<CInput>().canShoot = false;
			}
			// If lifespan is still remaining, decrease it
			if (lifespan.lifespan > 0)
			{
				lifespan.lifespan -= 1;
			}

			// If lifespan has expired, deactivate the entity
			if (lifespan.lifespan <= 0)
			{
				e->destroy();
			}
		}
	}
}

void Scene_Play::sCollision()
{
	auto& tiles = m_entityManager.getEntities("tile");
	auto& bullets = m_entityManager.getEntities("bullet");

	auto& transform = m_player->getComponent<CTransform>();
	auto& state = m_player->getComponent<CState>();

	for (auto bullet : bullets)
	{
		for (auto tile : tiles)
		{
			if (!tile->hasComponent<CBoundingBox>()) { continue; }

			Vec2 overlap = Physics::getOverlap(bullet, tile);
			if (overlap.x > 0 && overlap.y > 0)
			{
				bullet->destroy();

				if (tile->getComponent<CAnimation>().animation.getName() == "Brick")
				{
					tile->addComponent<CAnimation>(m_game->assets().getAnimation("Explosion"), true);
					tile->removeComponent<CBoundingBox>();
					tile->getComponent<CAnimation>().repeat = false;
				}

			}
		}
	}
	for (auto e : m_entityManager.getEntities("player"))
	{
		bool isColliding = false;  // Track if the player collides with any tile


		for (auto t : tiles)
		{
			Vec2 overlap = Physics::getOverlap(e, t);
			Vec2 prevOverlap = Physics::getPreviousOverlap(e, t);

			Vec2 shift(0, 0);  // Position correction
			Vec2 diff = transform.pos - t->getComponent<CTransform>().pos; // Difference in position

			if (overlap.x > 0 && overlap.y > 0)  // Ensure valid collision
			{
				isColliding = true;  // Player is colliding with at least one tile

				// Handle different collision directions
				// bottom collision
				if (prevOverlap.x > 0 && transform.prevPos.y < t->getComponent<CTransform>().prevPos.y)
				{

					transform.pos.y -= overlap.y;
					transform.velocity.y = 0;
					//state.state = "ground";
					state.isGrounded = true;
					e->getComponent<CInput>().canJump = true;

					// Check if the tile is moving (a platform)
					
				}
				//top collision
				else if (prevOverlap.x > 0 && transform.prevPos.y > t->getComponent<CTransform>().prevPos.y)
				{
					std::cout << "CURRENT STATE: " << state.state << "\n";
					transform.pos.y += overlap.y;
					transform.velocity.y = 0;

					
				
					hitBlock(t);

				}
				// if there was a non-zero previous X overlap, the the collision came from x
				else if (prevOverlap.y > 0)
				{
					std::cout << "Horizontal\n";
					shift.x += diff.x > 0 ? overlap.x : -overlap.x;
				}
				transform.pos += shift;
				/*if (t->getComponent<CAnimation>().animation.getName() == "Pole" ||
					t->getComponent<CAnimation>().animation.getName() == "PoleTop")
				{
					e->destroy();
					spawnPlayer();
				}*/
			}
		}
		// If the player is NOT colliding with anything, set the state to "jumping"
		if (!isColliding)
		{
			state.state = "air";
		}
		
	}

	// check to see if the player has fallen down a hole
	if (m_player->getComponent<CTransform>().pos.y > height())
	{
		spawnPlayer();
	}

	// does not let the player exit the screen to the left
	if (m_player->getComponent<CTransform>().pos.x < m_player->getComponent<CBoundingBox>().halfSize.x)
	{
		m_player->getComponent<CTransform>().pos.x = m_player->getComponent<CBoundingBox>().halfSize.x;
	}
	

}

void Scene_Play::sDoAction(const Action& action)
{
	
	if (action.type() == "START")
	{
		auto& pState = m_player->getComponent<CState>();
		auto& pTransform = m_player->getComponent<CTransform>();
		auto& pInput = m_player->getComponent<CInput>();

		if (action.name() == "TOGGLE_TEXTURE")			{ m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION")	{ m_drawCollision = !m_drawCollision; }
		else if (action.name() == "TOGGLE_GRID")		{ m_drawGrid = !m_drawGrid; }
		else if (action.name() == "PAUSE")				{m_paused = !m_paused;}
		else if (action.name() == "QUIT")				{ onEnd(); }
		else if (action.name() == "JUMP")
		{
			if (pState.state != "air" && pInput.canJump)
			{
				pTransform.velocity.y = m_playerConfig.JUMP;
			}
			pInput.canJump = false;
			pInput.up = true;
		}
		else if (action.name() == "DOWN")
		{
			pInput.down = true;
		}
		else if (action.name() == "LEFT")
		{
			pInput.left = true;
		}
		else if (action.name() == "RIGHT")
		{	
			pInput.right = true;
		}
		else if (action.name() == "BUSTER")
		{
			pInput.shoot = true;
		}
		else if (action.name() == "LEFT_CLICK")
		{
			Vec2 worldPos = windowToWorld(action.pos());

			for (auto e : m_entityManager.getEntities())
			{
				if (e->hasComponent<CDraggable>() && IsInside(worldPos,e))
				{
					//std::cout << "Clicked Entity: " << e->getComponent<CAnimation>().animation.getName() << "\n";
					//e->getComponent<CDraggable>().dragging = !e->getComponent<CDraggable>().dragging;
					//std::cout << "Dragging?: " << e->getComponent<CDraggable>().dragging << "\n";

					auto& draggable = e->getComponent<CDraggable>();
					draggable.dragging = !draggable.dragging;

					std::cout << "Clicked Entity: " << e->getComponent<CAnimation>().animation.getName() << "\n";
					std::cout << "Dragging?: " << draggable.dragging << "\n";

				}
			}
		}
		else if (action.name() == "MOUSE_MOVE")
		{
			m_mPos = action.pos();
			m_mouseShape.setPosition(m_mPos.x, m_mPos.y);
		}
	}
	else if (action.type() == "END")
	{
		auto& pTransform = m_player->getComponent<CTransform>();
		auto& pInput = m_player->getComponent<CInput>();
		if (action.name() == "JUMP")
		{
			if (m_player->getComponent<CTransform>().velocity.y < 0)
			{
				pTransform.velocity.y = 0;
			}
			m_player->getComponent<CInput>().canJump = true;
			m_player->getComponent<CInput>().up = false;
		}
		else if (action.name() == "DOWN")
		{
			pInput.down = false;
		}
		else if (action.name() == "LEFT")
		{
			pInput.left = false;
		}
		else if (action.name() == "RIGHT")
		{
			pInput.right = false;
		}
		else if (action.name() == "BUSTER")
		{
			pInput.shoot = false;
			pInput.canShoot = true;
		}
	}
}

void Scene_Play::sDragAndDrop()
{
	for (auto e : m_entityManager.getEntities())
	{
		if (e->hasComponent<CDraggable>() && e->getComponent<CDraggable>().dragging)
		{
			Vec2 worldPos = windowToWorld(m_mPos);
			e->getComponent<CTransform>().pos = worldPos;
		}
	}
}

void Scene_Play::sAnimation()
{ 
	// Store the current scale
	auto& currentAnimation = m_player->getComponent<CAnimation>().animation;
	auto& state = m_player->getComponent<CState>();
	auto& pTransform = m_player->getComponent<CTransform>();
	//sf::Vector2f currentScale = currentAnimation.getSprite().getScale();

	if (state.state == "air")
	{
		m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Air"), true);
	}
	else if (state.state == "ground")
	{
		auto& input = m_player->getComponent<CInput>();
		if ((input.left || input.right) && !(input.left && input.right))
		{
			if (currentAnimation.getName() != "Run")
			{
				m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Run"), true);
			}
		}
		else
		{
			m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
		}
		//m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
		//m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
	}
	else
	{
		m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
	}

	for (auto e : m_entityManager.getEntities())
	{
		if (!e->hasComponent<CAnimation>()) { continue; }

		auto& cAnimation = e->getComponent<CAnimation>();

		// if the animation is not repeating and it is completed, destroy the entity
		if (cAnimation.animation.hasEnded() && !cAnimation.repeat)
		{
			e->destroy();
		}
		else
		{
			cAnimation.animation.update();
		}

	}

	
}

void Scene_Play::onEnd()
{
	//TODO: When the scene ends, change back to the meny scene
	//		use m_game->changeScene(correct params);
	m_hasEnded = true;
	m_game->changeScene("MENU", nullptr, true);
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

	m_mouseShape.setFillColor(sf::Color::Red);
	m_mouseShape.setRadius(4);
	m_mouseShape.setOrigin(2, 2);
	
	Vec2 worldPos = windowToWorld(m_mPos);
	m_mouseShape.setPosition(worldPos.x, worldPos.y);
	m_game->window().draw(m_mouseShape);
}

// wont work if we zoom in or out, have a minimap etc, need to recalculate
Vec2 Scene_Play::windowToWorld(const Vec2& window) const
{
	auto view = m_game->window().getView();

	float worldX = view.getCenter().x - (m_game->window().getSize().x / 2);
	float worldY = view.getCenter().y - (m_game->window().getSize().y / 2);

	return Vec2(window.x + worldX, window.y + worldY);
}

// shooting two bullets;
