#pragma once
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

class UFO {
private:
    Sprite sprite;
    Texture texture;
    bool alive = false;

    float speed;
    float startX, endX;
    float y;
    int dr = 1;

    float interval;       // seconds between spawns
    float duration;       // time to cross screen
    Clock spawnClock;
    Clock moveClock;
    Clock scoreClock;
    int score = 0;
    bool showingScore = false;

    Music music;

public:
    UFO(const string& texturePath, float scale, float screenWidth, float startY, float ufoInterval = 25.6f, float ufoDuration = 7.0f)
        : interval(ufoInterval), duration(ufoDuration), y(startY)
    {
        music.openFromFile("Audio/UFO.wav");
        music.setVolume(100);
        texture.loadFromFile(texturePath);
        sprite.setTexture(texture);
        sprite.setScale(scale, scale);
        startX = screenWidth + sprite.getGlobalBounds().width;
        endX = -sprite.getGlobalBounds().width;
        speed = (endX - startX) / duration;
        sprite.setPosition(startX, y);
    }

    void update()
    {
        if (!alive && spawnClock.getElapsedTime().asSeconds() > interval)
        {
            reset();
        }

        if (alive)
        {
            if (music.getStatus() == !Music::Playing)
            {
                music.play();
            }
            float deltaTime = moveClock.restart().asSeconds();
            sprite.move(speed * deltaTime * dr, 0);

            if ((sprite.getPosition().x < endX && dr == 1) || (sprite.getPosition().x > endX && dr == -1))
            {
                alive = false;
                spawnClock.restart();

                swap(startX, endX);
                dr = -dr;
            }
        }
        else
        {
            music.stop();
        }
    }

    void reset()
    {
        sprite.setPosition(startX, y);
        alive = true;
        moveClock.restart();
    }

    void setColor(Color c) { sprite.setColor(c); }
    void setAlive(bool status) { alive = status; }
    bool isAlive() const { return alive; }
    void draw(RenderWindow& window, Font font)
    {
        if (alive) window.draw(sprite);
        else if (!alive && scoreClock.getElapsedTime().asSeconds() <= 0.5f)
        {
            Text text(to_string(score), font, 30);
            text.setPosition(sprite.getPosition());

            window.draw(text);
        }
        else
        {
            sprite.setPosition(startX, y);
        }
    }
    Sprite& getSprite() { return sprite; }
    Vector2f getPosition() const { return sprite.getPosition(); }
    void setPosition(float x, float y) { sprite.setPosition(x, y); }
    void destroy() { alive = false; spawnClock.restart(); }
    void scoreToShow(int score) { this->score = score; }
    void restartScoreClock() { scoreClock.restart(); }

    void pauseUFO()
    {
        moveClock.restart();
    }

    void restartSpawnClock()
    {
        spawnClock.restart();
    }
};
