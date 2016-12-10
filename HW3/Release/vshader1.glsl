#version 150

in  vec4 vPosition;
in  vec3 vNormal;
out vec4 color;

//out vec3 fN;
//out vec3 fE;
//out vec3 fL;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform mat4 projection_matrix;
uniform mat4 model_matrix;
uniform vec4 lightPos;

uniform float shininess;

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
  //vec3 pos = (model_matrix * vPosition).xyz;
  //vec3 L = normalize(lightPos.xyz - pos);
  //vec3 E = normalize(-pos);
  //vec3 H = normalize(L + E);
  //vec3 N = normalize(model_matrix * vec4(vNormal, 0.0)).xyz;
  //float cos_theta = max(dot(L, N), 0.0);
  //vec4 diffuse = cos_theta * diffuseProd;
  //float cos_phi = pow(max(dot(N, H), 0.0), shininess);
  //vec4 specular = cos_phi * specularProd;
  //if(dot(L, N) < 0.0) specular = vec4(0.0, 0.0, 0.0, 1.0);
  //interpolatedColor = ambient + diffuse + specular;
  //interpolatedColor.a = 1.0;

  //fN = vNormal;
  //fE = -vPosition.xyz;
  //fL = lightPos.xyz;

  //if(lightPos.w != 0.0){
  //    fL = lightPos.xyz - vPosition.xyz;
  //}

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

  //gl_Position = projection_matrix * model_matrix * vPosition;
  color = ambient + diffuse + specular;
  color.a = 1.0;

  gl_Position = projection_matrix*model_matrix*vPosition;
} 
