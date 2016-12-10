#version 150

uniform mat4 projection_matrix;
uniform mat4 model_matrix;
uniform vec4 LightPosition;

in  vec4 vPosition;
in  vec2 vTexCoord;
in  vec3 vNormals;

out vec3 fN;
out vec3 fE;
out vec3 fL;
out vec2 texCoord;
out vec3 R;
out vec3 T;

void main() 
{

  texCoord = vTexCoord;

  gl_Position = projection_matrix*model_matrix*vPosition;

  fN = vNormals;
  fE = vPosition.xyz;
  fL = LightPosition.xyz;

  if (LightPosition.w != 0.0){
      fL = LightPosition.xyz - vPosition.xyz;
  }

  vec4 eyePos = vPosition;
  vec4 NN = model_matrix * vec4(vNormals, 1.0);
  vec3 N = normalize(NN.xyz);
  R = reflect(eyePos.xyz, N);
  float ratio = 1.00 / 2.42;
  T = refract(eyePos.xyz, N, ratio);
} 
