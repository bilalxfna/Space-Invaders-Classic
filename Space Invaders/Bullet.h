#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include "Shooter.h"
#include "Shield.h"
#include "UFO.h"
#include "InvaderFormation.h"

using namespace std;
using namespace sf;

class Bullet
{
private:
	Texture texture;
	Sprite sprite;
	Vector2f position;
	bool fireStatus = false;
	float speed = 0;
	Music fireMusic;
	int shotCount = 0;
	Texture projTextures[3];
	bool projTexturesLoaded = false;

public:
	Bullet(float scale = 0.1f, const string& texturePath = "Sprites/Projectiles/bullet.png", const string& musicPath = "Audio/FireBullet.wav")
	{
		texture.loadFromFile(texturePath);
		sprite.setTexture(texture);
		sprite.setScale(scale, scale);
		sprite.setColor(Color::Cyan);

		fireMusic.openFromFile(musicPath);

		FloatRect bulletBounds = sprite.getLocalBounds();
		sprite.setOrigin(bulletBounds.width / 2.f, bulletBounds.height / 2.f);

		if (!projTexturesLoaded)
		{
			projTextures[0].loadFromFile("Sprites/Projectiles/ProjectileA.png");
			projTextures[1].loadFromFile("Sprites/Projectiles/ProjectileB.png");
			projTextures[2].loadFromFile("Sprites/Projectiles/ProjectileC.png");
			projTexturesLoaded = true;
		}

	}

	// Getters 
	Vector2f getPosition()
	{
		return position;
	}
	Sprite& getSprite()
	{
		return sprite;
	}
	bool& getFireStatus() { return fireStatus; }
	Color getColor() { return sprite.getColor(); }
	int getShotCount() { return shotCount; }

	// Setters
	void setPosition(float x, float y)
	{
		position.x = x;
		position.y = y;
		sprite.setPosition(position);
	}
	void setPosition(Vector2f position)
	{
		this->position = position;
	}
	void setFireStatus(bool status) { this->fireStatus = status; }
	void setColor(Color color) { sprite.setColor(color); }
	void setSpeed(float speed) { this->speed = speed; }
	void changeTexture(const string& texturePath)
	{
		texture.loadFromFile(texturePath);
		sprite.setTexture(texture);
	}

	// Hitting Functions
	void increaseShotCount() { shotCount++; }
	bool hitShield(Shield&, float, float);
	bool hitInvader(InvaderFormation&, int&, bool);
	bool hitUFO(UFO&, int&);
	bool hitShooter(Shooter&, bool);
	bool hitBullet(Bullet&);

	// Setting up the Bullet
	void setToCenterOfShooter(Shooter& shooter);
	void setupFromInvader(Invader*, float);

	// Fire
	void fire(float deltaTime, int stopAt = 0, char ch = 'S')
	{
		if (ch != 'S')
		{
			// ensure textures are loaded
			if (!projTexturesLoaded)
			{
				projTextures[0].loadFromFile("Sprites/Projectiles/ProjectileA.png");
				projTextures[1].loadFromFile("Sprites/Projectiles/ProjectileB.png");
				projTextures[2].loadFromFile("Sprites/Projectiles/ProjectileC.png");
				projTexturesLoaded = true;
			}

			int idx = ch - 'A';
			if (idx >= 0 && idx < 3)
			{
				sprite.setTexture(projTextures[idx]);
				sprite.setScale(2.5f, 2.5f);
				// center origin if needed:
				sf::FloatRect bounds = sprite.getLocalBounds();
				sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
			}
		}

		// movement
		position.y -= speed * deltaTime;
		sprite.setPosition(position);

		// stop condition
		if ((speed > 0 && position.y <= stopAt) || (speed < 0 && position.y >= stopAt))
		{
			fireStatus = false;
		}
	}
	void playFireMusic(bool mute)
	{
		fireMusic.play();
		fireMusic.setVolume(mute * 100);
	}


	//Draw
	void draw(RenderWindow& window)
	{
		sprite.setPosition(position);
		window.draw(sprite);
	}
};

void Bullet::setToCenterOfShooter(Shooter& shooter)
{
	FloatRect shooterBounds = shooter.getSprite().getGlobalBounds();
	Vector2f shooterCenter(shooterBounds.left + shooterBounds.width / 2.f,
		shooterBounds.top + shooterBounds.height / 2.f);

	FloatRect bulletBounds = sprite.getLocalBounds();
	sprite.setOrigin(bulletBounds.width / 2.f, bulletBounds.height / 2.f);

	position = shooterCenter;
	sprite.setPosition(position); // safe since origin is centered
}

bool Bullet::hitShield(Shield& shield, float shieldX, float shieldY)
{
	float pixelSize = shield.getPixelSize();

	float localX = position.x - shieldX;
	float localY = position.y - shieldY;

	int col = localX / pixelSize;
	int row = localY / pixelSize;

	if (row >= 0 && row < shield.getRows() && col >= 0 && col < shield.getCols())
	{
		if (shield[row][col].isActive())
		{
			shield[row][col] = false;
			return true;
		}
	}

	return false;
}

bool Bullet::hitInvader(InvaderFormation& formation, int& score, bool mute)
{
	for (int i = 0; i < formation.getRows(); i++)
	{
		for (int j = 0; j < formation.getCols(); j++)
		{
			Invader& inv = formation[i][j];
			if (inv.isAlive() && sprite.getGlobalBounds().intersects(inv.getSprite().getGlobalBounds()))
			{
				if (int(i / 2) == 0) score += 30;
				if (int(i / 2) == 1) score += 20;
				if (int(i / 2) == 2) score += 10;
				inv.destroyInvader(mute);
				return true;
			}
		}
	}
	return false;
}

bool Bullet::hitUFO(UFO& ufo, int& score)
{
	if (!sprite.getGlobalBounds().intersects(ufo.getSprite().getGlobalBounds())) return false;

	ufo.destroy();

	return true;
}

bool Bullet::hitBullet(Bullet& bullet)
{
	if (sprite.getGlobalBounds().intersects(bullet.getSprite().getGlobalBounds())) return true;
	return false;
}
void Bullet::setupFromInvader(Invader* invader, float speed)
{
	if (!invader) return;

	setPosition(invader->getPosition());
	setSpeed(-speed);
	setColor(invader->getColor());
}

bool Bullet::hitShooter(Shooter& shooter, bool mute)
{
	if (sprite.getGlobalBounds().intersects(shooter.getSprite().getGlobalBounds()))
	{
		shooter.isAlive() = false;
		shooter.startExplosion(mute);
		return true;
	}
	return false;
}