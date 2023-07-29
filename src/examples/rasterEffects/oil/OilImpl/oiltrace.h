#pragma once

#include "oilbrush.h"

#include <vector>
using std::vector;

/**
 * @brief Class that simulates the movement of a brush on the canvas
 *
 * @author Javier Graciá Carpio
 */
class OilTrace {
public:

	/**
	 * @brief Sets how random the trace movement is
	 */
	static float NOISE_FACTOR;

	/**
	 * @brief The minimum alpha value to be considered for the trace average color calculation
	 */
	static unsigned char MIN_ALPHA;

	/**
	 * @brief The brightness relative change range between the bristles colors
	 */
	static float BRIGHTNESS_RELATIVE_CHANGE;

	/**
	 * @brief The typical trajectory step when the color mixing starts
	 */
	static unsigned int TYPICAL_MIX_STARTING_STEP;

	/**
	 * @brief The color mixing strength
	 */
	static float MIX_STRENGTH;

	/**
	 * @brief Constructor
	 *
	 * @param startingPosition the trace starting position
	 * @param nSteps the total number of steps in the trace trajectory
	 * @param speed the trace moving speed (pixels/step)
	 */
	OilTrace(const SkPoint& startingPosition = SkPoint(), unsigned int nSteps = 20, float speed = 2);

	/**
	 * @brief Constructor
	 *
	 * @param _positions the trace trajectory positions
	 * @param _alphas the trace alpha values at each trajectory step
	 */
    OilTrace(const vector<SkPoint>& _positions, const vector<unsigned char>& _alphas);

	/**
	 * @brief Sets the trace brush size
	 *
	 * @param brushSize the brush size
	 */
    void setBrushSize(float brushSize, float bristleThickness,
                      float bristleDensity);

	/**
	 * @brief Sets the trace average color
	 *
	 * @param color the trace average color
	 */
    void setAverageColor(const SkColor& color);

	/**
	 * @brief Calculates the trace average color along the painted image
	 *
	 * @param img the painted image
	 */
    void calculateAverageColor(const SkBitmap& img);

	/**
	 * @brief Calculates the trace bristle colors
	 *
	 * @param paintedPixels the painted pixels
	 * @param backgroundColor the background color
	 */
    void calculateBristleColors(const SkBitmap& paintedPixels,
                                const SkColor& backgroundColor);

	/**
	 * @brief Paints the trace
	 *
	 * Note that the calculateBristleColors method should have been run before.
	 */
    void paint(SkCanvas& canvas);

	/**
	 * @brief Paints the trace
	 *
	 * Note that the calculateBristleColors method should have been run before.
	 *
	 * @param canvasBuffer the canvas buffer where the trace should also be painted when the color exceeds a minimum
	 * alpha value
	 */
    void paint(SkCanvas& canvas, SkCanvas& canvasBuffer);

	/**
	 * @brief Paints a given step in the trace trajectory
	 *
	 * Note that the calculateBristleColors method should have been run before.
	 *
	 * @param step the trace trajectory step to paint
	 */
    void paintStep(SkCanvas& canvas, unsigned int step);

	/**
	 * @brief Paints a given step in the trace trajectory
	 *
	 * Note that the calculateBristleColors method should have been run before.
	 *
	 * @param step the trace trajectory step to paint
	 * @param canvasBuffer the canvas buffer where the trace should also be painted when the color exceeds a minimum
	 * alpha value
	 */
    void paintStep(SkCanvas& canvas, unsigned int step, SkCanvas& canvasBuffer);

	/**
	 * @brief Returns the number of steps in the trace trajectory
	 *
	 * @return the total number of steps in the trace trajectory
	 */
	unsigned int getNSteps() const;

	/**
	 * @brief Returns the trace trajectory positions
	 *
	 * @return the trace trajectory positions
	 */
    const vector<SkPoint>& getTrajectoryPositions() const;

	/**
	 * @brief Returns the trace alpha values along the trace trajectory
	 *
	 * @return the trace alpha values along the trace trajectory
	 */
	const vector<unsigned char>& getTrajectoryAphas() const;

	/**
	 * @brief Returns the trace average color
	 *
	 * @return the trace average color
	 */
    const SkColor& getAverageColor() const;

	/**
	 * @brief Returns the number of bristles in the brush
	 *
	 * @return the total number of bristles in the brush
	 */
	unsigned int getNBristles() const;

	/**
	 * @brief Returns the brush bristle positions along the trace trajectory
	 *
	 * @return the brush bristle positions along the trace trajectory
	 */
    const vector<vector<SkPoint>>& getBristlePositions() const;

	/**
	 * @brief Returns the brush bristle image colors along the trace trajectory
	 *
	 * @return the brush bristle image colors along the trace trajectory
	 */
    const vector<vector<SkColor>>& getBristleImageColors() const;

	/**
	 * @brief Returns the brush bristle painted colors along the trace trajectory
	 *
	 * @return the brush bristle painted colors along the trace trajectory
	 */
    const vector<vector<SkColor>>& getBristlePaintedColors() const;

	/**
	 * @brief Returns the brush bristle colors along the trace trajectory
	 *
	 * @return the brush bristle colors along the trace trajectory
	 */
    const vector<vector<SkColor>>& getBristleColors() const;

protected:

	/**
	 * @brief Calculates the bristles positions along the trace trajectory
	 */
	void calculateBristlePositions();

	/**
	 * @brief Calculates the image colors at the bristles positions
	 *
	 * @param img the painted image
	 */
    void calculateBristleImageColors(const SkBitmap& img);

	/**
	 * @brief Calculates the painted colors at the bristles positions
	 *
	 * @param paintedPixels the painted pixels
	 * @param backgroundColor the canvas background color
	 */
    void calculateBristlePaintedColors(const SkBitmap& paintedPixels,
                                       const SkColor& backgroundColor);

	/**
	 * @brief The trace trajectory positions
	 */
    vector<SkPoint> positions;

	/**
	 * @brief The bristle colors alpha values at each trajectory position
	 */
    vector<unsigned char> alphas;

	/**
	 * @brief The trace average color
	 */
    SkColor averageColor;

	/**
	 * @brief The trace brush
	 */
    OilBrush brush;

	/**
	 * @brief The trace bristle positions along the trajectory
	 */
    vector<vector<SkPoint>> bPositions;

	/**
	 * @brief The trace bristle image colors along the trajectory
	 */
    vector<vector<SkColor>> bImgColors;

	/**
	 * @brief The trace bristle painted colors along the trajectory
	 */
    vector<vector<SkColor>> bPaintedColors;

	/**
     * @brief The trace bristle colors along the trajectory
	 */
    vector<vector<SkColor>> bColors;
};
