// cubic easing in - accelerating from zero velocity
Math.easeInCubic = function (t, b, c, d) {
	return c*(t/=d)*t*t + b;
};
