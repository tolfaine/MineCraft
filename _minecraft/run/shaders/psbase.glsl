varying vec3 normal;
varying vec3 vertex_to_light_vector;
varying vec4 color;

uniform sampler2D Texture0;
uniform float screen_width;
uniform float screen_height;

uniform float ambientLevel;
uniform float elapsed;
uniform vec3 eyePosition;

void main()
{

	vec3 normalized_normal = normalize(normal);
	vec3 normalized_vertex_to_light_vector = normalize(vertex_to_light_vector);
	float DiffuseTerm = clamp(dot(normal, vertex_to_light_vector), 0.0, 1.0);
	gl_FragColor = color * (DiffuseTerm*(1-0.3) + 0.3);
	gl_FragColor.a = color.a;


}