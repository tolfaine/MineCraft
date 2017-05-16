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

	//vec3 newNormal = normalize(normal);


	//mat3 mvReduc = mat3(gl_ModelViewMatrix[0].xyz, gl_ModelViewMatrix[1].xyz, gl_ModelViewMatrix[2].xyz);
	//vec3 viewNormal  = mvReduc * newNormal;
	//gl_Position = gl_ProjectionMatrix * vec4(viewNormal, 0.0) ;


	//vec3 newNormal = normalize(normal);
	//vec3 V = normalize(eyePosition - worldPos );

	//if(dot(V, newNormal) < 0.3){
	//color =  vec4(0.8,0.0,0.5,1.0);
	//}


	gl_Position =  gl_ProjectionMatrix *gl_ModelViewMatrix * gl_Vertex;
	normal = gl_NormalMatrix * gl_Normal; 
	normalize(normal);
	gl_Position += normal;
	vertex_to_light_vector = vec3(gl_LightSource[0].position);
	color =  vec4(0.8,0.0,0.5,1.0);


	//gl_Position = gl_Vertex;
//	color =  gl_Color;
}