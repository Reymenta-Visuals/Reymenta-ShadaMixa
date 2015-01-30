// https://www.shadertoy.com/view/MslGRH

// With tweaks by PauloFalcao

float BinarySerpentsTexture3D(vec3 n, float res){
  n = floor(n*res+.5);
  return fract(sin((n.x+n.y*1e5+n.z*1e7)*1e-4)*1e5);
}

float BinarySerpentsmap( vec3 p ){
    p.x+=sin(p.z*4.0+iGlobalTime*4.0)*0.1*cos(iGlobalTime*0.1);
    p = mod(p,vec3(1.0, 1.0, 1.0))-0.5;
    return length(p.xy)-.1;
}
void main(void)
{
   vec2 uv = iZoom * gl_TexCoord[0].st * 2.0 - 0.5;
   uv.x -= iRenderXY.x;
   uv.y -= iRenderXY.y;
//   	vec2 pos = (gl_FragCoord.xy*iZoom * 2.0 - iResolution.xy) / iResolution.y;
// 	pos.x -= iRenderXY.x;
//	pos.y -= iRenderXY.y;
  	 vec3 camPos = vec3(cos(iGlobalTime*0.3), sin(iGlobalTime*0.3), 1.5);
    vec3 camTarget = vec3(0.0, 0.0, 0.0);

    vec3 camDir = normalize(camTarget-camPos);
    vec3 camUp  = normalize(vec3(0.0, 1.0, 0.0));
    vec3 camSide = cross(camDir, camUp);
    float focus = 2.0;

    vec3 rayDir = normalize(camSide*uv.x + camUp*uv.y + camDir*focus);
    vec3 ray = camPos;
    float d = 0.0, total_d = 0.0;
    const int MAX_MARCH = 100;
    const float MAX_DISTANCE = 5.0;
    float c = 1.0;
    for(int i=0; i<MAX_MARCH; ++i) {
        d = BinarySerpentsmap(ray);
        total_d += d;
        ray += rayDir * d;
        if(abs(d)<0.001) { break; }
        if(total_d>MAX_DISTANCE) { c = 0.; total_d=MAX_DISTANCE; break; }
    }
	
    float fog = 5.0;
    vec3 result = vec3( vec3(iColor.r , iColor.g, iColor.b) * (fog - total_d) / fog );

    ray.z -= 5.+iGlobalTime*.5;
    float r = BinarySerpentsTexture3D(ray, 33.);
  gl_FragColor = vec4(result*(step(r,iFreq0/150.0)+r*iFreq1/150.0+.1),1.0);
}


