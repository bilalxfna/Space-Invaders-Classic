#pragma once
#include<SFML/Graphics.hpp>
#include<SFML/Window.hpp>
#include <iostream>
#include <vector>
#include "Bullet.h"
#include "Shooter.h"

using namespace std;
using namespace sf;

class Pixel
{
private:
	bool pixelActive;
	Color color;

public:
	Pixel()
	{
		pixelActive = false;
		color = Color::Transparent;
	}

	void operator=(bool status)
	{
		pixelActive = status;
		color = status ? Color::Yellow : Color::Transparent;
	}

	bool isActive() const { return pixelActive; }

	Color getColor() const { return color; }

	void setColor(Color c) { color = c; }

	void activate(Color c = Color::Yellow) {
		pixelActive = true;
		color = c;
	}

	void deactivate() {
		pixelActive = false;
		color = Color::Transparent;
	}
};


class Shield
{
private:
	int rows = 9;
	int cols = 13;
	float pixelSize;
	vector<vector<Pixel>> grid;

public:
	vector<vector<bool>> shieldPattern = {
		{0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
		{0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
		{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1},
		{1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1}
	};

	Shield(float pixelSize = 8.0f) : pixelSize(pixelSize)
	{
		grid = vector<vector<Pixel>>(rows, vector<Pixel>(cols));

		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < cols; j++)
			{
				grid[i][j] = shieldPattern[i][j];
			}
		}
	}
	int getRows() { return rows; }
	int getCols() { return cols; }

	vector<Pixel>& operator[](int index) { return grid[index]; }

	void draw(RenderWindow& window, float startX, float startY)
	{
		for (int i = 0; i < rows; ++i)
		{
			for (int j = 0; j < cols; ++j)
			{
				if (grid[i][j].isActive())
				{
					RectangleShape pixel(Vector2f(pixelSize, pixelSize));
					pixel.setFillColor(grid[i][j].getColor());
					pixel.setPosition(startX + j * pixelSize, startY + i * pixelSize);
					window.draw(pixel);
				}
			}
		}
	}

	float getPixelSize() { return pixelSize; }

	void fill()
	{
		// clear first (so repeated calls don't accumulate weird state)
		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
				grid[i][j].deactivate();

		// activate according to pattern, guarding sizes
		for (int i = 0; i < rows; i++)
		{
			if (i >= (int)shieldPattern.size()) continue;
			for (int j = 0; j < cols; j++)
			{
				if (j >= (int)shieldPattern[i].size()) continue;
				if (shieldPattern[i][j])
					grid[i][j].activate();
			}
		}
	}

};