#pragma once

#include "oiltrace.h"

/**
 * @brief Class used to simulate an oil paint
 *
 * @author Javier Graciá Carpio
 */
class OilSimulator {
public:

    float BRISTLE_THICKNESS = 0.8f;
    float BRISTLE_DENSITY = 1.f;

	/**
	 * @brief The smaller brush size allowed
	 */
    float SMALLER_BRUSH_SIZE = 4;

    float BIGGER_BRUSH_SIZE = 4;

	/**
	 * @brief The brush size decrement ratio
	 */
    float BRUSH_SIZE_DECREMENT = 1.3;

	/**
	 * @brief The maximum number of invalid trajectories allowed before the brush size is reduced
	 */
	static unsigned int MAX_INVALID_TRAJECTORIES;

	/**
	 * @brief The maximum number of invalid trajectories allowed for the smaller brush size before the painting is finished
	 */
	static unsigned int MAX_INVALID_TRAJECTORIES_FOR_SMALLER_SIZE;

	/**
	 * @brief The maximum number of invalid traces allowed before the brush size is reduced
	 */
	static unsigned int MAX_INVALID_TRACES;

	/**
	 * @brief The maximum number of invalid traces allowed for the smaller brush size before the painting is finished
	 */
	static unsigned int MAX_INVALID_TRACES_FOR_SMALLER_SIZE;

	/**
	 * @brief The trace speed in pixels/step
	 */
	static float TRACE_SPEED;

	/**
	 * @brief The typical trace length, relative to the brush size
	 */
    float RELATIVE_TRACE_LENGTH = 2.3;

	/**
	 * @brief The minimum trace length allowed
	 */
    float MIN_TRACE_LENGTH = 16;

	/**
	 * @brief The canvas background color
	 */
    static SkColor BACKGROUND_COLOR;

	/**
	 * @brief The maximum color difference between the painted image and the already painted color to consider it well
	 * painted
	 */
    vector<int> MAX_COLOR_DIFFERENCE = { 40, 40, 40 };

	/**
	 * @brief The maximum allowed fraction of pixels in the trace trajectory that have been visited before
	 */
	static float MAX_VISITS_FRACTION_IN_TRAJECTORY;

	/**
	 * @brief The minimum fraction of pixels in the trace trajectory that should fall inside the canvas
	 */
	static float MIN_INSIDE_FRACTION_IN_TRAJECTORY;

	/**
	 * @brief The maximum allowed fraction of pixels in the trace trajectory with colors similar to the painted image
	 */
	static float MAX_SIMILAR_COLOR_FRACTION_IN_TRAJECTORY;

	/**
	 * @brief The maximum allowed value of the colors standard deviation along the trace trajectory
	 */
	static float MAX_COLOR_STDEV_IN_TRAJECTORY;

	/**
	 * @brief The minimum fraction of pixels in the trace that should fall inside the canvas
	 */
	static float MIN_INSIDE_FRACTION;

	/**
	 * @brief The maximum fraction of pixels in the trace with colors similar to the painted image
	 */
	static float MAX_SIMILAR_COLOR_FRACTION;

	/**
	 * @brief The maximum fraction of pixels in the trace that has been painted already
	 */
	static float MAX_PAINTED_FRACTION;

	/**
	 * @brief The minimum color improvement factor of the already painted pixels required to paint the trace on the
	 * canvas
	 */
	static float MIN_COLOR_IMPROVEMENT_FACTOR;

	/**
	 * @brief The minimum improvement fraction in the number of well painted pixels to consider to paint the trace even
	 * if there is not a significant color improvement
	 */
	static float BIG_WELL_PAINTED_IMPROVEMENT_FRACTION;

	/**
	 * @brief The minimum reduction fraction in the number of bad painted pixels required to paint the trace on the
	 * canvas
	 */
	static float MIN_BAD_PAINTED_REDUCTION_FRACTION;

	/**
	 * @brief The maximum allowed fraction of pixels in the trace that were previously well painted and will be now bad
	 * painted
	 */
	static float MAX_WELL_PAINTED_DESTRUCTION_FRACTION;

