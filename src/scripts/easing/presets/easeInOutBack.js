/*_FRICTION_EXPRESSION_PRESET_*/
// back easing in/out - backtracking slightly, then reversing direction and moving to target,
// then overshooting target, reversing, and finally coming back to target
Math.easeInOutBack = function (t, b, c, d, s) {
	if (s == undefined) s = 1.70158; 
	if ((t/=d/2) < 1) return c/2*(t*t*(((s*=(1.525))+1)*t - s)) + b;
	return c/2*((t-=2)*t*(((s*=(1.525))+1)*t + s) + 2) + b;
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
return Math.easeInOutBack(frame, startVal, endVal, duration);
