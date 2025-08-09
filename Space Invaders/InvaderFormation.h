#pragma once
#include<SFML/Graphics.hpp>
#include<SFML/Window.hpp>
#include <iostream>
#include "Invaders.h"
#include "Bullet.h"

using namespace std;
using namespace sf;

class InvaderFormation
{
private:
	Invader** invaders;
	bool hitsBottom;
	float posX, posY;
	float initialX, initialY;
	float startX, startY;
	int gapX, gapY;
	const float moveStepX, moveStepY;
	float dr;
	float leftBoundary, rightBoundary;
	float bottomBoundary;
	const int ROWS, COLS;
	Texture invaderFrame1Textures[3];
	Texture invaderFrame2Textures[3];
	vector<Invader*> bottomInvaders;

public:
	InvaderFormation(const int screenWidth, const int screenHeight, const int rows, const int cols, float x, float y, float startX, float startY, const int moveX, const int moveY, int gapX = 100, int gapY = 55, int dr = 1) :
		ROWS(rows), COLS(cols), posX(x), posY(y), startX(startX), startY(startY), moveStepX(moveX), moveStepY(moveY), gapX(gapX), gapY(gapY), dr(dr)
	{
		hitsBottom = false;

		initialX = startX;
		initialY = startY;

		invaders = new Invader * [ROWS];
		for (int i = 0; i < ROWS; i++)
		{
			invaders[i] = new Invader[COLS];
		}

		leftBoundary = startX;
		rightBoundary = screenWidth - startX;

		bottomBoundary = screenHeight - startY;

		Color invaderColors[3] = { Color::Green, Color::Magenta, Color(255, 165, 0) };

		setFrame1Textures(invaderFrame1Textures);
		setFrame2Textures(invaderFrame2Textures);

		Sprite invaderSprites[3];
		bool changeFrame = false;

		for (int i = 0; i < 3; i++)
		{
			invaderSprites[i].setTexture(invaderFrame1Textures[i]);
			FloatRect bounds = invaderSprites[i].getLocalBounds();
			invaderSprites[i].setOrigin(bounds.width / 2.f, bounds.height / 2.f);
		}

		for (int i = 0; i < ROWS; i++)
		{
			int index = int((i + 1) / 2);

			for (int j = 0; j < cols; j++)
			{
				invaders[i][j].setSprite(invaderSprites[index]);
				invaders[i][j].setColor(invaderColors[index]);
				invaders[i][j].setPosition(startX + j * gapX, startY + i * gapY);
				invaders[i][j].setScale(2.5f);
			}
		}
	}

	void updateInvadersMovement(bool changeFrame)
	{
		if (posX > rightBoundary || startX < leftBoundary)
		{
			startY += moveStepY;
			dr = -dr;
		}

		for(int i = 0; i < ROWS; i++)
		{
			for(int j = 0; j < COLS; j++)
			{
				if (invaders[i][j].isAlive())
				{
					if (invaders[i][j].getPosition().y >= bottomBoundary)
					{
						if (!isFormationDestroyed())
						{
							hitsBottom = true;
							return;
						}
					}
				}

			}
		}

		startX += moveStepX * dr;

		for (int i = 0; i < ROWS; i++)
		{
			int index = int((i + 1) / 2);

			for (int j = 0; j < COLS; j++)
			{
				if (changeFrame)
				{
					invaders[i][j].getSprite().setTexture(invaderFrame2Textures[index]);
				}
				else
				{
					invaders[i][j].getSprite().setTexture(invaderFrame1Textures[index]);
				}
				posX = startX + j * gapX;
				posY = startY + i * gapY;
				invaders[i][j].setPosition(posX, posY);
			}
		}
	}

	void resetBottomInvaders()
	{
		bottomInvaders.clear();

		for (int col = 0; col < COLS; col++)
		{
			for (int row = ROWS - 1; row >= 0; row--)
			{
				if (invaders[row][col].isAlive())
				{
					bottomInvaders.push_back(&invaders[row][col]);
					break;
				}
			}
		}
	}

	bool hitsTheBottom()
	{
		return hitsBottom;
	}

	void drawFormation(RenderWindow& window)
	{
		for (int i = 0; i < ROWS; i++)
		{
			for (int j = 0; j < COLS; j++)
			{
				Invader& inv = invaders[i][j];

				if (inv.isAlive())
				{
					inv.draw(window); continue;
				}
				else if (inv.shouldDrawExplosion())
				{
					Texture explosionTex;
					explosionTex.loadFromFile("Sprites/Invaders/InvaderExplosion.png");
					Sprite explosionSprite(explosionTex);
					FloatRect bounds = explosionSprite.getLocalBounds();
					explosionSprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
					explosionSprite.setPosition(inv.getSprite().getPosition());
					explosionSprite.setColor(inv.getSprite().getColor());

					for (int i = 1; i <= 4; i++)
					{
						float growSize = -0.25f * (4 - i);
						if (inv.getExplosionElapsedTime() <= 0.25f * i)
						{
							explosionSprite.setScale(inv.getSprite().getScale().x + growSize, inv.getSprite().getScale().y + growSize);
							break;
						}
					}

					window.draw(explosionSprite);
				}
				else if (inv.isExplosionOver())
				{
					inv.stopExplosion();
				}
			}
		}
	}

	Invader* getRandomBottomInvader()
	{
		if (bottomInvaders.empty()) return nullptr;
		int index = rand() % bottomInvaders.size();
		return bottomInvaders[index];
	}

	Invader* operator[](int index) { return invaders[index]; }

	const int getRows() const { return ROWS; }
	const int getCols() const { return COLS; }

	bool isFormationDestroyed()
	{
		for (int i = 0; i < ROWS; i++)
		{
			for (int j = 0; j < COLS; j++)
			{
				if (invaders[i][j].isAlive()) return false;
			}
		}
		return true;
	}

	void reviveFormation()
	{
		hitsBottom = false;

		for (int i = 0; i < ROWS; i++)
		{
			for (int j = 0; j < COLS; j++)
			{
				invaders[i][j].setLifeStatus(true);
				invaders[i][j].setPosition(initialX + j * gapX, initialY + i * gapY);
				startX = initialX;
				startY = initialY;
				dr = 1;
			}
		}
	}

	int countRemaining() const
	{
		int cnt = 0;
		for (int i = 0; i < ROWS; ++i)
			for (int j = 0; j < COLS; ++j)
				if (invaders[i][j].isAlive()) ++cnt;
		return cnt;
	}

	int totalCount() const { return ROWS * COLS; }
};

