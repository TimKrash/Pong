#include "Button.h"

Button::Button(std::string text, RectangleShape posAndSize) :
	m_text(text),
	m_positionAndSize(posAndSize),
	m_colorUp(sf::Color::Red),
	m_colorDown(sf::Color::Black),
	m_colorHover(sf::Color::Blue),
	m_state(BUTTON_STATE::UP),
	m_callback([](){})
{}

/*
Function to set the position and size of the button
*/
void Button::setPositionAndSize(RectangleShape newPositionAndSize) {
	m_positionAndSize = newPositionAndSize;
}

/*
Function to only set the position of the button
*/
void Button::setPosition(Vector2D pos) {
	m_positionAndSize.x = pos.x;
	m_positionAndSize.y = pos.y;
}

/*
Function to only set the size of the button
*/
void Button::setSize(Vector2D size) {
	m_positionAndSize.width = size.x;
	m_positionAndSize.height = size.y;
}

/*
Function to get positionAndSize
*/
RectangleShape Button::getPositionAndSize() {
	return m_positionAndSize;
}

/*
Set button state
*/
void Button::setState(BUTTON_STATE newState) {
	this->m_state = newState;
}

/*
Function to get the state of button
*/
BUTTON_STATE Button::getState() {
	return m_state;
}

/*
Function to set the button colors
*/
void Button::setColors(sf::Color colorUp, sf::Color colorDown, sf::Color colorHover) {
	this->m_colorUp = colorUp;
	this->m_colorDown = colorDown;
	this->m_colorHover = colorHover;
}

/*
Function to set the function callback
*/
void Button::setCallback(CallbackFunc callback) {
	this->m_callback = callback;
}

/*
Function to handle any mouse input
*/
bool Button::handleInput(Vector2D& mouse_position, MOUSE_STATE& mouse_state) {
	// If the mouse is hovering over the button (within its dimensions), then the button state is hover
	if (mouse_position.x >= m_positionAndSize.x && mouse_position.x <= m_positionAndSize.x + m_positionAndSize.width
		&& mouse_position.y >= m_positionAndSize.y && mouse_position.y <= m_positionAndSize.y + m_positionAndSize.height) {
		m_state = BUTTON_STATE::HOVER;
	}
	else {
		m_state = BUTTON_STATE::UP;
	}

	if (m_state == BUTTON_STATE::HOVER && mouse_state == MOUSE_STATE::M_DOWN) {
		m_state = BUTTON_STATE::DOWN;
		m_callback();
		return true;
	}

	return false;
}

void Button::Render(sf::RenderTarget& target, sf::Font& font) {

	sf::Text buttonText(m_text, font, 60);
	buttonText.setFillColor(m_colorUp);
	if (m_state == BUTTON_STATE::DOWN) {
		buttonText.setFillColor(m_colorDown);
	}
	else if (m_state == BUTTON_STATE::HOVER) {
		buttonText.setFillColor(m_colorHover);
	}

	buttonText.setPosition({ m_positionAndSize.x, m_positionAndSize.y });

	sf::RectangleShape bg;
	bg.setPosition({ m_positionAndSize.x, m_positionAndSize.y });
	bg.setSize({ m_positionAndSize.width, m_positionAndSize.height });
	bg.setFillColor(sf::Color::White);

	target.draw(bg);
	target.draw(buttonText);
}