#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <time.h>

using namespace std;
using namespace sf;

class Shooter
{
private:
	float startingX;
	float startingY;
	Texture texture;
	Sprite sprite;
	Vector2f position;
	float speed;
	bool alive;
	const int totalLives;
	int lives;
	bool exploding = false;
	Clock explosionClock;

	const float explosionDuration = 1.0f;

	Texture normalTexture;
	Texture explosionTexture;

	Music music;

public:
	Shooter(float x = 0, float y = 0, const string& texturePath = "Sprites/player.png", float scale = 1.0f, int lives = 3) : alive(true), totalLives(lives)
	{
		music.openFromFile("Audio/ShooterDestroyed.wav");
		this->lives = totalLives;
		startingX = x;
		startingY = y;
		texture.loadFromFile(texturePath);
		sprite.setTexture(texture);
		sprite.setScale(scale, scale);
	}

	// Getters
	Sprite& getSprite() { return sprite; }
	Vector2f& getPosition() { return position; }
	bool& isAlive() { return alive; }
	int getLives() { return lives; }
	bool isExploding() { return (exploding && (explosionClock.getElapsedTime().asSeconds() <= 5)); }

	// Setters
	void setSpeed(float speed) { this->speed = speed; }
	void setPosition(float x, float y)
	{
		position.x = x;
		position.y = y;
		sprite.setPosition(position);
	}
	void setAlive(bool status) { alive = status; }
	void updatePosition(Vector2f position) { this->position = position; }
	void startExplosion(bool mute)
	{
		music.play();
		music.setVolume(100);
		alive = false;
		exploding = true;
		explosionClock.restart();
	}

	// Movement & Lives
	void move(float deltaTime, int dr)
	{
		position.x += (deltaTime * speed * dr);
	}
	void loadNextLife(Shooter* shooters)
	{
		if (lives != 0)
		{
			shooters[totalLives - lives].setAlive(false);
			lives--;
		}
	}

	//Draw 
	void draw(RenderWindow& window)
	{
		if (alive)
		{
			sprite.setPosition(position);
			window.draw(sprite);
		}
		else
		{
			if (isExploding())
			{
				Sprite& shooterSprite = Shooter::getSprite();

				Texture explosionTex;
				explosionTex.loadFromFile("Sprites/ShooterExplosion.png");
				Sprite sprite(explosionTex);
				sprite.setColor(Color::Cyan);
				sprite.setScale(shooterSprite.getScale());

				FloatRect shooterBounds = shooterSprite.getGlobalBounds();
				float shooterBottomY = shooterBounds.top + shooterBounds.height;

				FloatRect bounds = sprite.getGlobalBounds();

				float shooterCenterX = shooterBounds.left + shooterBounds.width / 2.f;
				float explosionX = shooterCenterX - bounds.width / 2.f;
				float explosionY = shooterBottomY - bounds.height;

				sprite.setPosition(explosionX, explosionY);

				window.draw(sprite);
			}
			else
			{
				exploding = false;
				alive = true;
			}
		}
	}
};
