void main(void)
{
   vec2 uv = iZoom * gl_TexCoord[0].st;
   uv.x -= iRenderXY.x;
   uv.y -= iRenderXY.y;

   vec4 tex = texture2D(iChannel0, uv);
   gl_FragColor = vec4(vec3( tex.r, tex.g, tex.b ),1.0);
}

