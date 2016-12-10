#version 150

in vec4 vPosition;
in vec3 vNormal;
out vec4 color;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform mat4 model_matrix;
uniform mat4 projection_matrix;
uniform vec4 lightPos;

uniform float shininess;

void main()
{
	vec3 pos = (model_matrix * vPosition).xyz;

	vec3 L = normalize(lightPos.xyz - pos);
	vec3 E = normalize(-pos);
	vec3 H = normalize(L + E);

	vec3 N = normalize(model_matrix * vec4(vNormal, 0.0)).xyz;

	vec4 ambient = AmbientProduct;

	float Kd = max(dot(L, N), 0.0);
	vec4 diffuse = Kd * DiffuseProduct;

	float Ks = pow(max(dot(N, H), 0.0), shininess);
	vec4 specular = Ks * SpecularProduct;

	if(dot(L, N) < 0.0) specular = vec4(0.0, 0.0, 0.0, 1.0);

	gl_Position = projection_matrix * model_matrix * vPosition;
	color = ambient + diffuse + specular;
	color.a = 1.0;
}