#version 460 core

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

#define PI 3.14159265358979323846
#define MAX_BOUNCES 10

layout(rgba32f, binding = 0) uniform image2D imgOutput;
layout(location = 0) uniform vec3 u_CameraPosition;
layout(location = 1) uniform vec3 u_CameraDirection;
layout(location = 2) uniform vec3 u_CameraUp;
layout(location = 3) uniform uint u_FrameCount;

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct Hit {
    float t;
    vec3 position;
    vec3 normal;
    bool frontFace;
    uint materialIdx;
};

struct ONB {
    vec3 u;
    vec3 v;
    vec3 w;
};

#define LAMBERTIAN 0
#define METAL 1
#define DIELECTRIC 2
#define LIGHT 3

struct Material {
    vec3 albedo;
    uint type;
    float typeData; // Metal: fuzziness; Glass: refraction index; Light: emission strength
};

#define TYPE_FACE 0
#define TYPE_SPHERE 1

struct Object {
    vec4 data; // Sphere: center, radius; Face: v0, v1, v2, empty
    uint type; // 0: Triangle/Face, 1: Sphere
    uint materialIdx;
};

struct BVHNode {
    vec3 aabbMin;
    vec3 aabbMax;
    int leftChild;
    int firstObject;
    int numObjects;
};

layout(std430, binding = 1) buffer ObjectBuffer {
    Object objects[];
};

layout(std430, binding = 2) buffer VertexBuffer {
    vec3 vertices[];
};

layout(std430, binding = 3) buffer MaterialBuffer {
    Material materials[];
};

layout(std430, binding = 4) buffer BVHBuffer {
    BVHNode bvh[];
};

float stepRngFloat(inout uint state) {
    state = state * 747796405 + 2891336453;
    uint word = ((state >> ((state >> 28) + 4)) ^ state) * 277803737;
    word = (word >> 22) ^ word;
    return float(word) / 4294967296.0f;
}

uint rngState = (600 * gl_GlobalInvocationID.x + gl_GlobalInvocationID.y) * (u_FrameCount + 1);
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

// Random value in normal distribution with mean 0 and standard deviation 1
float randomNormalDistribution() {
    float theta = 2.0 * PI * rand();
    float rho = sqrt(-2.0 * log(rand()));
    return rho * cos(theta);
}

void setHitFaceNormal(inout Hit hit, Ray ray, vec3 outwardNormal) {
    hit.frontFace = dot(ray.direction, outwardNormal) < 0.0;
    hit.normal = hit.frontFace ? outwardNormal : -outwardNormal;
}

vec2 intersectAABB(Ray ray, vec3 boxMin, vec3 boxMax, out float t) {
    vec3 tMin = (boxMin - ray.origin) / ray.direction;
    vec3 tMax = (boxMax - ray.origin) / ray.direction;
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);
    float tNear = max(max(t1.x, t1.y), t1.z);
    float tFar = min(min(t2.x, t2.y), t2.z);
    return vec2(tNear, tFar);
}

bool hitSphere(Ray ray, Object sphere, float tMin, float tMax, out Hit hit) {
    vec3 oc = sphere.data.xyz - ray.origin;
    float a = dot(ray.direction, ray.direction);
    float h = dot(oc, ray.direction);
    float c = dot(oc, oc) - sphere.data.w * sphere.data.w;
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
    hit.materialIdx = sphere.materialIdx;
    hit.normal = (hit.position - sphere.data.xyz) / sphere.data.w;
    setHitFaceNormal(hit, ray, hit.normal);
    return true;
}

