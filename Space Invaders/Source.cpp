#include<SFML/Graphics.hpp>
#include<SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include "Shooter.h"
#include "Bullet.h"
#include "Shield.h"
#include "InvaderFormation.h"
#include "UFO.h"

using namespace std;
using namespace sf;

const int screenWidth = 1280;
const int screenHeight = 900;

void resetInvaderBulletToRandomShooter(InvaderFormation&, Bullet&, Clock&, char&);
void handleScoreByUFO(Bullet&, UFO&, int&);
void drawGameOverScreen(RenderWindow&, Font&, bool, int, int, bool);
void drawMainScreen(RenderWindow&, Font);
void drawPauseGameScreen(RenderWindow&, Font);

int main()
{
	srand(time(0));

	// ----- BACKGROUND ----- //

	Texture backTex;
	backTex.loadFromFile("Sprites/background.png");
	Sprite backSprite(backTex);
	backSprite.setPosition(0, 0);
	backSprite.setScale(1.0f, 1.0f);
	RenderWindow window(VideoMode(screenWidth, screenHeight), "Space Invaders", Style::Close);

	// ----- SHOOTER ----- //

	Shooter shooter;
	float shooterY = screenHeight - 140;
	shooter.setPosition(100, shooterY);
	shooter.setSpeed(300.0f);
	int totalLives = 3;
	int lives = totalLives;
	bool extraLife = true;

	Clock shooterClock;

	// ----- BULLET ----- //

	Bullet bullet;
	bullet.setToCenterOfShooter(shooter);
	bullet.setSpeed(800.0f);
	bullet.setColor(shooter.getSprite().getColor());

	Clock bulletClock;

	// ----- SHIELD ----- //

	Shield shield[4];
	int shieldStartX = 225, shieldGap = 250;

	float shieldX[4], shieldY = shooterY - 120;

	for (int i = 0; i < 4; i++) shieldX[i] = shieldStartX + i * shieldGap;

	// ----- INVADERS ----- //

	bool changeFrame = false;
	const float startX = 150;
	const float startY = 250;

	InvaderFormation invaderFormation(screenWidth, screenHeight, 5, 11, startX, startY, startX, startY, 10, 10, 80, 50);
	invaderFormation.resetBottomInvaders();
	int callOnce = true;

	float moveTimer = 0;
	clock_t invaderClock = clock();
	float delay = 0.5f;

	Bullet invaderBullet(2.5, "Sprites/Projectiles/ProjectileA.png");
	bool canInvaderShoot = true;

	char bulletChar = 'S';

	Clock invaderShootClock;
	float invaderShootDelay = 1.5f;

	float invaderMoveTimer = 0.0f;
	float baseInvaderDelay = 0.5f;
	const float minInvaderDelay = 0.1f;

	float invaderShootTimer = 0.0f;
	float baseInvaderShootDelay = 2.0f;
	const float minInvaderShootDelay = 0.5f;

	int level = 1;

	// ----- UFO ----- //

	UFO ufo("Sprites/Invaders/UFO.png", 1.5f, screenWidth, startY - 100);
	ufo.setColor(Color::Red);
	int ufoScore = 0;

	// ----- FONT ----- // 

	Font font;
	font.loadFromFile("SpaceInvadersText.ttf");

	// ----- GAME OVER & PAUSE GAME ----- //

	bool gameOver = false;
	bool showMainScreen = true;
	Clock gameOverClock;
	Music gameOverMusic;
	gameOverMusic.openFromFile("Audio/GameOver.wav");
	bool playGameOverMusic = true;

	bool pauseGame = false;
	bool lastPauseEnterFrame = false;

	// ----- HIGHSCORE & SCORE ----- //

	bool muteVolume = 1;

	int score = 0;
	int highscore = 0;
	const string highscoreFile = "highscore.txt";

	ifstream infile(highscoreFile);
	if (infile.is_open()) {
		infile >> highscore;
		infile.close();
	}

	int prevHighscore = highscore;
	Music beatScoreMusic;
	beatScoreMusic.openFromFile("Audio/BeatHighscore.mp3");
	bool playBeatMusic = true;

	// ----- BORDER ----- //

	RectangleShape border;
	border.setOutlineColor(Color::White);
	border.setOutlineThickness(2.f);
	border.setSize(Vector2f(screenWidth + 1, shooterY - 60));
	border.setTexture(&backTex);
	border.setPosition(-1, 120);

	float topBorderY = border.getPosition().y - border.getOutlineThickness() / 2.f;
	float bottomBorderY = border.getPosition().y + border.getSize().y + border.getOutlineThickness() / 2.f;

	// ----- TITLE ----- //

	Texture TitleTex;
	TitleTex.loadFromFile("Sprites/SpaceInvadersTitle.png");

	Sprite TitleSprite(TitleTex);

	FloatRect Tbounds = TitleSprite.getGlobalBounds();

	TitleSprite.setOrigin(Tbounds.left + Tbounds.width / 2.f, Tbounds.top + Tbounds.height / 2.f);
	TitleSprite.setPosition(screenWidth / 2.f, 50);
	TitleSprite.setScale(0.15f, 0.15f);

	// ------------------ //

	while (window.isOpen())
	{
		window.clear();
		showMainScreen ? window.draw(backSprite) : window.draw(border);

		float deltaTime = shooterClock.restart().asSeconds();
		if (shooter.isExploding()) deltaTime = 0.0f;

		Event ev;
		while (window.pollEvent(ev))
		{
			if (ev.type == Event::Closed)
			{
				window.close();
			}
			if (ev.type == Event::KeyPressed)
			{
				if (ev.key.code == Keyboard::M)
				{
					muteVolume = !muteVolume;
				}
				if (ev.key.code == Keyboard::Escape)
				{
					window.close();
				}
				if (showMainScreen)
				{
					if (ev.key.code == Keyboard::Enter)
					{
						score = 0;
						lives = totalLives;
						gameOver = false;
						showMainScreen = false;
						playGameOverMusic = true;
						playBeatMusic = true;
						prevHighscore = highscore;
						ufo.restartSpawnClock();

						shooter.setPosition(100, shooterY);
						shooter.setSpeed(300.0f);
						bullet.setToCenterOfShooter(shooter);
						bullet.setFireStatus(false);
						invaderFormation.reviveFormation();
						invaderFormation.resetBottomInvaders();
						invaderBullet.setFireStatus(false);

						for (int i = 0; i < 4; i++) shield[i].fill();

						lastPauseEnterFrame = true;
					}
				}
			}
		}

		if (showMainScreen)
		{
			drawMainScreen(window, font);

			window.display();
			continue;
		}

		if (Keyboard::isKeyPressed(Keyboard::Enter) && !gameOver && !showMainScreen && !lastPauseEnterFrame)
		{
			pauseGame = !pauseGame;
			lastPauseEnterFrame = true;
		}
		else if (!Keyboard::isKeyPressed(Keyboard::Enter)) lastPauseEnterFrame = false;

		// Only enalble controls and movement if game is being played and shooter is not exploding
		if (!shooter.isExploding() && !gameOver && !showMainScreen && !pauseGame)
		{
			if (Keyboard::isKeyPressed(Keyboard::Right) && shooter.getPosition().x < screenWidth - 200)
				shooter.move(deltaTime, 1);

			if (Keyboard::isKeyPressed(Keyboard::Left) && shooter.getPosition().x > 100)
				shooter.move(deltaTime, -1);

			if (Keyboard::isKeyPressed(Keyboard::Space) && !bullet.getFireStatus())
			{
				bullet.increaseShotCount();
				bullet.getFireStatus() = true;
				bullet.playFireMusic(muteVolume);
			}

			if (invaderFormation.isFormationDestroyed())
			{
				baseInvaderDelay += 0.1f;
				baseInvaderShootDelay -= 0.05;
				invaderFormation.reviveFormation();
				invaderFormation.resetBottomInvaders();
			}
			else if (invaderFormation.hitsTheBottom() && !gameOver)
			{
				gameOverClock.restart();
				gameOver = true;
			}

			if (bullet.getFireStatus()) bullet.fire(deltaTime, topBorderY);

			if (!bullet.getFireStatus())
			{
				bullet.setToCenterOfShooter(shooter);
			}

			for (int i = 0; i < 4; i++)
			{
				if (bullet.hitShield(shield[i], shieldX[i], shieldY))
				{
					bullet.setToCenterOfShooter(shooter);
					bullet.setFireStatus(false); break;
				}
			}

			// Increase invaders movement speed with time 
			int remaining = invaderFormation.countRemaining();
			int total = invaderFormation.totalCount();
			float moveDelay = baseInvaderDelay * ((float)remaining / total);
			if (moveDelay < minInvaderDelay) moveDelay = minInvaderDelay;

			invaderMoveTimer += deltaTime;
			if (invaderMoveTimer >= moveDelay)
			{
				changeFrame = !changeFrame;
				invaderFormation.updateInvadersMovement(changeFrame);
				invaderMoveTimer = 0.0f;

				invaderFormation.resetBottomInvaders();
			}

			// Increase shooting frequency with time
			float shootDelay = baseInvaderShootDelay * ((float)remaining / total);
			if (shootDelay < minInvaderShootDelay) shootDelay = minInvaderShootDelay;

			invaderShootTimer += deltaTime;
			if (invaderShootTimer >= shootDelay && !invaderBullet.getFireStatus())
			{
				resetInvaderBulletToRandomShooter(invaderFormation, invaderBullet, invaderShootClock, bulletChar);
				invaderBullet.setFireStatus(true);
				invaderShootTimer = 0.0f;
			}

			if (bullet.hitUFO(ufo, score))
			{
				bullet.setToCenterOfShooter(shooter);
				bullet.setFireStatus(false);

				handleScoreByUFO(bullet, ufo, score);
			}

			if (invaderBullet.getFireStatus())
			{
				invaderBullet.fire(deltaTime, bottomBorderY, bulletChar);
			}

			for (int i = 0; i < 4; i++)
			{
				if (invaderBullet.hitShield(shield[i], shieldX[i], shieldY))
				{
					resetInvaderBulletToRandomShooter(invaderFormation, invaderBullet, invaderShootClock, bulletChar);
					break;
				}
			}

			if (bullet.hitInvader(invaderFormation, score, muteVolume))
			{
				invaderFormation.resetBottomInvaders();
				bullet.setToCenterOfShooter(shooter);
				bullet.setFireStatus(false);
			}

			if (invaderBullet.hitShooter(shooter, muteVolume))
			{
				bullet.setToCenterOfShooter(shooter);
				resetInvaderBulletToRandomShooter(invaderFormation, invaderBullet, invaderShootClock, bulletChar);

				if (lives > 0) lives--;
				if (lives == 0) gameOver = true;
				else invaderBullet.setFireStatus(false);
			}

			if (bullet.hitBullet(invaderBullet))
			{
				resetInvaderBulletToRandomShooter(invaderFormation, invaderBullet, invaderShootClock, bulletChar);
			}

			bullet.draw(window);

			ufo.update();
		}
		else
		{
			ufo.pauseUFO();
		}

		if (score > highscore) highscore = score;
		if (score >= 1500 && extraLife)
		{
			lives++;
			extraLife = false;
		}

		window.draw(TitleSprite);

		Text scoreText("SCORE : " + to_string(score), font, 35);
		scoreText.setPosition(startX - 20, 45);
		window.draw(scoreText);

		Text highText("HIGHSCORE : " + to_string(highscore), font, 35);
		highText.setPosition(screenWidth - 3 * startX, 45);
		window.draw(highText);

		Text pauseText("Press Enter\nTo Pause", font, 15);
		pauseText.setPosition(screenWidth - 200, screenHeight - 55);
		window.draw(pauseText);

		Text livesText("LIVES", font, 30);
		livesText.setPosition(100, screenHeight - 55);
		window.draw(livesText);
		Sprite lifeIcon = shooter.getSprite();
		lifeIcon.setScale(1.0f, 1.0f);

		for (int i = 0; i < lives; ++i)
		{
			lifeIcon.setPosition((225 + i * 75), screenHeight - 50);
			window.draw(lifeIcon);
		}

		for (int i = 0; i < 4; i++)
		{
			shield[i].draw(window, shieldX[i], shieldY);
		}
		invaderBullet.draw(window);

		invaderFormation.drawFormation(window);

		ufo.draw(window, font);

		if (lives || shooter.isExploding()) shooter.draw(window);

		if (pauseGame)
		{
			drawPauseGameScreen(window, font);
		}

		if (gameOver && shooter.isExploding()) gameOverClock.restart();

		if (gameOver && (!shooter.isExploding() || invaderFormation.hitsTheBottom()) && !showMainScreen)
		{
			if (playGameOverMusic)
			{
				gameOverMusic.play();
				gameOverMusic.setVolume(100 * muteVolume);
				playGameOverMusic = false;
			}

			RectangleShape overlay(Vector2f((float)screenWidth, (float)screenHeight));
			overlay.setPosition(0.f, 0.f);
			overlay.setFillColor(Color(0, 0, 0, 200));
			window.draw(overlay);

			bool beatHighscore = score > prevHighscore;
			bool showMessage = false;

			if (gameOverClock.getElapsedTime().asSeconds() <= 8.0f)
			{
				if (beatHighscore)
				{
					ofstream out("highscore.txt");
					if (out.is_open()) out << highscore;
				}

				if (gameOverClock.getElapsedTime().asSeconds() > 3.0f) showMessage = true;

				drawGameOverScreen(window, font, beatHighscore, prevHighscore, score, showMessage);

				if (showMessage && playBeatMusic && beatHighscore)
				{
					beatScoreMusic.play();
					beatScoreMusic.setVolume(100 * muteVolume);
					playBeatMusic = false;
				}
			}
			else
			{
				showMainScreen = true;
				gameOver = false;
			}
		}
		window.display();
	}
}

