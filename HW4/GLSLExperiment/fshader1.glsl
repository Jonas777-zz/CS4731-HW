#version 150
#extension GL_NV_shadow_samplers_cube : enable

//in  vec4  interpolatedColor;
in  vec2  texCoord;
in  vec3  fN;
in  vec3  fL;
in  vec3  fE;
in  vec3  R;
in  vec3  T;

out vec4  fColor;

uniform sampler2D texture;
uniform samplerCube texMap;
uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform mat4 model_matrix;
uniform vec4 LightPosition;
uniform float Shininess;
uniform float doTexture;

void main() 
{ 
	vec3 N = normalize(fN);
	vec3 E = normalize(fE);
	vec3 L = normalize(fL);

	vec3 H = normalize(L + E);

	vec4 ambient = AmbientProduct;

	float Kd = max(dot(L, N), 0.0);
	vec4 diffuse = Kd * DiffuseProduct;

	float Ks = pow(max(dot(N, H), 0.0), Shininess);
	vec4 specular = Ks * SpecularProduct;

	if(dot(L, N) < 0.0){
		specular = vec4(0.0, 0.0, 0.0, 1.0);
	}

	vec4 intColor = ambient + diffuse + specular;
	intColor.a = 1.0;

	if (doTexture == 1.0){
		fColor = intColor * texture2D( texture, texCoord );
	}
	else if (doTexture == 2.0){
		vec4 texColor = textureCube(texMap, R);
		fColor = texColor;
	}
	else if (doTexture == 3.0){
		vec4 refractColor = textureCube(texMap, T);
		fColor = refractColor;
	}
	else {
		fColor = intColor;
	}
} 

