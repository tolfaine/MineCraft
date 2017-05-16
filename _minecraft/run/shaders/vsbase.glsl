varying vec3 normal;
varying vec3 vertex_to_light_vector;
varying vec4 color;

uniform float elapsed;
uniform mat4 invertView;
uniform mat4 mvp;
uniform float randomVal;

uniform mat4 m;
uniform mat4 v;
uniform mat4 p;
uniform mat4 mv;

uniform mat4 View;
uniform mat4 InvertView;


void main()
{
	
	gl_Position =  InvertView *gl_ModelViewMatrix * gl_Vertex;


	if(gl_Color.b > 0.7 && gl_Color.r < 0.8){
		float param = (gl_Position.x+ gl_Position.y)/2;
		//param /= 200;
		param /= 10;
		param += elapsed;
		float temp = sin(param) * cos(param);
		temp *= 3;
		gl_Position.z += temp;
		//gl_Color.a = 0.4;
	}

	gl_Position = View * gl_Position;
	gl_Position = gl_ProjectionMatrix * gl_Position;

	//gl_Position.y += (sin(gl_Position.x + elapsed)) *;

	normal = gl_NormalMatrix * gl_Normal; 
	normalize(normal);

	vertex_to_light_vector = vec3(gl_LightSource[0].position);
	color = gl_Color;
}