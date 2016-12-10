#version 150

uniform mat4 projection_matrix;
uniform mat4 model_matrix;
uniform vec4 LightPosition;

in  vec4 vPosition;
in  vec2 vTexCoord;
//in  vec4 vColor;
in  vec3 vNormals;

//out vec4 interpolatedColor;
out vec3 fN;
out vec3 fE;
out vec3 fL;
out vec2 texCoord;
out vec3 R;

void main() 
{
  // PROTIP #3
  // can extend to gl_Position = projection * camera * models * vertex
  // you may wish to view camera motion as a transform on the projection (projection * camera) * models ...
  // this could be thought of as moving the observer in space
  // or you may view camera motion as transforming all objects in the world while the observer remains still
  // projection * (camera * models) * ...
  // notice mathematically there is no difference
  // however in rare circumstances the order of the transform may affect the numerical stability
  // of the overall projection

  texCoord = vTexCoord;

  gl_Position = projection_matrix*model_matrix*vPosition;

  //interpolatedColor = vColor;

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
} 
