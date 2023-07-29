#pragma once

#include <vector>
using std::vector;

#include "skia/skiaincludes.h"

/**
 * @brief Class that simulates the movement of a bristle
 *
 * @author Javier Graciá Carpio
 */
class OilBristle {
public:

	/**
	 * @brief Constructor
	 *
	 * @param position the bristle position
	 * @param length the bristle total length
	 */
    OilBristle(const SkPoint& position = SkPoint(), float length = 10);

	/**
	 * @brief Updates the bristle position
	 *
	 * @param newPosition the new bristle position
	 */
    void updatePosition(const SkPoint& newPosition);

	/**
	 * @brief Sets the bristle elements positions
	 *
	 * @param newPosition the new bristle elements position
	 */
    void setElementsPositions(const SkPoint& newPosition);

	/**
	 * @brief Sets the bristle elements lengths
	 *
	 * @param newLengths the new bristle elements lengths
	 */
    void setElementsLengths(const vector<float>& newLengths);

	/**
	 * @brief Paints the bristle
	 *
	 * @param color the color to use
	 * @param thickness the thickness of the first bristle element
	 */
    void paint(SkCanvas& canvas, const SkColor& color, float thickness) const;

	/**
	 * @brief Returns the number of bristle elements
	 *
	 * @return the number of bristle elements
	 */
    unsigned int getNElements() const;

protected:

	/**
	 * @brief The bristle elements positions
	 */
    vector<SkPoint> positions;

	/**
	 * @brief The bristle elements lengths
	 */
    vector<float> lengths;
};
