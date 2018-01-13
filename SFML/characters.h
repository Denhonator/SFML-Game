#pragma once
#include <string.h>
#include <SFML/Graphics.hpp>

class characters {
public:
	std::string name;
	sf::Vector2f speed;
	std::string action;
	sf::RectangleShape rect;
	sf::Sprite sprite;
	int anim = 0;
	int animCounter = 0;
	characters();
	~characters();
};
