#version 330 core

in vec2 fragTexcoord;  // Input texture coordinates
out vec4 FragColor;  // Output fragment color

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


void main() {
    vec3 woodColor = generateWoodColor(fragTexcoord);

    FragColor = vec4(woodColor, 1.0);
}