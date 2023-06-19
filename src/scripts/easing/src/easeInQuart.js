// quartic easing in - accelerating from zero velocity
Math.easeInQuart = function (t, b, c, d) {
	return c*(t/=d)*t*t*t + b;
};
