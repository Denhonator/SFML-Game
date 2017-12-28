#include <SFML/Graphics.hpp>
#include <ctime>
#include <iostream>
#include <Windows.h>
#include <string>

const int resx = 800;
const int resy = 600;
const int walls = 20;
sf::RectangleShape player(sf::Vector2f(120, 50));
sf::Sprite sprite;
std::string action = "walk";
int anim = 0;
int animCounter = 0;
sf::RectangleShape wall[walls];
sf::Vector2f speed(0, 0);
sf::FloatRect view(0, 0, 800, 600);
int state = 2;

void jump() {
	if (player.getPosition().y > 549) {
		speed.y = -15;
	}
}

void attack() {
	if (anim == 0) {
		anim = 1;
		animCounter = 1;
	}
}

void animate() {
	if (animCounter == 0 && (action!="walk"||abs(speed.x)+abs(speed.y)>1)) { //new frame unless standing still
		anim++;
		if (action == "walk") {
			animCounter = 10;
			if (anim > 2)
				anim = 1;
		}
		else if (action == "attack") {
			animCounter = anim;
			if (anim > 4) {
				anim = 1;
				action = "cooldown";
				animCounter = 8;
			}
		}
		else if (action == "cooldown") {
			action = "walk";
			anim = 1;
			animCounter = 1;
		}
	}
	if (animCounter > 0)
		animCounter--;
}

void input() {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && speed.x >-15)
	{
		speed.x--;
		if (state == 2) {
			sprite.setRotation(-90);
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && speed.x <15)
	{
		speed.x++;
		if (state == 2) {
			sprite.setRotation(90);
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		if(state==1)
			jump();
		if (state == 2) {
			speed.y--;
			sprite.setRotation(0);
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		if (state == 2) {
			speed.y++;
			sprite.setRotation(180);
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::X) && action=="walk") {
		action = "attack";
		animCounter = 1;
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
	if (player.getPosition().x > view.left + view.width)
		camera(window, 1);
	if (player.getPosition().x < view.left)
		camera(window, -1);
	if (player.getPosition().y > view.top + view.height)
		camera(window, 2);
	if (player.getPosition().y < view.top)
		camera(window, -2);
	window.clear();
	for (int i = 0; i < walls; i++) {
		window.draw(wall[i]);
	}
	sf::Texture character;
	if (!character.loadFromFile(action+std::to_string(anim)+".png"))
		{
			std::cout << "Can't load" + action + std::to_string(anim)+".png\n";
			character.loadFromFile("walk1.png"); //default
			anim = 1;
			animCounter = 5;
			action = "cooldown";
		}
	sprite.setTexture(character);
	sprite.setPosition(player.getPosition());
	sprite.move(player.getSize().x/2, player.getSize().y / 2); //correction
	window.draw(sprite);
	window.display();
}

void collision() {
	sf::Vector2f p = player.getPosition();						//predict
	sf::Vector2f ps = player.getSize();
	for (int i = 0; i < walls; i++) {
		sf::Vector2f w = wall[i].getPosition();
		sf::Vector2f ws = wall[i].getSize();
		if (p.y + ps.y + speed.y > w.y && p.y + ps.y + speed.y < w.y + ws.y && p.x<w.x+ws.x && p.x+ps.x>w.x) { //collision below
			player.setPosition(p.x, w.y - ps.y);
			speed.y = 0;
			std::cout << "Collision below\n";
		}
		if (p.y + speed.y<w.y + ws.y && p.y + speed.y > w.y && p.x<w.x + ws.x && p.x + ps.x>w.x) { //Collision above
			player.setPosition(p.x, w.y + ws.y);
			speed.y = 0;
			std::cout << "Collision above\n";
		}
		if (p.x + ps.x + speed.x > w.x && p.x + ps.x + speed.x < w.x + ws.x && p.y<w.y + ws.y && p.y + ps.y > w.y) { //collision on the right
			player.setPosition(w.x - ps.x, p.y);
			speed.x = 0;
			std::cout << "Collision on the right\n";
		}
		if (p.x + speed.x<w.x + ws.x && p.x + speed.x>w.x && p.y<w.y + ws.y && p.y + ps.y > w.y) { //Collision on the left
			player.setPosition(w.x + ws.x, p.y);
			speed.x = 0;
			std::cout << "Collision on the left\n";
		}
	}
}

void physics() {
	if (state == 1) {
		if (speed.y < 30)
			speed.y++;
		if (speed.x > 0)
			speed.x--;
		if (speed.x < 0)
			speed.x++;
	}
	if (state == 2) {
		if (speed.y > 4)
			speed.y--;
		if (speed.y > 0)
			speed.y--;
		if (speed.y < -4)
			speed.y++;
		if (speed.y < 0)
			speed.y++;
		if (speed.x > 4)
			speed.x--;
		if (speed.x > 0)
			speed.x--;
		if (speed.x < -4)
			speed.x++;
		if (speed.x < 0)
			speed.x++;
	}
}

void move(float m) {
	player.move(speed.x*m, speed.y*m);
}

void init() {
	sprite.setOrigin(32, 32);
	player.setSize(sf::Vector2f(20, 20));
	player.move(200, 100);
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