#include "oilbrush.h"
#include "oilbristle.h"

#include "oilhelpers.h"
using namespace OilHelpers;

#include "simplemath.h"
#include "exceptions.h"

float OilBrush::MAX_BRISTLE_LENGTH = 15;

float OilBrush::MAX_BRISTLE_THICKNESS = 5;

float OilBrush::MAX_BRISTLE_HORIZONTAL_NOISE = 4;

float OilBrush::BRISTLE_VERTICAL_NOISE = 8;

float OilBrush::NOISE_SPEED_FACTOR = 0.04;

unsigned int OilBrush::POSITIONS_FOR_AVERAGE = 4;

OilBrush::OilBrush(const SkPoint& _position, float _size,
                   float _bristlesThickness, float _bristlesDensity) :
		position(_position), size(_size) {
	// Calculate some of the bristles properties
    bristlesLength = qMin(size, MAX_BRISTLE_LENGTH);
    bristlesThickness = qMin(_bristlesThickness * bristlesLength, MAX_BRISTLE_THICKNESS);
    bristlesHorizontalNoise = qMin(0.3f * size, MAX_BRISTLE_HORIZONTAL_NOISE);
    bristlesHorizontalNoiseSeed = gSkRandF(0, 1000);

	// Initialize the bristles offsets and positions containers with default values
    unsigned int nBristles = floor(size * gSkRandF(_bristlesDensity*1.6,
                                                   _bristlesDensity*1.9));
    bOffsets = vector<SkPoint>(nBristles);
    bPositions = vector<SkPoint>(nBristles);

	// Randomize the bristle offset positions
	for (SkPoint& offset : bOffsets) {
        offset.set(size * gSkRandF(-0.5, 0.5),
                   BRISTLE_VERTICAL_NOISE * gSkRandF(-0.5, 0.5));
	}

	// Initialize the variables used to calculate the brush average position
	averagePosition = position;
	positionsHistory.push_back(position);
	updatesCounter = 0;
}

void OilBrush::resetPosition(const SkPoint& newPosition) {
	// Reset the brush position
	position = newPosition;

	// Reset the variables used to calculate the brush average position
	averagePosition = position;
	positionsHistory.clear();
	positionsHistory.push_back(position);
	updatesCounter = 0;
}

void OilBrush::updatePosition(const SkPoint& newPosition, bool updateBristlesElements) {
	// Update the brush position
	position = newPosition;

	// Increment the updates counter
	updatesCounter++;

	// Add the new position to the positions history
	if (positionsHistory.size() < POSITIONS_FOR_AVERAGE) {
		positionsHistory.push_back(position);
	} else {
		positionsHistory[updatesCounter % POSITIONS_FOR_AVERAGE] = position;
	}

	// Update the average position
	SkPoint prevAveragePosition = averagePosition;
    averagePosition.set(0, 0);
	int counter = 0;

	for (const SkPoint& pos : positionsHistory) {
		averagePosition += pos;
		++counter;
	}

    averagePosition.set(averagePosition.x()/counter,
                        averagePosition.y()/counter);

	// Update the bristles containers only if the average position is stable or is close to be stable
	if (positionsHistory.size() >= POSITIONS_FOR_AVERAGE - 1) {
		// Calculate the direction angle
        float directionAngle = 0.5*PI
                + atan2(averagePosition.y() - prevAveragePosition.y(),
                        averagePosition.x() - prevAveragePosition.x());

		// Update the bristles positions
		unsigned int nBristles = getNBristles();
		float cosAng = cos(directionAngle);
		float sinAng = sin(directionAngle);
		float noisePos = bristlesHorizontalNoiseSeed + NOISE_SPEED_FACTOR * updatesCounter;

		for (unsigned int i = 0; i < nBristles; ++i) {
			// Add some horizontal noise to the offset to make it look more realistic
			const SkPoint& offset = bOffsets[i];
            float x = offset.x() + bristlesHorizontalNoise * (ofNoise(noisePos + 0.1 * i) - 0.5);
            float y = offset.y();

			// Rotate the offset and add it to the brush central position
            bPositions[i].set(position.x() + (x * cosAng - y * sinAng),
                              position.y() + (x * sinAng + y * cosAng));
		}

		// Update the bristles elements to their new positions if necessary
		if (updateBristlesElements) {
			// Check if the bristles container should be initialized
			if (bristles.size() == 0) {
                bristles = vector<OilBristle>(nBristles, OilBristle(SkPoint(), bristlesLength));
			}

			if (positionsHistory.size() == POSITIONS_FOR_AVERAGE - 1) {
				for (unsigned int i = 0; i < nBristles; ++i) {
					bristles[i].setElementsPositions(bPositions[i]);
				}
			} else {
				for (unsigned int i = 0; i < nBristles; ++i) {
					bristles[i].updatePosition(bPositions[i]);
				}
			}
		}
	}
}

void OilBrush::paint(SkCanvas& canvas, const SkColor& color) const {
	if (positionsHistory.size() == POSITIONS_FOR_AVERAGE) {
		for (const OilBristle& bristle : bristles) {
            bristle.paint(canvas, color, bristlesThickness);
		}
	}
}

void OilBrush::paint(SkCanvas& canvas, const vector<SkColor>& colors, unsigned char alpha) const {
	// Check that the input makes sense
	if (colors.size() != getNBristles()) {
        RuntimeThrow("There should be one color for each bristle in the brush.");
	}

	if (positionsHistory.size() == POSITIONS_FOR_AVERAGE) {
        for (int i = 0, nBristles = getNBristles(); i < nBristles; ++i) {
            const SkColor iCol = SkColorSetA(colors[i], alpha);
            bristles[i].paint(canvas, iCol, bristlesThickness);
		}
	}
}

unsigned int OilBrush::getNBristles() const {
	return bOffsets.size();
}

const vector<SkPoint> OilBrush::getBristlesPositions() const {
    return positionsHistory.size() == POSITIONS_FOR_AVERAGE ? bPositions : vector<SkPoint>();
}
