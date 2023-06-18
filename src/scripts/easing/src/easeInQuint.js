// quintic easing in - accelerating from zero velocity
Math.easeInQuint = function (t, b, c, d) {
	return c*(t/=d)*t*t*t*t + b;
};
