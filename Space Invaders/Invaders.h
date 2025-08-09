#pragma once
#include<SFML/Graphics.hpp>
#include<SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

using namespace std;
using namespace sf;

void setFrame1Textures(Texture textures[])
{
	textures[0].loadFromFile("Sprites/Invaders/Squid_1.png");
	textures[1].loadFromFile("Sprites/Invaders/Crab_1.png");
	textures[2].loadFromFile("Sprites/Invaders/Octopus_1.png");
}
void setFrame2Textures(Texture textures[])
{
	textures[0].loadFromFile("Sprites/Invaders/Squid_2.png");
	textures[1].loadFromFile("Sprites/Invaders/Crab_2.png");
	textures[2].loadFromFile("Sprites/Invaders/Octopus_2.png");
}

// Invaders:
// - Squid	 : Green
// - Crab	 : Purple
// - Octopus : Orange

class Invader
{
private:
	Texture texture;
	Sprite sprite;
	bool lifeStatus;
	Vector2f position;
	bool isExploding = false;
	Clock explosionClock;
	Music music;

public:
	Invader()
	{
		lifeStatus = true;
		music.openFromFile("Audio/InvaderDestroyed.wav");
	}
	Invader(const string& texturePath, float scale)
	{
		lifeStatus = true;
		texture.loadFromFile(texturePath);
		sprite.setTexture(texture);
		sprite.setScale(scale, scale);
	}

	// Getters
	Vector2f& getPosition() { return position; }
	float getExplosionElapsedTime()
	{
		return explosionClock.getElapsedTime().asSeconds();
	}
	Sprite& getSprite() { return sprite; }
	Color getColor() { return sprite.getColor(); }
	bool isAlive() { return lifeStatus; }

	// Setters
	void setSprite(Sprite sprite) { this->sprite = sprite; }
	void setColor(Color color) { sprite.setColor(color); }
	void setPosition(float x, float y) { position.x = x; position.y = y; sprite.setPosition(position); }
	void setPosition(Vector2f position) { this->position = position; sprite.setPosition(position); }
	void setScale(float scale) { sprite.setScale(scale, scale); }
	void setLifeStatus(bool status) { lifeStatus = status; }

	// Explosion & Death Related
	bool shouldDrawExplosion()
	{
		return isExploding && explosionClock.getElapsedTime().asSeconds() <= 1.0f;
	}
	void stopExplosion()
	{
		isExploding = false;
	}
	bool isExplosionOver()
	{
		return explosionClock.getElapsedTime().asSeconds() > 1.0f;
	}
	void destroyInvader(bool mute)
	{
		isExploding = true;
		explosionClock.restart();
		lifeStatus = false;
		music.play();
		music.setVolume(100 * mute);
	}


	// Draw
	void draw(RenderWindow& window) { window.draw(sprite); }

};



