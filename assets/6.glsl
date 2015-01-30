void main(void)
{
	vec2 c = gl_FragCoord.xy / iResolution.xy;

	float x = (c.x - 0.5)*iZoom*2.0;
	float y = (c.y - 0.5)*iZoom*2.0;

	vec2 uv = vec2(x, y);
   	uv.x -= iRenderXY.x;
   	uv.y -= iRenderXY.y;

   	vec4 tex = texture2D(iChannel2, c-uv);
   	gl_FragColor = vec4(vec3( tex.r, tex.g, tex.b ),1.0);
}

