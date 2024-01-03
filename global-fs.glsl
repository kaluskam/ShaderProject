#version 330 core
uniform int effect;

in vec3 Position;
in vec3 Normal;
in vec3 Eye;
in vec2 fragTexcoord;

out vec4 FragColor;  // Output fragment color

const vec3 lightDir = vec3(1, -1, -1);
const vec3 lightColor = vec3(1, 1, 1);

vec3 ambientShading(vec3 color) {
	float ambientStrength = 0;
	return ambientStrength * color;
}

vec3 diffuseShading(vec3 color) {
	float diff = max(dot(Normal, -lightDir), 0.0);
	return lightColor * color * diff;
}

vec3 specularShading(vec3 color) {
	float shininess = 1000.0;
	vec3 H = normalize(lightDir + Eye);
	float spec = pow(max(dot(Normal, H), 0.0), shininess);
	return lightColor * color * spec;	
}

vec3 specularShadingWithReflectance(vec3 color) {
	float shininess = 1000.0;
	vec3 H = normalize(lightDir + Eye);
	float spec = pow(max(dot(Normal, H) * 0.5 + 0.5, 0.0), shininess);

	float refractiveIndex = 1.5;
	float R0 = pow((1.0 - refractiveIndex) / (1.0 + refractiveIndex), 2.0);
	float reflectance = R0 + (1.0 - R0) * pow(1.0 - dot(-Eye, Normal), 5.0);
	return mix(spec * color, vec3(1.0), reflectance);
}

vec3 BlinnPhongShading(vec3 color) {
	return ambientShading(color) + diffuseShading(color) + specularShading(color);
}




// wood
float invmax(float x) {
    return pow(x, (x) / (x - 1)) / (x - 1);
}

float ring(float x, float y) {
    return invmax(x) * (x - pow(y, x));
}

// Function to generate wood color based on 2D noise
vec3 generateWoodColor(vec2 uv) {
    int stripes = 15;
    vec3 baseColor = vec3(0.6, 0.4, 0.2);  // Base color of wood
    baseColor = baseColor * sin((floor(ring(uv.x, uv.y) * stripes) - (ring(uv.x, uv.y) * stripes) + 2) * 0.5);
    return baseColor;

    // :)
}

// glass
void generateGlass() {
    vec3 glassColor = vec3(0.6, 0.8, 1.0); // Light blue color for glass
    float shininess = 1000.0;                  // Adjust for specular highlight intensity
	float refractiveIndex = 1.5;

	vec3 shading = ambientShading(glassColor) + diffuseShading(glassColor); 
	vec3 refractDir = refract(-Eye, Normal, 1.0 / 1.5);

	if (!isnan(refractDir.x)) {
		float refract = pow(max(dot(refractDir, -Normal), 0.0), shininess);
		vec3 refracted = refract * glassColor;
		vec3 result = shading + specularShadingWithReflectance(glassColor) + refracted;		
		FragColor = vec4(result, 0.1);
	} else {
		//vec3 specular = spec * vec3(1.0);
		FragColor = vec4(1, 0, 0, 1); //vec4(specular, 1.0);
	}
}

void main() {
	vec3 baseColor;
	float alpha = 1;
	if (effect == 0) {
		vec3 wood = generateWoodColor(fragTexcoord);
		FragColor = vec4(BlinnPhongShading(wood), 1);
	// wood
	} else if (effect == 1) {
		generateGlass();
        return;
	// glass
	} else if (effect == 2) {
		baseColor = vec3(0.1, 0.8, 0.8);
		FragColor = vec4(BlinnPhongShading(baseColor), 1);
	}	else if (effect == 3) {
		baseColor = vec3(1.0, 0.8, 0.8);
		FragColor = vec4(BlinnPhongShading(baseColor), 1);
	}	
}