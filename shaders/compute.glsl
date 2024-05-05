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

Sphere sphere = Sphere(vec3(0.0, 0.0, -1.0), 0.5);

bool intersectSphere(Ray ray, Sphere sphere, out Hit hit) {
    vec3 oc = sphere.center - ray.origin;
    float a = dot(ray.direction, ray.direction);
    float h = dot(oc, ray.direction);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;
    float discriminant = h * h - a * c;

    if (discriminant >= 0.0) {
        float t = (h - sqrt(discriminant)) / a;
        if (t > 0.0) {
            hit.t = t;
            hit.position = ray.origin + ray.direction * t;
            hit.normal = (hit.position - sphere.center) / sphere.radius;
            return true;
        }
    }

    return false;
}

vec3 color(Ray ray) {
    Hit hit;
    if (intersectSphere(ray, sphere, hit)) {
        return 0.5 * (hit.normal + 1.0);
    }

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
    vec3 origin = vec3(0.0, 0.0, 0.0);
    // Upper left corner of the viewport, (0,0) is at top left corner
    vec3 upperLeftCorner = vec3(-viewportWidth / 2.0, -viewportHeight / 2.0, -1.0);

    // Get the pixel's position in the image
    vec2 uv = (gl_GlobalInvocationID.xy) / imageSize.xy;

    // Shoot a ray from the camera's position to the pixel on the screen
    // Offset is for each work group
    Ray ray = Ray(origin,
            upperLeftCorner + vec3(uv.x * viewportWidth, uv.y * viewportHeight, 0.0) - origin);

    // Get the color of the pixel at where the ray intersects the scene
    vec3 pixelColor = color(ray);

    vec4 value = vec4(pixelColor, 1.0);

    imageStore(imgOutput, ivec2(gl_GlobalInvocationID.xy), value);
}
