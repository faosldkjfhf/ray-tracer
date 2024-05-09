#version 460 core

layout(local_size_x = 10, local_size_y = 10, local_size_z = 1) in;

#define PI 3.14159265358979323846
#define MAX_BOUNCES 10

layout(rgba32f, binding = 0) uniform image2D imgOutput;
layout(location = 0) uniform vec3 u_CameraPosition;

uniform int u_NumSpheres;
uniform int u_NumTriangles;

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
    int material;
};

struct Triangle {
    vec3 v0;
    vec3 v1;
    vec3 v2;
    int material;
};

struct Mesh {
    uint vertexOffset;
    uint vertexCount;
    uint triangleOffset;
    uint triangleCount;
};

struct Hit {
    float t;
    vec3 position;
    vec3 normal;
    bool frontFace;
    Material material;
};

layout(std430, binding = 1) buffer SphereBuffer {
    Sphere spheres[];
};

// layout (std430, binding = 2) buffer VertexBuffer {
//     vec3 vertices[];
// };

layout(std430, binding = 2) buffer TriangleBuffer {
    Triangle triangles[];
};

layout(std430, binding = 3) buffer MaterialBuffer {
    Material materials[];
};

// layout(std430, binding = 4) buffer MeshBuffer {
//     Mesh meshes[];
// };

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

void setHitFaceNormal(inout Hit hit, Ray ray, vec3 outwardNormal) {
    hit.frontFace = dot(ray.direction, outwardNormal) < 0.0;
    hit.normal = hit.frontFace ? outwardNormal : -outwardNormal;
}

bool hitSphere(Ray ray, Sphere sphere, float tMin, float tMax, out Hit hit) {
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
    hit.material = materials[sphere.material];
    hit.normal = (hit.position - sphere.center) / sphere.radius;
    setHitFaceNormal(hit, ray, hit.normal);
    return true;
}

bool hitTriangle(Ray ray, Triangle triangle, float tMin, float tMax, out Hit hit) {
    // Check if ray intersects the plane of the triangle
    vec3 v0v1 = triangle.v1 - triangle.v0;
    vec3 v1v2 = triangle.v2 - triangle.v1;
    vec3 v2v0 = triangle.v0 - triangle.v2;
    vec3 normal = cross(v0v1, v1v2);

    float nDotV0MinusO = dot(normal, triangle.v0 - ray.origin);
    float nDotD = dot(normal, ray.direction);
    float t = nDotV0MinusO / nDotD;
    vec3 intersectPoint = ray.origin + ray.direction * t;

    if (t < tMin || t > tMax) {
        return false;
    }

    // Check if the intersection point is inside the triangle
    float dotCross0 = dot(cross(v0v1, intersectPoint - triangle.v0), normal);
    float dotCross1 = dot(cross(v1v2, intersectPoint - triangle.v1), normal);
    float dotCross2 = dot(cross(v2v0, intersectPoint - triangle.v2), normal);

    if ((dotCross0 >= 0.0 && dotCross1 >= 0.0 && dotCross2 >= 0.0) ||
            (dotCross0 <= 0.0 && dotCross1 <= 0.0 && dotCross2 <= 0.0)) {
        hit.t = t;
        hit.position = intersectPoint;
        hit.normal = normal;
        hit.material = materials[triangle.material];
        setHitFaceNormal(hit, ray, hit.normal);
        return true;
    }

    return false;
}

bool hitScene(Ray ray, out Hit hit) {
    float tMin = 0.001;
    float tMax = 1000.0;

    Hit tempHit;
    bool hitAnything = false;
    float closest = tMax;

    for (int i = 0; i < spheres.length(); i++) {
        if (hitSphere(ray, spheres[i], tMin, closest, tempHit)) {
            hitAnything = true;
            closest = tempHit.t;
            hit = tempHit;
        }
    }

    for (int i = 0; i < triangles.length(); i++) {
        if (hitTriangle(ray, triangles[i], tMin, closest, tempHit)) {
            hitAnything = true;
            closest = tempHit.t;
            hit = tempHit;
        }
    }

    return hitAnything;
}

vec3 rayColor(Ray ray) {
    Hit hit;

    vec3 finalColor = vec3(0.0);
    vec3 accumulatedColor = vec3(1.0);
    for (int i = 0; i < MAX_BOUNCES; i++) {
        if (hitScene(ray, hit)) {
            // Update the ray
            vec3 direction = hit.normal + randomOnUnitSphere();
            ray.origin = hit.position;
            ray.direction = normalize(direction);

            // Accumulate color based on the material that was hit
            Material mat = hit.material;
            vec3 emittedLight = mat.emissionColor * mat.emissionStrength;
            finalColor += emittedLight * accumulatedColor;
            accumulatedColor *= mat.color;
        } else {
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
