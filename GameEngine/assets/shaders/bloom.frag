#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D image;
uniform sampler2D SceneTexture;
uniform float threshold;
uniform float Strength;


void main() {


  vec3 result = (texture(image, TexCoords).rgb);

  vec3 intensity_vec = smoothstep( threshold,  threshold + 0.25, result / 1.25);  
  float intensity = (intensity_vec.x + intensity_vec.y + intensity_vec.z) * 0.33333333;

  vec3 SceneColor = (texture(SceneTexture, TexCoords).xyz);
  FragColor = vec4(
    (mix(SceneColor, result * intensity + SceneColor, Strength)),
    1.0);
}