	/**
	 * @brief Constructor
	 *
	 * @param _useCanvasBuffer sets if the simulator should use a canvas buffer for the color mixing calculation
	 * @param _verbose sets if the simulator should print some debugging information
	 */
private:
    OilSimulator(const bool _useGpu, bool _useCanvasBuffer, bool _verbose);
public:
    OilSimulator(SkBitmap& dst, bool _useCanvasBuffer, bool _verbose);
    OilSimulator(SkCanvas& dst, bool _useCanvasBuffer, bool _verbose);

	/**
	 * @brief Sets the image that should be painted
	 *
	 * @param image the image that should be painted
	 * @param clearCanvas if true the canvas will be cleared before the painting starts
	 */
    void setImage(const SkBitmap& image, bool clearCanvas);

	/**
	 * @brief Updates the simulation
	 *
	 * @param stepByStep if true each update will paint one single step of the current trace. The trace will be painted
	 * completely otherwise.
	 */
	void update(bool stepByStep);

	/**
	 * @brief Indicates if the simulator finished the painting
	 *
	 * @return true if the painting is finished
	 */
    bool isFinished() const;
protected:

    /**
     * @brief Updates the pixel arrays
     */
    void updatePixelArrays();

	/**
	 * @brief Updates the visited pixels array
	 */
	void updateVisitedPixels();

	/**
	 * @brief Gets a new trace for the simulation
	 */
	void getNewTrace();

	/**
	 * @brief Checks if the trace trajectory falls in a region that has been visited before
	 *
	 * @return true if the trace trajectory falls in a region that has been visited before
	 */
	bool alreadyVisitedTrajectory() const;

	/**
	 * @brief Checks if the trace trajectory is valid
	 *
	 * To be valid it should fall on a region that was not painted correctly before, it should fall most of the time
	 * inside the canvas, and the image color changes should be small.
	 *
	 * @return true if the trace has a valid trajectory
	 */
	bool validTrajectory() const;

	/**
	 * @brief Checks if drawing the trace will improve the overall painting
	 *
	 * Note that the calculateBristleColors method should have been run before.
	 *
	 * @return false if the region covered by the trace was already painted with similar colors, most of the trace is
	 *         outside the canvas, or drawing the trace will not improve considerably the painting
	 */
	bool traceImprovesPainting() const;

	/**
	 * @brief Paints the current trace
	 */
	void paintTrace();

	/**
	 * @brief Paints a step of the current trace
	 */
	void paintTraceStep();

    /**
     * @brief Sets if a canvas should be GPU accelarated
     */
    const bool mUseGpu;

	/**
	 * @brief Sets if a canvas buffer should be used for the color mixing calculation
	 */
	bool useCanvasBuffer;

	/**
	 * @brief Sets if the simulator should print debugging information
	 */
	bool verbose;

    SkCanvas* mGpuDst = nullptr;
    SkBitmap mCpuDst;

	/**
	 * @brief The image to paint
	 */
    SkBitmap mImg;

	/**
	 * @brief The canvas where the oil painting is done
	 */
    std::shared_ptr<SkCanvas> mCanvas;
    int mCanvasWidth = 0;
    int mCanvasHeight = 0;

	/**
	 * @brief The canvas buffer used for the color mixing calculation
	 */
    std::shared_ptr<SkCanvas> mCanvasBuffer;

	/**
	 * @brief Container indicating which canvas pixels have been visited by previous traces
	 */
    vector<unsigned char> mVisitedPixels;

	/**
	 * @brief Container with the colors of the currently painted pixels
	 */
    SkBitmap mPaintedPixels;

	/**
	 * @brief Container with the indices of pixels that are currently bad painted
	 */
    vector<unsigned int> badPaintedPixels;

	/**
	 * @brief The total number of pixels that are currently bad painted
	 */
	unsigned int nBadPaintedPixels;

	/**
	 * @brief The current average brush size
	 */
	float averageBrushSize;

	/**
	 * @brief Indicates if the painting simulation is finished
	 */
	bool paintingIsFinised;

	/**
	 * @brief Indicates if a new trace should be obtained
	 */
	bool obtainNewTrace;

	/**
	 * @brief The current trace
	 */
    OilTrace trace;

	/**
	 * @brief The current trace step
	 */
	unsigned int traceStep;

	/**
	 * @brief The total number of painted traces
	 */
	unsigned int nTraces;
};