bool hitFace(Ray ray, Object face, float tMin, float tMax, out Hit hit) {
    // Check if ray intersects the plane of the triangle
    vec3 v0 = vertices[int(face.data.x)];
    vec3 v1 = vertices[int(face.data.y)];
    vec3 v2 = vertices[int(face.data.z)];
    vec3 v0v1 = v1 - v0;
    vec3 v1v2 = v2 - v1;
    vec3 v2v0 = v0 - v2;
    vec3 normal = cross(v0v1, v1v2);

    float nDotV0MinusO = dot(normal, v0 - ray.origin);
    float nDotD = dot(normal, ray.direction);
    float t = nDotV0MinusO / nDotD;
    vec3 intersectPoint = ray.origin + ray.direction * t;

    if (t < tMin || t > tMax) {
        return false;
    }

    // Check if the intersection point is inside the triangle
    float dotCross0 = dot(cross(v0v1, intersectPoint - v0), normal);
    float dotCross1 = dot(cross(v1v2, intersectPoint - v1), normal);
    float dotCross2 = dot(cross(v2v0, intersectPoint - v2), normal);

    if ((dotCross0 >= 0.0 && dotCross1 >= 0.0 && dotCross2 >= 0.0) ||
            (dotCross0 <= 0.0 && dotCross1 <= 0.0 && dotCross2 <= 0.0)) {
        hit.t = t;
        hit.position = intersectPoint;
        hit.normal = normalize(normal);
        hit.materialIdx = face.materialIdx;
        setHitFaceNormal(hit, ray, hit.normal);
        return true;
    }

    return false;
}

// bool hitScene(Ray ray, out Hit hit) {
//     float tMin = 0.001;
//     float tMax = 1000.0;
//
//     Hit tempHit;
//     bool hitAnything = false;
//     float closest = tMax;
//
//     for (int i = 0; i < spheres.length(); i++) {
//         if (hitSphere(ray, spheres[i], tMin, closest, tempHit)) {
//             hitAnything = true;
//             closest = tempHit.t;
//             hit = tempHit;
//         }
//     }
//
//     for (int i = 0; i < faces.length(); i++) {
//         if (hitFace(ray, faces[i], tMin, closest, tempHit)) {
//             hitAnything = true;
//             closest = tempHit.t;
//             hit = tempHit;
//         }
//     }
//
//     return hitAnything;
// }

bool hitBvh(Ray ray, out Hit hit) {
    float tMin = 0.001;
    float tMax = 5000.0;

    Hit tempHit;
    bool hitAnything = false;
    float closest = tMax;

    int stack[64];
    int stackSize = 0;
    stack[stackSize++] = 0;

    while (stackSize > 0 && stackSize < 64) {
        int nodeIdx = stack[--stackSize];
        BVHNode node = bvh[nodeIdx];

        vec2 tIntersect = intersectAABB(ray, node.aabbMin, node.aabbMax, tempHit.t);
        if (tIntersect.y < tIntersect.x) {
            continue;
        }

        if (node.numObjects > 0) {
            for (int i = 0; i < node.numObjects; i++) {
                int objectIdx = node.firstObject + i;
                Object obj = objects[objectIdx];
                if (obj.type == TYPE_FACE) {
                    if (hitFace(ray, obj, tMin, closest, tempHit)) {
                        hitAnything = true;
                        closest = tempHit.t;
                        hit = tempHit;
                    }
                } else if (obj.type == TYPE_SPHERE) {
                    if (hitSphere(ray, obj, tMin, closest, tempHit)) {
                        hitAnything = true;
                        closest = tempHit.t;
                        hit = tempHit;
                    }
                }
            }
        }

        if (node.leftChild != -1) {
            stack[stackSize++] = node.leftChild;
            stack[stackSize++] = node.leftChild + 1;
        }
    }

    return hitAnything;
}

vec3 reflect(vec3 v, vec3 n) {
    return v - 2.0 * dot(v, n) * n;
}

vec3 refract(vec3 uv, vec3 n, float etaiOverEtat) {
    float cosTheta = min(dot(-uv, n), 1.0);
    vec3 rOutPerp = etaiOverEtat * (uv + cosTheta * n);
    vec3 rOutParallel = -sqrt(1.0 - dot(rOutPerp, rOutPerp)) * n;
    return rOutPerp + rOutParallel;
}

float reflectance(float cosine, float refIdx) {
    float r0 = (1.0 - refIdx) / (1.0 + refIdx);
    r0 = r0 * r0;
    return r0 + (1.0 - r0) * pow((1.0 - cosine), 5.0);
}

