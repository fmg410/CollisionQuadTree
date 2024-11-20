#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "structures/QuadNode.hpp"
#include <array>
#include <iostream>
#include "structures/QuadTree.hpp"
#include <cmath>
#include <chrono>
#include "collision/Figure.hpp"
#include "collision/DrawableFigure.hpp"
#include "collision/Collision.hpp"
#include "utils/RainbowGenerator.hpp"
#include <random>
#include <functional>

//#define NO_UI

#ifndef NO_UI
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#endif

//#define ROTATE_FIGURES
#include "physics/functions.hpp"

const bool SIMULATE_BOTH = false;
const bool SIMULATE_TREE = true;
const bool START_PAUSED = false;
constexpr unsigned int VERTICES_COUNT = 7;
const unsigned int TREE_THRESHHOLD = 16; // 8
const float SCALE = 50.f;
const int CURRENT_ELEMENTS = 2000; // 20 000
const unsigned int MAX_ITERATIONS = -1; // -1 for infinity
const unsigned int MAX_SNAPSHOTS = 100;

const float AREA_WIDTH = 10000.f; // 10 000
const float AREA_HEIGHT = 10000.f; // 10 000
const float INITIAL_X = 0.f;
const float INITIAL_Y = 0.f;

static const float VIEW_HEIGHT = 600.f;

#ifndef NO_UI
void resizeView(const sf::RenderWindow& window, sf::View& view)
{
	float aspectRatio = float(window.getSize().x) / float(window.getSize().y);
	view.setSize(VIEW_HEIGHT * aspectRatio, VIEW_HEIGHT);
}

void zoomViewAt(sf::Vector2i pixel, sf::RenderWindow& window, float zoom, sf::View& view, int delta)
{
	// delta < 0 -> zoom out
	// zoom > 0 -> zoom out
	if (delta > 0)
	{
		const sf::Vector2f beforeCoord{ window.mapPixelToCoords(pixel) };
		view.zoom(1.0f / zoom);
		window.setView(view);
		const sf::Vector2f afterCoord{ window.mapPixelToCoords(pixel) };
		const sf::Vector2f offsetCoords{ beforeCoord - afterCoord };
		view.move(offsetCoords);
		window.setView(view);
	}
	else if (delta < 0)
	{
		view.zoom(zoom);
	}
}
#endif

#endif // SETTINGS_HPP