void resetInvaderBulletToRandomShooter(InvaderFormation& formation, Bullet& bullet, Clock& clock, char& c)
{
	Invader* shootingInvader = formation.getRandomBottomInvader();
	bullet.setupFromInvader(shootingInvader, 300.0f);
	if (shootingInvader->getColor() == Color::Green) c = 'A';
	if (shootingInvader->getColor() == Color::Magenta) c = 'B';
	if (shootingInvader->getColor() == Color(255, 165, 0)) c = 'C';
	bullet.setFireStatus(false);
}

void handleScoreByUFO(Bullet& bullet, UFO& ufo, int& score)
{
	int ufoScore = 0;

	int shotCount = bullet.getShotCount();
	if (shotCount == 23 || (shotCount > 23 && (shotCount - 23) % 15 == 0))
	{
		ufoScore = 300;
	}
	else
	{
		int fallback[3] = { 50, 100, 150 };
		ufoScore = fallback[(shotCount / 5) % 3];
	}
	score += ufoScore;

	ufo.scoreToShow(ufoScore);
	ufo.restartScoreClock();
}

void drawGameOverScreen(RenderWindow& window, Font& font, bool beatHighscore, int previousHighscore, int playerScore, bool showMessage)
{
	if (!showMessage)
	{
		// --- Game Over Message --- 
		Text gameOverText("GAME OVER", font, 80);
		gameOverText.setStyle(Text::Bold);
		gameOverText.setFillColor(Color::Red);
		FloatRect textRect = gameOverText.getLocalBounds();
		gameOverText.setOrigin(textRect.left + textRect.width / 2.f, textRect.top + textRect.height / 2.f);
		gameOverText.setPosition(screenWidth / 2.f, screenHeight / 2.f);
		window.draw(gameOverText);
		return;
	}

	// --- Main Message ---
	string message = beatHighscore
		? "YOU BEAT THE HIGHSCORE!"
		: "OOPS! YOU DID NOT BEAT THE HIGHSCORE";

	Text mainText(message, font, 45);
	mainText.setFillColor(beatHighscore ? Color::Green : Color::Red);
	FloatRect mainBounds = mainText.getLocalBounds();
	mainText.setOrigin(mainBounds.left + mainBounds.width / 2.f, mainBounds.top + mainBounds.height / 2.f);
	mainText.setPosition(screenWidth / 2.f, screenHeight / 2.f - 100);
	window.draw(mainText);

	// --- Previous Highscore ---
	Text highscoreText("Previous Highscore: " + to_string(previousHighscore), font, 30);
	highscoreText.setFillColor(Color::White);
	FloatRect highBounds = highscoreText.getLocalBounds();
	highscoreText.setOrigin(highBounds.left + highBounds.width / 2.f, highBounds.top + highBounds.height / 2.f);
	highscoreText.setPosition(screenWidth / 2.f, screenHeight / 2.f);
	window.draw(highscoreText);

	// --- Player's Score ---
	Text playerScoreText("Your Score: " + to_string(playerScore), font, 30);
	playerScoreText.setFillColor(Color::Yellow);
	FloatRect playerBounds = playerScoreText.getLocalBounds();
	playerScoreText.setOrigin(playerBounds.left + playerBounds.width / 2.f, playerBounds.top + playerBounds.height / 2.f);
	playerScoreText.setPosition(screenWidth / 2.f, screenHeight / 2.f + 70);
	window.draw(playerScoreText);
}

