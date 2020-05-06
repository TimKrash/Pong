#pragma once
#ifndef BUTTON_h
#define BUTTON_h
#include <iostream>
#include <SFML/Graphics.hpp>
#include "shapes.h"
#include <string>
#include <functional>

typedef std::function<void(void)> CallbackFunc;

enum class BUTTON_STATE {
	UP,
	DOWN,
	HOVER
};

enum class MOUSE_STATE {
	M_UP,
	M_DOWN
};


class Button {

private:
	std::string m_text;
	RectangleShape m_positionAndSize;
	sf::Color m_colorUp;
	sf::Color m_colorDown;
	sf::Color m_colorHover;
	std::function<void()> m_callback;
	BUTTON_STATE m_state;

public:

	Button(std::string text, RectangleShape pos);
	/*
	Function to set the position and size of the button
	*/
	void setPositionAndSize(RectangleShape newPositionAndSize);

	/*
	Function to only set the position of the button
	*/
	void setPosition(Vector2D pos);

	/*
	Function to only set the size of the button
	*/
	void setSize(Vector2D size);

	/*
	Function to get positionAndSize
	*/
	RectangleShape getPositionAndSize();

	/*
	Set button state
	*/
	void setState(BUTTON_STATE newState);

	/*
	Function to get the state of button
	*/
	BUTTON_STATE getState();

	/*
	Function to handle the mouse input (clicks and movement)
	*/
	bool handleInput(Vector2D& mouse_position, MOUSE_STATE& mouse_state);

	void setColors(sf::Color colorUp, sf::Color colorDown, sf::Color colorHover);

	void setCallback(const CallbackFunc c);

	void Render(sf::RenderTarget& target, sf::Font& font);

};

#endif // !BUTTON_h

