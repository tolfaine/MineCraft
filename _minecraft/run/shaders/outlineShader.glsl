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

void main()
{
	// Transforming The Vertex

//	gl_Position = gl_Vertex;
	//gl_Position.y += (sin(gl_Position.x)) * 5;
	//gl_Position.x += (sin(randomVal + gl_Position.x));
//	gl_Position =  mvp *  gl_Position;


	//gl_Position = m * gl_Vertex;
	//gl_Position =   p * v *  gl_Position;

	//gl_Position =   mvp*  gl_Vertex;

// 	gl_Position =   p*  gl_ModelViewMatrix*  gl_Vertex;

//	gl_Position =   m * gl_Vertex;
	//gl_Position.y += (sin(elapsed+ gl_Position.x)) * 10;
	//gl_Position =   p * v * gl_Position;

	gl_Position =  gl_ProjectionMatrix *gl_ModelViewMatrix * gl_Vertex;
	normal = gl_NormalMatrix * gl_Normal; 
	normalize(normal);
	gl_Position += normal;
	vertex_to_light_vector = vec3(gl_LightSource[0].position);
	color = gl_Color;
}