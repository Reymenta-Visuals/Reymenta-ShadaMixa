// https://www.shadertoy.com/view/MtlGRn

// Reference: http://www.crytek.com/download/Sousa_Graphics_Gems_CryENGINE3.pdf on slide 36
// Implemented as GLSL example by Benjamin 'BeRo' Rosseaux - CC0

// o = tap sample xy, f = f-factor, n = diaphragm shape ngon, phiShutterMax = max. lens shutter rotation 
vec2 getBokehTapSampleCoord(const in vec2 o, const in float f, const float n, const in float phiShutterMax){
    vec2 ab = (o * 2.0) - vec2(1.0);    
    vec2 phir = ((ab.x * ab.x) > (ab.y * ab.y)) ? vec2((abs(ab.x) > 1e-8) ? ((PI * 0.25) * (ab.y / ab.x)) : 0.0, ab.x) : vec2((abs(ab.y) > 1e-8) ? ((PI * 0.5) - ((PI * 0.25) * (ab.x / ab.y))) : 0.0, ab.y); 
    phir.x += f * phiShutterMax;
   	phir.y *= (f > 0.0) ? pow((cos(PI / n) / cos(phir.x - ((2.0 * (PI / n)) * floor(((n * phir.x) + PI) / (2.0 * PI))))), f) : 1.0;
    return vec2(cos(phir.x), sin(phir.x)) * phir.y;
}

void main(void){
   
    // should for real usage: (fstop - fstopmin) / (fstopmax - fstopmin)
    float f = (sin(iGlobalTime) * 0.5) + 0.5; 
    
    // Diaphragm shape structure: 4.0 = box, 5.0 = pentagon, 6.0 = hexagon, 7.0 = heptagon, 8.0 = octagon etc.
    float ngon = 6.0; // 6.0 because I like hexagons :-) 
    
	vec2 coord = ((gl_FragCoord.xy / iResolution.xy) - vec2(0.5)) * 4.0 * vec2(1.0, iResolution.y /  iResolution.x);
    float v = 0.0;
    for(float y = 0.0; y <= 1.0; y += 1.0 / 16.0){
    	for(float x = 0.0; x <= 1.0; x += 1.0 / 16.0){
            vec2 c = getBokehTapSampleCoord(vec2(x, y), f, ngon, PI * 0.5);
            v = mix(1.0, v, pow(smoothstep(0.0, 0.05, length(coord - c)), 8.0));
		}
    }    
    
	gl_FragColor = vec4(v);
    
}