/*_FRICTION_EXPRESSION_PRESET_*/
// quintic easing in/out - acceleration until halfway, then deceleration
Math.easeInOutQuint = function (t, b, c, d) {
	if ((t/=d/2) < 1) return c/2*t*t*t*t*t + b;
	return c/2*((t-=2)*t*t*t*t + 2) + b;
};
/*_FRICTION_EXPRESSION_PRESET_*/
current = $frame;
/*_FRICTION_EXPRESSION_PRESET_*/
frame = current;
startVal = __START_VALUE__;
endVal = __END_VALUE__;
startFrame = __START_FRAME__;
endFrame = __END_FRAME__;
duration = endFrame - startFrame;
endVal = endVal - startVal;
if (frame < startFrame) { frame = startFrame; }
else if (frame > endFrame) { frame = endFrame; }
if (frame >= startFrame && frame <= endFrame) { frame = frame - startFrame; }
if (duration <= 0) { duration = 1; }
if (frame < 0) { frame = 0; }
return Math.easeInOutQuint(frame, startVal, endVal, duration);
