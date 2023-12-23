#version 330 core
uniform vec3 ambientColor = vec3(0.2, 0.2, 0.2);  
uniform int effect;

in vec3 Position;
in vec3 Normal;

out vec4 FragColor;  // Output fragment color
void ambientShading() {
	
}

void main() {
	vec3 lightDirection = vec3(1, -1, -1);
	vec3 lightColor = vec3(1, 1, 1);
	vec3 baseColor;
	if (effect == 0) {
		baseColor = vec3(0.6, 0.4, 0.2);
	// wood
	} else if (effect == 1) {
		baseColor = vec3(0.1, 0.8, 0.8);
	// glass
	}
	float diffuseStrength = max(dot(Normal, -lightDirection), 0.0);
    vec3 diffuse = lightColor * baseColor * diffuseStrength;

	FragColor =  normalize(vec4(baseColor * ambientColor, 1) 
	+ vec4(diffuse, 1));
}