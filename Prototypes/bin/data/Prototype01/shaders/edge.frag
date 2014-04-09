#version 120
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect tex0;

uniform float time;
uniform float radius;
uniform float intensity;

float getIntensity(vec2 u){
	vec3 a = texture2DRect(tex0,u).xyz;
	return (a.x+a.y+a.z)/3.0;
}

void main(){
	vec2 uv = gl_TexCoord[0].st;//gl_FragCoord.xy;
	vec2 p = vec2(radius*10.0);
	
	float avg = 0.0;
	avg += getIntensity(uv+vec2(p.x,0.0));
	avg += getIntensity(uv+vec2(-p.x,0.0));
	avg += getIntensity(uv+vec2(0.0,p.y));
	avg += getIntensity(uv+vec2(0.0,-p.y));
	avg += getIntensity(uv+vec2(p.x,p.y));
	avg += getIntensity(uv+vec2(-p.x,-p.y));
	avg += getIntensity(uv+vec2(p.x,-p.y));
	avg += getIntensity(uv+vec2(-p.x,p.y));
	avg /= 8.0;
	
	float result = (1.0-getIntensity(uv)) + avg;
	result = (1.0-result)* (intensity*10.0);

    gl_FragColor.rgb = vec3(result);
    gl_FragColor.a = 1.0;
}