#include <SFML/Graphics.hpp>
#include <ctime>
#include <iostream>
#include <string>
#include "characters.h"

const int resx = 1600;
const int resy = 1200;
const int walls = 20;
sf::RectangleShape wall[walls];
sf::FloatRect view(0, 0, 800, 600);
int state = 2;
characters hero;

void jump() {
	if (hero.rect.getPosition().y > 549) {
		hero.speed.y = -15;
	}
}

void attack() {
	if (hero.anim == 0) {
		hero.anim = 1;
		hero.animCounter = 1;
	}
}

void animate() {
	if (hero.animCounter == 0 && (hero.action!="walk"||abs(hero.speed.x)+abs(hero.speed.y)>1)) { //new frame unless standing still
		hero.anim++;
		if (hero.action == "walk") {
			hero.animCounter = 10;
			if (hero.anim > 2)
				hero.anim = 1;
		}
		else if (hero.action == "attack") {
			hero.animCounter = hero.anim;
			if (hero.anim > 4) {
				hero.anim = 1;
				hero.action = "cooldown";
				hero.animCounter = 8;
			}
		}
		else if (hero.action == "cooldown") {
			hero.action = "walk";
			hero.anim = 1;
			hero.animCounter = 1;
		}
	}
	if (hero.animCounter > 0)
		hero.animCounter--;
}

void input() {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && hero.speed.x >-15)
	{
		hero.speed.x--;
		if (state == 2) {
			hero.sprite.setRotation(-90);
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && hero.speed.x <15)
	{
		hero.speed.x++;
		if (state == 2) {
			hero.sprite.setRotation(90);
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		if(state==1)
			jump();
		if (state == 2) {
			hero.speed.y--;
			hero.sprite.setRotation(0);
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		if (state == 2) {
			hero.speed.y++;
			hero.sprite.setRotation(180);
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::X) && hero.action=="walk") {
		hero.action = "attack";
		hero.animCounter = 1;
	}
}

void camera(sf::RenderWindow& window, int move = 0) {
	if (move == 1 || move == -1) {
		view.left += 800 * move;
		std::cout << view.left << std::endl;
	}
	if (move == 2 || move == -2) {
		view.top += 300 * move;
		std::cout << view.top << std::endl;
	}
	sf::View view1(view);
	window.setView(view1);
}

void draw(sf::RenderWindow& window) {
	if (hero.rect.getPosition().x > view.left + view.width)
		camera(window, 1);
	if (hero.rect.getPosition().x < view.left)
		camera(window, -1);
	if (hero.rect.getPosition().y > view.top + view.height)
		camera(window, 2);
	if (hero.rect.getPosition().y < view.top)
		camera(window, -2);
	window.clear();
	for (int i = 0; i < walls; i++) {
		window.draw(wall[i]);
	}
	sf::Texture character;
	if (!character.loadFromFile(hero.action+std::to_string(hero.anim)+".png"))
		{
			std::cout << "Can't load" + hero.action + std::to_string(hero.anim)+".png\n";
			character.loadFromFile("walk1.png"); //default
			hero.anim = 1;
			hero.animCounter = 5;
			hero.action = "cooldown";
		}
	hero.sprite.setTexture(character);
	hero.sprite.setPosition(hero.rect.getPosition());
	hero.sprite.move(hero.rect.getSize().x/2, hero.rect.getSize().y / 2); //correction
	window.draw(hero.sprite);
	window.display();
}

void collision() {
	sf::Vector2f p = hero.rect.getPosition();						//predict
	sf::Vector2f ps = hero.rect.getSize();
	for (int i = 0; i < walls; i++) {
		sf::Vector2f w = wall[i].getPosition();
		sf::Vector2f ws = wall[i].getSize();
		if (p.y + ps.y + hero.speed.y > w.y && p.y + ps.y + hero.speed.y < w.y + ws.y && p.x<w.x+ws.x && p.x+ps.x>w.x) { //collision below
			hero.rect.setPosition(p.x, w.y - ps.y);
			hero.speed.y = 0;
			std::cout << "Collision below\n";
		}
		if (p.y + hero.speed.y<w.y + ws.y && p.y + hero.speed.y > w.y && p.x<w.x + ws.x && p.x + ps.x>w.x) { //Collision above
			hero.rect.setPosition(p.x, w.y + ws.y);
			hero.speed.y = 0;
			std::cout << "Collision above\n";
		}
		if (p.x + ps.x + hero.speed.x > w.x && p.x + ps.x + hero.speed.x < w.x + ws.x && p.y<w.y + ws.y && p.y + ps.y > w.y) { //collision on the right
			hero.rect.setPosition(w.x - ps.x, p.y);
			hero.speed.x = 0;
			std::cout << "Collision on the right\n";
		}
		if (p.x + hero.speed.x<w.x + ws.x && p.x + hero.speed.x>w.x && p.y<w.y + ws.y && p.y + ps.y > w.y) { //Collision on the left
			hero.rect.setPosition(w.x + ws.x, p.y);
			hero.speed.x = 0;
			std::cout << "Collision on the left\n";
		}
	}
}

void physics() {
	if (state == 1) {
		if (hero.speed.y < 30)
			hero.speed.y++;
		if (hero.speed.x > 0)
			hero.speed.x--;
		if (hero.speed.x < 0)
			hero.speed.x++;
	}
	if (state == 2) {
		if (hero.speed.y > 4)
			hero.speed.y--;
		if (hero.speed.y > 0)
			hero.speed.y--;
		if (hero.speed.y < -4)
			hero.speed.y++;
		if (hero.speed.y < 0)
			hero.speed.y++;
		if (hero.speed.x > 4)
			hero.speed.x--;
		if (hero.speed.x > 0)
			hero.speed.x--;
		if (hero.speed.x < -4)
			hero.speed.x++;
		if (hero.speed.x < 0)
			hero.speed.x++;
	}
}

void move(float m) {
	hero.rect.move(hero.speed.x*m, hero.speed.y*m);
}

void init() {
	hero.anim = 0;
	hero.animCounter = 0;
	hero.speed = sf::Vector2f(0, 0);
	hero.action = "walk";
	hero.sprite.setOrigin(32, 32);
	hero.rect = sf::RectangleShape(sf::Vector2f(120, 50));
	hero.rect.setSize(sf::Vector2f(20, 20));
	hero.rect.move(200, 100);
	wall[0].setSize(sf::Vector2f(2000, 10));
	wall[0].setPosition(sf::Vector2f(-100, 580));
	wall[1].setSize(sf::Vector2f(30, 20));
	wall[1].setPosition(sf::Vector2f(400, 300));
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(resx, resy), "SFML works!");
	init();
	std::clock_t time;
	time = clock();
	int loop = 0;
	bool skip = 0;
	int gameSpeed = 240;
	float speedMultiplier = 0.3;
	int fpsDivider = 1;
	int physicsDivider = 12;
	camera(window);

	while (window.isOpen())
	{
		time = clock();
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		if(loop%(physicsDivider/2)==0)
			input();
		if(loop%physicsDivider==0)
			physics();
		if (loop % (physicsDivider / 4) == 0) {
			animate();
		}
		if (loop%fpsDivider==0) {
			draw(window);
		}
		collision();
		move(speedMultiplier);
		loop++;
		if (loop > 1000)
			loop = 0;
		skip = 1;
		while (clock() - time < 1000 / gameSpeed) {
			skip = 0;
		}
	}
	return 0;
}