bool scatter(Hit hit, inout vec3 albedo, inout Ray scattered) {
    albedo = materials[hit.materialIdx].albedo;
    scattered.origin = hit.position;

    uint type = materials[hit.materialIdx].type;
    if (type == LAMBERTIAN) {
        scattered.direction = hit.normal + randomOnUnitSphere();
        if (length(scattered.direction) < 0.0001) {
            scattered.direction = hit.normal;
        }
    } else if (type == METAL) {
        scattered.direction = reflect(scattered.direction, hit.normal);
        scattered.direction += materials[hit.materialIdx].typeData * randomOnUnitSphere();
    } else if (type == DIELECTRIC) {
        float refractionIndex = materials[hit.materialIdx].typeData;
        float ri = hit.frontFace ? 1.0 / refractionIndex : refractionIndex;
        float cosTheta = min(dot(-scattered.direction, hit.normal), 1.0);
        float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

        bool cannotRefract = ri * sinTheta > 1.0;
        if (cannotRefract || reflectance(cosTheta, ri) > rand()) {
            scattered.direction = reflect(scattered.direction, hit.normal);
        } else {
            scattered.direction = refract(scattered.direction, hit.normal, ri);
        }
    } else if (type == LIGHT) {
        albedo *= materials[hit.materialIdx].typeData;
        return false;
    }

    return true;
}

vec3 rayColor(Ray ray) {
    Hit hit;

    vec3 finalColor = vec3(1.0);
    for (int i = 0; i < MAX_BOUNCES; i++) {
        if (hitBvh(ray, hit)) {
            vec3 albedo;
            bool scatters = scatter(hit, albedo, ray);
            finalColor *= albedo;
            if (!scatters) {
                break;
            }
        } else {
            finalColor *= 0.0;
            break;
        }
    }

    return finalColor;
}

ONB createONB(vec3 vec, vec3 up) {
    ONB onb;
    onb.w = normalize(vec);
    onb.u = normalize(cross(onb.w, up));
    onb.v = cross(onb.u, onb.w);
    return onb;
}

#define SAMPLES 10
void main() {
    vec2 imageSize = vec2(imageSize(imgOutput));

    float vfov = 40.0;
    float theta = vfov * PI / 180.0;
    float h = tan(theta / 2.0);
    float viewportHeight = 2.0 * h;
    float viewportWidth = imageSize.x / imageSize.y * viewportHeight;
    float focalLength = 1.0;

    // Calculate the uvw basis
    ONB onb = createONB(u_CameraDirection, u_CameraUp);

    // Calculate the vectors across the horizontal and vertical viewport edges
    vec3 horizontal = onb.u * viewportWidth;
    vec3 vertical = onb.v * viewportHeight;

    // Position of the camera
    vec3 origin = u_CameraPosition;

    // Upper left corner of the viewport, (0,0) is at top left corner
    vec3 upperLeftCorner = origin - (horizontal / 2.0) - (vertical / 2.0) + (focalLength * onb.w);

    // Get the pixel's position in the image
    vec2 uv = (gl_GlobalInvocationID.xy) / imageSize.xy;

    // anti-aliasing
    vec3 colorAccumulator = vec3(0.0);
    for (int i = 0; i < SAMPLES; i++) {
        vec2 offset = vec2(rand(-0.5, 0.5), rand(-0.5, 0.5));
        vec2 sampleUv = uv + offset / imageSize;
        Ray ray;
        ray.origin = origin;
        ray.direction = upperLeftCorner + sampleUv.x * horizontal + sampleUv.y * vertical - origin;
        // Get the color of the pixel at where the ray intersects the scene
        colorAccumulator += rayColor(ray);
    }

    vec3 pixelColor = colorAccumulator / SAMPLES;

    vec4 oldColor = imageLoad(imgOutput, ivec2(gl_GlobalInvocationID.xy)).rgba * min(1.0, u_FrameCount);
    vec4 finalColor = (oldColor * u_FrameCount + vec4(pixelColor, 1.0)) / (u_FrameCount + 1.0);

    imageStore(imgOutput, ivec2(gl_GlobalInvocationID.xy), finalColor);
}
