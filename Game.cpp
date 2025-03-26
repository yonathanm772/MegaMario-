
*/*#include "Game.h"
#include <iostream>
#include <fstream>
#include <random>
#include <cmath>  // For std::fmod

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


Game::Game(const std::string& config)
{
	init(config);
	
}

void Game::init(const std::string& path)
{
	// Reads in config file here
	std::ifstream fin(path);
	if (!fin) {
		std::cerr << "Error opening file!" << std::endl;
	}

	std::string entityType;
	while (fin >> entityType) {
		if (entityType == "Window") {
			fin >> m_windowConfig.WIDTH >> m_windowConfig.HEIGHT >> m_windowConfig.FRAMES
				>> m_windowConfig.FULLSCREEN;
		}
		else if (entityType == "Font") {
			fin >> m_fontConfig.FONTFILE >> m_fontConfig.SIZE >> m_fontConfig.FR
				>> m_fontConfig.FG >> m_fontConfig.FB;
		}
		else if (entityType == "Player") {
			fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >> m_playerConfig.FR >> m_playerConfig.FG
				>> m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB
				>> m_playerConfig.OT >> m_playerConfig.V;
		}
		else if (entityType == "Enemy") {
			fin >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX
				>> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT
				>> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI;
		}
		else if (entityType == "Bullet") {
			fin >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S >> m_bulletConfig.FR
				>> m_bulletConfig.FG >> m_bulletConfig.FB >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB
				>> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;
		}
	}

	fin.close();
	// set the config window parameters

	if (m_windowConfig.FULLSCREEN == 1)
	{
		sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
		m_windowConfig.WIDTH = desktop.width;
		m_windowConfig.HEIGHT = desktop.height;
	}
	sf::Uint32 windowStyle = (m_windowConfig.FULLSCREEN == 1) ? sf::Style::Fullscreen : sf::Style::Default;

	m_window.create(sf::VideoMode(m_windowConfig.WIDTH, m_windowConfig.HEIGHT), "Circle Shooter", windowStyle);
	m_window.setFramerateLimit(m_windowConfig.FRAMES);
	spawnPlayer();
}

void Game::run()
{
	// Adds pause functionality
	while (m_running)
	{
		m_entities.update();

		if (!m_paused)
		{
			sEnemySpawner();
			sMovement();
			sCollision();
			sLifespan();
			m_currentFrame++;
		}
		sUserInput();
		sRender();
	}
}

void Game::setPaused(bool paused)
{
	m_paused = paused;
}

void Game::sMovement()
{
	// Implements all entity movement in this function
	m_player->cTransform->velocity = { 0, 0 };

	//implement player movement
	if (m_player->cInput->up)
	{
		m_player->cTransform->velocity.y -=m_playerConfig.S;
	}
	if (m_player->cInput->down)
	{
		m_player->cTransform->velocity.y += m_playerConfig.S;
	}
	if (m_player->cInput->left)
	{
		m_player->cTransform->velocity.x -= m_playerConfig.S;
	}
	if (m_player->cInput->right)
	{
		m_player->cTransform->velocity.x += m_playerConfig.S;
	}

	// Find active black holes (there might be multiple)
	std::vector<std::shared_ptr<Entity>> blackHoles = m_entities.getEntities("blackHole");

	// Update all entities' positions
	for (auto e : m_entities.getEntities())
	{

		float radius = e->cCollision->radius;  // Get the entity's radius
		float left = radius;
		float right = m_window.getSize().x - radius;
		float top = radius;
		float bottom = m_window.getSize().y - radius;

		// Check if the entity is an enemy and apply black hole attraction
		if (e->tag() == "enemy" && !blackHoles.empty())
		{
			for (auto blackHole : blackHoles)
			{
				Vec2 toBlackHole = blackHole->cTransform->pos - e->cTransform->pos; // Vector to black hole
				float distance = toBlackHole.lengthDist();

				if (distance > 0)
				{
					toBlackHole.vecNorm(distance); // Normalize direction vector
					float pullStrength = 100.0f / distance; // Attraction strength (stronger when closer)
					Vec2 attractionForce = toBlackHole * pullStrength; // Scale the pull force

					// Apply force to enemy velocity
					e->cTransform->velocity = e->cTransform->velocity + attractionForce;
				}

			}
		}
		if (e->tag() == "enemy")
		{
			if (e->cTransform->pos.x < left || e->cTransform->pos.x > right)
			{
				e->cTransform->velocity.x *= -1;  // Reverse X direction
				e->cTransform->pos.x += e->cTransform->velocity.x;
			}
			if (e->cTransform->pos.y < top || e->cTransform->pos.y > bottom)
			{
				e->cTransform->velocity.y *= -1;  // Reverse Y direction
				e->cTransform->pos.y += e->cTransform->velocity.y;
			}
		}
		if (e->tag() == "player")
		{
			if (e->cTransform->pos.x < left)
			{
				e->cTransform->pos.x = left;
				
			}
			if (e->cTransform->pos.x > right)
			{
				e->cTransform->pos.x = right;
			}
			if (e->cTransform->pos.y < top)
			{
				e->cTransform->pos.y = top;
			}
			if (e->cTransform->pos.y > bottom)
			{
				e->cTransform->pos.y = bottom;
			}
		}

		// Update position
		e->cTransform->pos.x += e->cTransform->velocity.x;
		e->cTransform->pos.y += e->cTransform->velocity.y;
	}


}

void Game::sUserInput()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			m_running = false;
		}

		// this is triggered when a key is pressed
		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				//sets player's input component "up to" true
				m_player->cInput->up = true;
				break;
			case sf::Keyboard::S:
				m_player->cInput->down = true;
				break;
			case sf::Keyboard::A:
				m_player->cInput->left = true;
				break;
			case sf::Keyboard::D:
				m_player->cInput->right = true;
				break;
			case sf::Keyboard::Escape:
				m_running = false;
				break;
			case sf::Keyboard::P:
				std::cout << "The game was: " << m_paused << "\n";
				setPaused(!m_paused);
				std::cout << "The game is: " << m_paused << "\n";
				break;
			default: break;
			}
		}

		// this event is triggered when a key is released
		if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				m_player->cInput->up = false;
				break;
			case sf::Keyboard::S:
				m_player->cInput->down = false;
				break;
			case sf::Keyboard::A:
				m_player->cInput->left = false;
				break;
			case sf::Keyboard::D:
				m_player->cInput->right = false;
				break;
			default: break;
			}
		}

		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
			}

			if (event.mouseButton.button == sf::Mouse::Right)
			{
				bool cooldown = false;
				for (auto e : m_entities.getEntities("blackHole"))
				{
					if (e->isActive())
					{
						cooldown = true;
					}
				}
				if (!cooldown)
				{
					spawnSpecialWeapon(Vec2(event.mouseButton.x, event.mouseButton.y));
				}
				
			}
		}
	}
}

void Game::sLifespan()
{
	// Loop through all entities
	for (auto e : m_entities.getEntities())
	{
		// Check if the entity has a lifespan component
		if (e->cLifespan)
		{
			
			// If lifespan is still remaining, decrease it
			if (e->cLifespan->remaining > 0)
			{

				// Compute the alpha ratio properly
				float alphaRatio = static_cast<float>(e->cLifespan->remaining) / e->cLifespan->total;
				int alphaValue = static_cast<int>(alphaRatio * 255); // Scale to 0-255 range

				// Get current fill color and update alpha
				sf::Color fillColor = e->cShape->circle.getFillColor();
				fillColor.a = alphaValue;
				e->cShape->circle.setFillColor(fillColor);

				// Get current outline color and update alpha
				sf::Color outlineColor = e->cShape->circle.getOutlineColor();
				outlineColor.a = alphaValue;
				e->cShape->circle.setOutlineColor(outlineColor);

				e->cLifespan->remaining -= 1;
			}

			// If lifespan has expired, deactivate the entity
			if (e->cLifespan->remaining <= 0)
			{
				e->destroy();
			}
		}
	}
}

void Game::sRender()
{
	m_window.clear();

	// draws all the entities
	for (auto e : m_entities.getEntities())
	{
		// set the position of the shape based on the entity's transform->pos
		e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);

		// set the rotation of the shape based on the entity's transform-> angle
		e->cTransform->angle += 1.0f;
		e->cShape->circle.setRotation(e->cTransform->angle);

		//draw the entity's sf::CircleShape
		m_window.draw(e->cShape->circle);
	}

	// font
	sf::Font font;
	if (!font.loadFromFile(m_fontConfig.FONTFILE))
	{
		std::cerr << ("Font could not be loaded");
	}
	// display text
	sf::Text text;
	text.setFont(font);

	// set the string to display
	text.setString("Score: " + std::to_string(m_score));

	// set the character size
	text.setCharacterSize(m_fontConfig.SIZE); // in pixels, not points!

	// set the color
	text.setFillColor(sf::Color(m_fontConfig.FR, m_fontConfig.FG, m_fontConfig.FB));

	m_window.draw(text);


	m_window.display();
}

void Game::sEnemySpawner()
{
	// Implements enemy spawning should go here
	// (use m_currentFrame - m_lastEnemySpawnTime) to determine
	// how long it has been since the last enemy spawned
	// 
	if((m_currentFrame - m_lastEnemySpawnTime) == m_enemyConfig.SI)
	{
		spawnEnemy();
	}
}

void Game::sCollision()
{

	for (auto b : m_entities.getEntities())
	{
		if (b->tag() == "bullet")
		{
			for (auto e : m_entities.getEntities("enemy"))
			{

				Vec2 differenceV = e->cTransform->pos.differenceVec(b->cTransform->pos);
				float distance = differenceV.distSqrt();
				float radius_sum = pow((b->cCollision->radius + e->cCollision->radius), 2);
				// if the distance is less than the sum of their radius
				if (distance < radius_sum)
				{
					b->destroy();
					e->destroy();
					if (!e->hasSpawnedSmallEnemies())
					{
						spawnSmallEnemies(e);
					}

					m_score += e->cScore->score;
				}
			}
		}

		if (b->tag() == "blackHole")
		{
			for (auto e : m_entities.getEntities("enemy"))
			{

				Vec2 differenceV = e->cTransform->pos.differenceVec(b->cTransform->pos);
				float distance = differenceV.distSqrt();
				float radius_sum = pow((b->cCollision->radius + e->cCollision->radius), 2);
				
				if (distance < radius_sum)
				{
					e->destroy();
					if (!e->hasSpawnedSmallEnemies())
					{
						spawnSmallEnemies(e);
					}

					m_score += e->cScore->score;
				}
			}
		}
		
		if (b->tag() == "player")
		{
			for (auto e : m_entities.getEntities("enemy"))
			{

				Vec2 differenceV = e->cTransform->pos.differenceVec(b->cTransform->pos);
				float distance = differenceV.distSqrt();
				float radius_sum = pow((b->cCollision->radius + e->cCollision->radius), 2);
				
				if (distance < radius_sum)
				{
					e->destroy();
					if (!e->hasSpawnedSmallEnemies())
					{
						spawnSmallEnemies(e);
					}
					float middle_of_screen_x = m_window.getSize().x / 2.0f;
					float middle_of_screen_y = m_window.getSize().y / 2.0f;
					b->cTransform->pos.x = middle_of_screen_x;
					b->cTransform->pos.y = middle_of_screen_y;

					m_score += e->cScore->score;
				}
			}
		}
		
	}
}

void Game::spawnPlayer()
{
	auto entity = m_entities.addEntity("player");

	float middle_of_screen_x = m_window.getSize().x / 2.0f;
	float middle_of_screen_y = m_window.getSize().y / 2.0f;
	// Gives this entity a Transform so it spawns at (200, 200) with velocity (1,1) at angle 0
	entity->cTransform = std::make_shared<CTransform>(Vec2(middle_of_screen_x, middle_of_screen_y), Vec2(0.0f, .0f), 0.0f);

	// Gives the entity's shape will have a radius 32, 8 siudes, dark grey fill, and red outline of thickness 4
	// use m_playerConfig.SR for example to give value to player configuration
	entity->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V,
		sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB),
		sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT);

	// Adds an input component to the player so that we can use inputs
	entity->cInput = std::make_shared<CInput>();

	entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

	// Assign this entity to be our player
	m_player = entity;

}

void Game::spawnEnemy()
{

	auto entity = m_entities.addEntity("enemy");

	float enemy_x = (rand() % ((m_window.getSize().x - m_enemyConfig.SR) - (m_enemyConfig.SR) + 1)) + (m_enemyConfig.SR);
	float enemy_y = (rand() % ((m_window.getSize().y - m_enemyConfig.SR) - (m_enemyConfig.SR) + 1)) + (m_enemyConfig.SR);
	float vertices = (rand() % ((m_enemyConfig.VMAX) - (m_enemyConfig.VMIN) + 1)) + (m_enemyConfig.VMIN);
	float colorR = (rand() % ((255) - (0) + 1)) + (0);
	float colorG = (rand() % ((255) - (0) + 1)) + (0);
	float colorB = (rand() % ((255) - (0) + 1)) + (0);
	float angle = (rand() % ((360) - (0) + 1)) + (0);
	//generates a random speed for the enemy
	float enemySpeed = (rand() % ((m_enemyConfig.SMAX) - (m_enemyConfig.SMIN) + 1)) + (m_enemyConfig.SMIN);
	Vec2 v(1, 1); 
	Vec2 speedVector = v.velocity(enemySpeed, angle);

	entity->cTransform = std::make_shared<CTransform>(Vec2(enemy_x, enemy_y), Vec2(speedVector.x, speedVector.y) , 0.0f);
	
	entity->cShape = std::make_shared<CShape>(m_enemyConfig.SR, vertices,
		sf::Color(colorR, colorG, colorB),
		sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT);

	
	entity->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);
	// Adds an input component to the enemy so that we can use inputs
	entity->cInput = std::make_shared<CInput>();

	// add a points component
	entity->cScore = std::make_shared<CScore>(entity->cShape->circle.getPointCount() * 100);

	// record when the most recent enemy was spawned
	m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> entity)
{
	if (entity->hasSpawnedSmallEnemies())
	{
		return;
	}
	

	int numSmallEnemies = entity->cShape->circle.getPointCount();
	float smallRadius = entity->cShape->circle.getRadius() / 2;
	sf::Color fillColor = entity->cShape->circle.getFillColor();
	sf::Color outlineColor = entity->cShape->circle.getOutlineColor();
	float outlineThickness = entity->cShape->circle.getOutlineThickness();
	int newScore = entity->cScore->score * 2;

	float baseSpeed = std::sqrt(std::pow(entity->cTransform->velocity.x, 2) +
		std::pow(entity->cTransform->velocity.y, 2));

	for (int i = 0; i < numSmallEnemies; i++)
	{
		auto e = m_entities.addEntity("enemy");
		// Mark that small enemies have been spawned for this entity
		e->setSmallEnemySpawned();

		// Calculate angle in radians
		float angle = (i * 360.0f / numSmallEnemies) * (M_PI / 180.0f);

		// Set position same as original entity
		Vec2 spawnPos = entity->cTransform->pos;

		// Calculate new velocity using angle (spread them out evenly)
		float newSpeed = baseSpeed * 1.2f; // Optional: Increase speed slightly
		Vec2 velocity = Vec2(std::cos(angle) * newSpeed, std::sin(angle) * newSpeed);

		e->cTransform = std::make_shared<CTransform>(spawnPos, velocity, angle);

		e->cShape = std::make_shared<CShape>(smallRadius, numSmallEnemies,
			fillColor, outlineColor, outlineThickness);

		e->cCollision = std::make_shared<CCollision>(smallRadius);

		e->cInput = std::make_shared<CInput>();

		e->cScore = std::make_shared<CScore>(newScore);
		
		e->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);
	}	

}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousePos)
{
	// - bullet speed is given as a scalar speed
	// - must set the velocity by using formula in notes
	// - position of the bullet is the position of the entity that spawned it std::shared_ptr<Entity> entity

	auto bullet = m_entities.addEntity("bullet");

	Vec2 differenceV = entity->cTransform->pos.differenceVec(mousePos);
	float length = differenceV.lengthDist();
	differenceV.vecNorm(length);

	bullet->cTransform = std::make_shared<CTransform>(entity->cTransform->pos,Vec2(10 * differenceV.x, 10 * differenceV.y), 0);
	bullet->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V,
		sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB),
		sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB),
		m_bulletConfig.OT);

	bullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
	

	bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);

}

void Game::spawnSpecialWeapon(const Vec2& mousePos)
{
	// Create the black hole bullet entity
	auto blackHole = m_entities.addEntity("blackHole");

	// Set bullet's position at the mouse position (black hole's center)
	blackHole->cTransform = std::make_shared<CTransform>(mousePos, Vec2(0, 0), 0);

	// Set bullet's shape and color
	blackHole->cShape = std::make_shared<CShape>(m_bulletConfig.SR * 8, m_bulletConfig.V,
		sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB),
		sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB),
		m_playerConfig.OT);

	// Set bullet's collision radius
	blackHole->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR*8);

	// Set lifespan of the black hole
	blackHole->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L * 5);
}
*/


