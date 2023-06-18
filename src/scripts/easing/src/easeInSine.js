// sinusoidal easing in - accelerating from zero velocity
Math.easeInSine = function (t, b, c, d) {
    return -c * Math.cos(t/d * (Math.PI/2)) + c + b;
};
