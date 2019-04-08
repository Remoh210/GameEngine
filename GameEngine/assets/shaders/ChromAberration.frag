#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D image;

void main() {             
  
  
	vec4 color = vec4(0.0);
	color.r = texture(image, TexCoords - 0.002).r;
	color.g = texture(image, TexCoords + 0.000).g;
	color.b = texture(image, TexCoords + 0.002).b;
	color.a = 1.0; 
 
	FragColor = color;
}