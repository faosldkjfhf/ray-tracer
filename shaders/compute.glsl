#version 460 core

layout(local_size_x = 10, local_size_y = 10, local_size_z = 1) in;

#define PI 3.14159265358979323846
#define MAX_BOUNCES 10

layout(rgba32f, binding = 0) uniform image2D imgOutput;
layout(location = 0) uniform vec3 u_CameraPosition;

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct Material {
    vec3 color;
    vec3 emissionColor;
    float emissionStrength;
};

struct Sphere {
    vec3 center;
    float radius;
};

struct Hit {
    float t;
    vec3 position;
    vec3 normal;
};

layout(std430, binding = 1) buffer SphereBuffer {
    Sphere spheres[];
};

float stepRngFloat(inout uint state) {
    state = state * 747796405 + 2891336453;
    uint word = ((state >> ((state >> 28) + 4)) ^ state) * 277803737;
    word = (word >> 22) ^ word;
    return float(word) / 4294967296.0f;
}

uint rngState = (600 * gl_GlobalInvocationID.x + gl_GlobalInvocationID.y);
float rand() {
    return stepRngFloat(rngState);
}

float rand(float min, float max) {
    return min + (max - min) * rand();
}

vec3 randomOnUnitSphere() {
    vec3 p = vec3(rand(-1.0, 1.0), rand(-1.0, 1.0), rand(-1.0, 1.0));
    return normalize(p);
}

vec3 randomOnHemisphere(vec3 normal) {
    vec3 inSphere = randomOnUnitSphere();
    return dot(inSphere, normal) > 0.0 ? inSphere : -inSphere;
}

bool intersectSphere(Ray ray, Sphere sphere, float tMin, float tMax, out Hit hit) {
    vec3 oc = sphere.center - ray.origin;
    float a = dot(ray.direction, ray.direction);
    float h = dot(oc, ray.direction);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;
    float discriminant = h * h - a * c;

    if (discriminant < 0.0) {
        return false;
    }

    float sqrtd = sqrt(discriminant);

    float t = (h - sqrtd) / a;
    if (t < tMin || t > tMax) {
        t = (h + sqrtd) / a;
        if (t < tMin || t > tMax) {
            return false;
        }
    }

    hit.t = t;
    hit.position = ray.origin + ray.direction * t;
    hit.normal = (hit.position - sphere.center) / sphere.radius;
    return true;
}

bool hitScene(Ray ray, out Hit hit) {
    float tMin = 0.001;
    float tMax = 1000.0;

    Hit tempHit;
    bool hitAnything = false;
    float closest = tMax;

    for (int i = 0; i < spheres.length(); i++) {
        if (intersectSphere(ray, spheres[i], tMin, closest, tempHit)) {
            hitAnything = true;
            closest = tempHit.t;
            hit = tempHit;
        }
    }

    return hitAnything;
}

vec3 rayColor(Ray ray) {
    Hit hit;

    vec3 finalColor = vec3(1.0);
    for (int i = 0; i < MAX_BOUNCES; i++) {
        if (hitScene(ray, hit)) {
            vec3 direction = randomOnHemisphere(hit.normal);
            ray.origin = hit.position;
            ray.direction = direction;
            finalColor *= 0.5 * (hit.normal + 1.0);
        } else {
            vec3 unitDirection = normalize(ray.direction);
            float t = 0.5 * (unitDirection.y + 1.0);
            finalColor *= (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
            break;
        }
    }

    return finalColor;
}

#define SAMPLES 100
void main() {
    vec2 imageSize = vec2(imageSize(imgOutput));

    float vfov = 90.0;
    float theta = vfov * PI / 180.0;
    float h = tan(theta / 2.0);
    float viewportHeight = 2.0 * h;
    float viewportWidth = imageSize.x / imageSize.y * viewportHeight;
    float focalLength = 1.0;

    // Position of the camera
    vec3 origin = u_CameraPosition;
    // Upper left corner of the viewport, (0,0) is at top left corner
    vec3 upperLeftCorner = origin - vec3(viewportWidth / 2.0, viewportHeight / 2.0, focalLength);

    // Get the pixel's position in the image
    vec2 uv = (gl_GlobalInvocationID.xy) / imageSize.xy;

    // anti-aliasing
    vec3 colorAccumulator = vec3(0.0);
    for (int i = 0; i < SAMPLES; i++) {
        vec2 offset = vec2(rand(-0.5, 0.5), rand(-0.5, 0.5));
        vec2 sampleUv = uv + offset / imageSize;
        Ray ray;
        ray.origin = origin;
        ray.direction = upperLeftCorner + vec3(sampleUv.x * viewportWidth, sampleUv.y * viewportHeight, 0.0) - origin;
        // Get the color of the pixel at where the ray intersects the scene
        colorAccumulator += rayColor(ray);
    }

    vec3 pixelColor = colorAccumulator / SAMPLES;

    vec4 value = vec4(pixelColor, 1.0);

    imageStore(imgOutput, ivec2(gl_GlobalInvocationID.xy), value);
}
