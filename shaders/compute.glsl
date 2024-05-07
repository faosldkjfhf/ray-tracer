#version 460 core

layout(local_size_x = 10, local_size_y = 10, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D imgOutput;

struct Ray {
    vec3 origin;
    vec3 direction;
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

uniform vec3 u_CameraPosition;

Sphere[2] spheres = Sphere[2](
        Sphere(vec3(0.0, 0.0, -1.0), 0.5),
        Sphere(vec3(0.0, -100.5, -1.0), 100.0)
    );

float rand(vec2 co) {
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 randomInUnitSphere() {
    vec3 p;
    do {
        p = 2.0 * vec3(rand(gl_GlobalInvocationID.xy)) - vec3(1.0);
    } while (dot(p, p) >= 1.0);
    return p;
}

vec3 randomOnHemisphere(vec3 normal) {
    vec3 inUnitSphere = normalize(randomInUnitSphere());
    if (dot(inUnitSphere, normal) > 0.0) {
        return inUnitSphere;
    } else {
        return -inUnitSphere;
    }
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

vec3 color(Ray ray) {
    Hit hit;

    vec3 color = vec3(1.0);
    for (int i = 0; i < 10; i++) {
        if (hitScene(ray, hit)) {
            vec3 direction = randomOnHemisphere(hit.normal);
            ray.origin = hit.position;
            ray.direction = direction;
            color += 0.25 * (hit.normal + 1.0);
        } else {
            vec3 unitDirection = normalize(ray.direction);
            float t = 0.5 * (unitDirection.y + 1.0);
            vec3 result = (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
            color *= result;
            return color;
        }
    }

    return color / 10.0;

    vec3 unitDirection = normalize(ray.direction);
    float t = 0.5 * (unitDirection.y + 1.0);
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

void main() {
    vec2 imageSize = vec2(imageSize(imgOutput));

    float vfov = 90.0;
    float theta = vfov * 3.14159265358979323846 / 180.0;
    float h = tan(theta / 2.0);
    float viewportHeight = 2.0 * h;
    float viewportWidth = imageSize.x / imageSize.y * viewportHeight;

    // Position of the camera
    vec3 origin = u_CameraPosition;
    // Upper left corner of the viewport, (0,0) is at top left corner
    vec3 upperLeftCorner = origin + vec3(-viewportWidth / 2.0, -viewportHeight / 2.0, -1.0);

    // Get the pixel's position in the image
    vec2 uv = (gl_GlobalInvocationID.xy) / imageSize.xy;

    // Shoot a ray from the camera's position to the pixel on the screen
    // Offset is for each work group
    // Ray ray = Ray(origin,
    //         upperLeftCorner + vec3(uv.x * viewportWidth, uv.y * viewportHeight, 0.0) - origin);

    const int samples = 100;

    // anti-aliasing
    vec3 colorAccumulator = vec3(0.0);
    for (int i = 0; i < samples; i++) {
        vec2 offset = vec2(rand(vec2(gl_GlobalInvocationID.xy + i)), rand(vec2(gl_GlobalInvocationID.xy + i + 1)));
        vec2 uvSample = uv + offset / imageSize;
        Ray ray;
        ray.origin = origin;
        ray.direction = upperLeftCorner + vec3(uvSample.x * viewportWidth, uvSample.y * viewportHeight, 0.0) - origin;
        colorAccumulator += color(ray);
    }

    vec3 pixelColor = colorAccumulator / float(samples);
    // pixelColor = vec3(sqrt(pixelColor.r), sqrt(pixelColor.g), sqrt(pixelColor.b));

    // Get the color of the pixel at where the ray intersects the scene
    // vec3 pixelColor = color(ray);

    vec4 value = vec4(pixelColor, 1.0);

    imageStore(imgOutput, ivec2(gl_GlobalInvocationID.xy), value);
}