void drawMainScreen(RenderWindow& window, Font font)
{
	Texture titleTex;
	titleTex.loadFromFile("Sprites/SpaceInvadersTitle.png");

	Sprite titleSprite(titleTex);

	FloatRect bounds = titleSprite.getGlobalBounds();

	titleSprite.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
	titleSprite.setPosition(screenWidth / 2.f, 200);
	titleSprite.setScale(0.45f, 0.45f);

	window.draw(titleSprite);

	Text enterText("Press ENTER to start game", font, 30);

	bounds = enterText.getGlobalBounds();

	enterText.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
	enterText.setPosition(screenWidth / 2.f, screenHeight - 150);
	enterText.setFillColor(Color::Yellow);

	window.draw(enterText);

	Text exitText("Press ESC to exit game", font, 20);

	bounds = exitText.getGlobalBounds();

	exitText.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
	exitText.setPosition(screenWidth / 2.f, screenHeight - 100);
	exitText.setFillColor(Color::Red);

	window.draw(exitText);

	int pointsX = 500;
	int pointsY = 400;

	string paths[4] = { "Octopus_1", "Crab_1", "Squid_1", "UFO" };
	Color  colors[4] = { Color(255, 165, 0), Color::Magenta, Color::Green, Color::Red };
	Texture textures[4];
	Sprite sprites[4];

	for (int i = 0; i < 4; i++)
	{
		textures[i].loadFromFile("Sprites/Invaders/" + paths[i] + ".png");
		sprites[i].setTexture(textures[i]);
		sprites[i].setColor(colors[i]);
		sprites[i].setPosition(pointsX, pointsY + i * 70);
		(i == 3) ? sprites[i].setScale(1.5f, 1.5f) : sprites[i].setScale(2.5f, 2.5f);
		window.draw(sprites[i]);
	}

	string points[4] = { "10", "20", "30", "???" };
	Text pointsText[4];

	for (int i = 0; i < 4; i++)
	{
		pointsText[i].setFont(font);
		pointsText[i].setCharacterSize(25);
		pointsText[i].setString(" - \t" + points[i] + " Points");
		pointsText[i].setPosition(pointsX + 100, pointsY + i * 70);
		window.draw(pointsText[i]);
	}

}
void drawPauseGameScreen(RenderWindow& window, Font font)
{
	int screenY = 180;

	RectangleShape overlay(Vector2f((float)screenWidth, (float)screenHeight));
	overlay.setPosition(0.f, 0.f);
	overlay.setFillColor(Color(0, 0, 0, 200));
	window.draw(overlay);

	Text pauseText("GAME PAUSED", font, 60);
	pauseText.setFillColor(Color::Green);
	FloatRect textRect = pauseText.getLocalBounds();
	pauseText.setOrigin(textRect.left + textRect.width / 2.f, textRect.top + textRect.height / 2.f);
	pauseText.setPosition(screenWidth / 2.f, screenY);
	window.draw(pauseText);

	Text controlsHeading("CONTROLS & RULES", font, 35);
	controlsHeading.setFillColor(Color::Yellow);
	textRect = controlsHeading.getLocalBounds();
	controlsHeading.setOrigin(textRect.left + textRect.width / 2.f, textRect.top + textRect.height / 2.f);
	controlsHeading.setPosition(screenWidth / 2.f, screenY + 100);
	window.draw(controlsHeading);

	Text controls("", font, 25);
	controls.setFillColor(Color::White);
	controls.setString(
		"\n      Move Shooter"
		"\n      Fire Bullet"
		"\n      Mute / Unmute"
		"\n      Pause / Resume"
		"\n      Quit Game"
		"\n"
		"\n    - Destroy invaders to gain points."
		"\n    - Hitting UFO gives bonus points (???)."
		"\n    - Shields absorb some bullets but degrade over time."
		"\n    - At 1500 Points: You get an Extra Life Only Once."
		"\n    - Avoid getting hit by enemy bullets or invaders!"
		"\n"
		"\n Tip: The fewer invaders remain, the faster they shoot and move!"
	);
	textRect = controls.getLocalBounds();
	controls.setOrigin(textRect.left + textRect.width / 2.f, textRect.top);
	controls.setPosition(screenWidth / 2.f, screenY + 160);
	window.draw(controls);

	Text controlsButtons("", font, 25);
	controlsButtons.setFillColor(Color::White);
	controlsButtons.setString(
		"\n    : Left / Right Arrow Keys"
		"\n    : Spacebar"
		"\n    : M Key"
		"\n    : Enter Key"
		"\n    : ESC Key"
	);

	textRect = controlsButtons.getLocalBounds();
	controlsButtons.setOrigin(textRect.left + textRect.width / 2.f, textRect.top);
	controlsButtons.setPosition(screenWidth / 2.f + 50, screenY + 160);
	window.draw(controlsButtons);

}


