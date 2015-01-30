// https://www.shadertoy.com/view/4djXRD
// http://demosceneacademy.wordpress.com/
// live coding: https://www.youtube.com/watch?v=RGmgHfbU0hU

// Tutorial básico sobre raymarching.
// por Vinicius Graciano Santos (vgs).

// Constantes.
//#define STEPS 64
#define EPS 0.01
#define FAR 10.0

// Função de distância.
float map(vec3 p) {
    float v = 2.5*iGlobalTime;
    float r = iColor.r*1.35 + 0.15*cos(10.0*p.y + v) + 0.15*cos(10.0*p.x + v);
    return length(p) - r;
}

// Gradiente (numérico) da função de distância.
vec3 grad(vec3 p) {
    vec2 q = vec2(0.0, EPS);
    return vec3(map(p + q.yxx) - map(p - q.yxx),
                map(p + q.xyx) - map(p - q.xyx),
                map(p + q.xxy) - map(p - q.xxy));
}

vec3 shade(vec3 ro, vec3 rd, float t) {
    vec3 n = normalize(grad(ro +t*rd));
    return 0.05+0.95*iColor*pow((1.0-dot(-rd, n)), 3.0);
}

void main(void) {
    // Prepara o raio.
	vec2 uv = 2.0 * iZoom * (gl_FragCoord.xy / iResolution.xy- 0.5);
	uv.x *= float(iResolution.x )/ float(iResolution.y);
	uv.x -= iRenderXY.x;
	uv.y -= iRenderXY.y;
   
    vec3 ro = vec3(0.0, 0.0, 2.5); // início do raio.
    vec3 rd = normalize(vec3(uv, -1.0)); // direção do raio.
    
    // Loop do raymarcher.
    float t = 0.0, d = EPS;
    for (int i = 0; i < iSteps; ++i) {
        d = map(ro + t*rd);
        if (d < EPS || t > FAR) break;
        t += d;
    }

    // Shading.
    vec3 col = d < EPS ? shade(ro, rd, t) : mix(vec3(0.0), iBackgroundColor, 2.0-length(uv)) ;
    
    // Pós-processamento. 
    col = smoothstep(0.0, 1.0, col);
    col = pow(col, vec3(0.45));
    
	gl_FragColor = vec4(col,1.0);
}

