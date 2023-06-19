// quadratic easing in - accelerating from zero velocity
Math.easeInQuad = function (t, b, c, d) {
	return c*(t/=d)*t + b;
};
