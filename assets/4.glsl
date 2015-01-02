// functions begin
// https://www.shadertoy.com/view/ldj3Rd
vec3 AAIPalette[7]; // the color AAIPalette is stored here

// just pick a color based on c value
vec3 AAIGetColor(float c) 
{
   c=mod(c,7.); // cycle AAIPalette
   int p=0;
   vec3 color=vec3(0.);
   for(int i=0;i<7;i++) {
      if (float(i)-c<=.0) { // check loop index against color value
         color=AAIPalette[i]; // store color picked   
      }
   }
   return color;
}

// get a gradient of the AAIPalette based on c value, with a smooth parameter (0...1)
vec3 AAIGetSMColor(float c, float s) 
{
    s*=.5;
    c=mod(c-.5,7.);
    vec3 color1=vec3(0.0),color2=vec3(0.0);
    for(int i=0;i<7;i++) {
        if (float(i)-c<=.0) {
            color1 = AAIPalette[i];
            color2 = AAIPalette[(i+1>6)?0:i+1];
        }
    }
    // smooth mix the two colors
    return mix(color1,color2,smoothstep(.5-s,.5+s,fract(c)));
}
// functions end

// https://www.shadertoy.com/view/ldj3Rd
void main(void)
{
   vec2 uv = iZoom * gl_TexCoord[0].st;
   uv.x -= iRenderXY.x;
   uv.y -= iRenderXY.y;
   // define the colors (rainbow)
   AAIPalette[6]=vec3(255,000,000)/255.;
   AAIPalette[5]=vec3(255,127,000)/255.;
   AAIPalette[4]=vec3(255,255,000)/255.;
   AAIPalette[3]=vec3(000,255,000)/255.;
   AAIPalette[2]=vec3(000,000,255)/255.;
   AAIPalette[1]=vec3(075,000,130)/255.;
   AAIPalette[0]=vec3(143,000,255)/255.;
   
   vec3 color=vec3(0.);
   if (uv.x>.9) { 
      color=AAIGetSMColor(uv.y*7.+iGlobalTime*.5,.25+.75*abs(sin(iGlobalTime))); //gradient function
   } else if (uv.x<.1) {
      color=AAIGetColor(uv.y*7.-iGlobalTime*.5); // basic function
   } 

   vec2 p=(uv-.5);
   p.x*=iResolution.x/iResolution.y;
   

   // fractal
   float a=iGlobalTime*.05;   
   float ot,otl=ot=1000.;
   mat2 rot=mat2(cos(a),sin(a),-sin(a),cos(a));
   for(int i=0;i<14;i++) {
      p=abs(p+1.)-1.;
      p=p*1.25;
      p*=rot;
      ot=min(ot,abs(min(abs(p.y),abs(p.x))-.75)); //orbit trap 1
      ot=max(ot,length(p)*.02); //orbit trap 2
      otl=min(otl,abs(length(p)-.75)); //orbit trap 3
   }
   ot=pow(max(0.,1.-ot),10.); //orbit trap (0 to 1)
   if (length(max(vec2(0.),abs(uv-.5)-vec2(.485,.47)))>0.0) color*=0.; // border    
   if (iMouse.w>0. || length(max(vec2(0.),abs(uv-.5)-vec2(.37,.46)))<0.01) {
      color=AAIGetSMColor(ot*7.+length(uv)*14.,1.); //get color gradient for orbit trap value   
      color=mix(vec3(length(color)),color,.8)-pow(max(0.,1.-otl),25.)*.8+.1;
   }
	  gl_FragColor = vec4(color,1.0);
}
