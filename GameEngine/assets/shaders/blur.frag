#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D image;
uniform bool horizontal;
uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);
void main() {             
  vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
  vec3 result = texture(image, TexCoords).rgb * weight[0];
  if(horizontal) {
    for(int i = 1; i < 5; ++i) {
      result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
      result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
    }
  }
  else {
    for(int i = 1; i < 5; ++i) {
        result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
    }
  }

  //FragColor = mix( vec4(result.xxx, 1.0), texture(scene_texture, TexCoords), .5
  //FragColor = mix( texture(image, TexCoords), texture(scene_texture, TexCoords), .5);
  // result = vec3(
  //   pow(result.r, 3),
  //   pow(result.g, 3),
  //   pow(result.b, 3)
  // );
//   result = pow(result, vec3(1.0 / 2.2));
//   float intensity = result.x + result.y + result.z;
//  intensity = smoothstep( threshold,  threshold + 0.25, intensity * 0.3333333);  
FragColor = vec4(
    result ,
    1.0);
//   FragColor = vec4(
//     result * intensity + texture(scene_texture, TexCoords).xyz,
//     1.0);
}