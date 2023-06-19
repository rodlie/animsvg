/*_FRICTION_EXPRESSION_PRESET_*/
// circular easing out - decelerating to zero velocity
Math.easeOutCirc = function (t, b, c, d) {
	return c * Math.sqrt(1 - (t=t/d-1)*t) + b;
};
/*_FRICTION_EXPRESSION_PRESET_*/
current = $frame;
/*_FRICTION_EXPRESSION_PRESET_*/
frame = current;
startVal = __START_VALUE__;
endVal = __END_VALUE__;
startFrame = __START_FRAME__;
endFrame = __END_FRAME__;
duration = endFrame-startFrame;
if (frame < startFrame) { frame = startFrame; }
else if (frame > endFrame) { frame = endFrame; }
if (frame >= startFrame && frame <= endFrame) { frame = frame - startFrame; }
if (duration <= 0) { duration = 1; }
if (frame < 0) { frame = 0; }
return Math.easeOutCirc(frame, startVal, endVal, duration);
