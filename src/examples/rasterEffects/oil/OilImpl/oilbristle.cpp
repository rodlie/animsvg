#include "oilbristle.h"

#include <math.h>

#include "exceptions.h"

OilBristle::OilBristle(const SkPoint& position, float length) {
	// Check that the input makes sense
    if(length <= 0) RuntimeThrow("There bristle length should be higher than zero.");

	// Fill the positions and lengths containers
    unsigned int nElements = round(sqrt(length / 2)); // round(sqrt(2 * length));
    positions = vector<SkPoint>(nElements + 1, position);

    lengths.reserve(nElements);
	for (unsigned int i = 0; i < nElements; ++i) {
		lengths.push_back(nElements - i);
	}
}

void OilBristle::updatePosition(const SkPoint& newPosition) {
	// Set the first element head position
	positions[0] = newPosition;

	// Set the elements tail positions
    for (unsigned int i = 0, nElements = getNElements(); i < nElements; ++i) {
        const SkPoint& headPos = positions[i];
        SkPoint& tailPos = positions[i + 1];
        float length = lengths[i];
        float ang = std::atan2(headPos.y() - tailPos.y(),
                               headPos.x() - tailPos.x());
        tailPos.set(headPos.x() - length * cos(ang),
                    headPos.y() - length * sin(ang));
	}
}

void OilBristle::setElementsPositions(const SkPoint& newPosition) {
    for (SkPoint& pos : positions) {
		pos = newPosition;
	}
}

void OilBristle::setElementsLengths(const vector<float>& newLengths) {
	// Check that the input makes sense
    if(newLengths.size() != getNElements()) {
        RuntimeThrow("The newLengths vector should contain as many values as elements in the bristle");
	}

	lengths = newLengths;
}

void OilBristle::paint(SkCanvas& canvas, const SkColor& color, float thickness) const {
	// Set the stroke color
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(color);

	// Paint the bristle elements
	unsigned int nElements = getNElements();
	float deltaThickness = thickness / nElements;

    for (unsigned int i = 0; i < nElements; ++i) {
        paint.setStrokeWidth(thickness - i * deltaThickness);
        canvas.drawLine(positions[i].x(), positions[i].y(),
                        positions[i + 1].x(), positions[i + 1].y(), paint);
    }
}

unsigned int OilBristle::getNElements() const {
	return lengths.size();
}
