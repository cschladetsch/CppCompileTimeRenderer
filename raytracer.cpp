#include <array>
#include <algorithm>
#include <cstddef>
#include <type_traits>
#include <utility>

namespace rt {

constexpr std::size_t kWidth = 80;
constexpr std::size_t kHeight = 40;

struct Vec3 {
    float x;
    float y;
    float z;

    constexpr Vec3 operator+(const Vec3& other) const {
        return {x + other.x, y + other.y, z + other.z};
    }
    constexpr Vec3 operator-(const Vec3& other) const {
        return {x - other.x, y - other.y, z - other.z};
    }
    constexpr Vec3 operator*(float s) const { return {x * s, y * s, z * s}; }
    constexpr Vec3 operator/(float s) const { return {x / s, y / s, z / s}; }
    constexpr Vec3 operator-() const { return {-x, -y, -z}; }
};

constexpr Vec3 operator*(float s, const Vec3& v) { return v * s; }

constexpr float dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

constexpr Vec3 cross(const Vec3& a, const Vec3& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
    };
}

constexpr float csqrt(float x) {
    if (x <= 0.0f) {
        return 0.0f;
    }
    float guess = x;
    for (int i = 0; i < 24; ++i) {
        guess = 0.5f * (guess + x / guess);
    }
    return guess;
}

constexpr float length(const Vec3& v) { return csqrt(dot(v, v)); }

constexpr Vec3 normalize(const Vec3& v) {
    float len = length(v);
    return (len > 0.0f) ? (v / len) : Vec3{0.0f, 0.0f, 0.0f};
}

constexpr float powi(float base, int exp) {
    float result = 1.0f;
    for (int i = 0; i < exp; ++i) {
        result *= base;
    }
    return result;
}

struct Ray {
    Vec3 origin;
    Vec3 dir;
};

struct Sphere {
    Vec3 center;
    float radius;
    float albedo;
    float specular;
};

struct HitInfo {
    float t;
    Vec3 point;
    Vec3 normal;
    float albedo;
    float specular;
};

constexpr bool hit_sphere(const Sphere& s, const Ray& ray, float t_min, float t_max, HitInfo& out) {
    Vec3 oc = ray.origin - s.center;
    float a = dot(ray.dir, ray.dir);
    float b = 2.0f * dot(oc, ray.dir);
    float c = dot(oc, oc) - s.radius * s.radius;
    float discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0.0f) {
        return false;
    }
    float sqrt_d = csqrt(discriminant);
    float t = (-b - sqrt_d) / (2.0f * a);
    if (t < t_min || t > t_max) {
        t = (-b + sqrt_d) / (2.0f * a);
        if (t < t_min || t > t_max) {
            return false;
        }
    }
    out.t = t;
    out.point = ray.origin + ray.dir * t;
    out.normal = normalize(out.point - s.center);
    out.albedo = s.albedo;
    out.specular = s.specular;
    return true;
}

constexpr float clamp01(float v) { return std::clamp(v, 0.0f, 1.0f); }

constexpr float shade(const HitInfo& hit, const Vec3& light_pos, const Vec3& light_color,
                      const std::array<Sphere, 5>& spheres) {
    Vec3 to_light = light_pos - hit.point;
    float light_dist = length(to_light);
    Vec3 light_dir = normalize(to_light);

    Ray shadow_ray{hit.point + hit.normal * 0.001f, light_dir};
    HitInfo shadow_hit{};
    for (const auto& s : spheres) {
        if (hit_sphere(s, shadow_ray, 0.001f, light_dist - 0.002f, shadow_hit)) {
            return 0.05f;
        }
    }

    float diff = std::max(0.0f, dot(hit.normal, light_dir));
    Vec3 view_dir = normalize(-hit.point);
    Vec3 reflect_dir = normalize(2.0f * dot(hit.normal, light_dir) * hit.normal - light_dir);
    float spec = powi(std::max(0.0f, dot(view_dir, reflect_dir)), 32) * hit.specular;

    float light = (diff * hit.albedo + spec) * (0.6f * light_color.x + 0.3f * light_color.y + 0.1f * light_color.z);
    return clamp01(light + 0.05f);
}

constexpr float trace(const Ray& ray, const std::array<Sphere, 5>& spheres,
                      const Vec3& light_pos, const Vec3& light_color) {
    HitInfo hit{};
    bool has_hit = false;
    float closest = 1e9f;
    for (const auto& s : spheres) {
        HitInfo tmp{};
        if (hit_sphere(s, ray, 0.001f, closest, tmp)) {
            has_hit = true;
            closest = tmp.t;
            hit = tmp;
        }
    }

    if (!has_hit) {
        float t = 0.5f * (ray.dir.y + 1.0f);
        return clamp01(0.15f + 0.35f * (1.0f - t));
    }

    return shade(hit, light_pos, light_color, spheres);
}

constexpr char brightness_to_char(float b) {
    constexpr const char* palette = " .:-=+*#%@";
    int index = static_cast<int>(b * 9.99f);
    index = std::clamp(index, 0, 9);
    return palette[index];
}

constexpr std::array<std::array<char, kWidth + 1>, kHeight> render_ascii() {
    std::array<std::array<char, kWidth + 1>, kHeight> lines{};

    constexpr Vec3 camera_pos{0.0f, 0.0f, 0.0f};
    constexpr float viewport_height = 2.0f;
    constexpr float viewport_width = viewport_height * (static_cast<float>(kWidth) / static_cast<float>(kHeight));
    constexpr float focal_length = 1.5f;

    constexpr Vec3 horizontal{viewport_width, 0.0f, 0.0f};
    constexpr Vec3 vertical{0.0f, viewport_height, 0.0f};
    constexpr Vec3 lower_left = camera_pos - horizontal / 2.0f - vertical / 2.0f + Vec3{0.0f, 0.0f, -focal_length};

    constexpr std::array<Sphere, 5> spheres{ {
        {{0.0f, -100.5f, -2.5f}, 100.0f, 0.35f, 0.0f},
        {{-0.9f, -0.2f, -2.0f}, 0.5f, 0.9f, 0.2f},
        {{0.7f, 0.0f, -2.8f}, 0.7f, 0.8f, 0.4f},
        {{1.6f, -0.1f, -1.8f}, 0.35f, 0.95f, 0.8f},
        {{-1.8f, 0.3f, -3.2f}, 0.9f, 0.7f, 0.1f},
    } };

    constexpr Vec3 light_pos{2.5f, 3.0f, -1.5f};
    constexpr Vec3 light_color{1.0f, 0.9f, 0.8f};

    for (std::size_t y = 0; y < kHeight; ++y) {
        float v = static_cast<float>(kHeight - 1 - y) / static_cast<float>(kHeight - 1);
        for (std::size_t x = 0; x < kWidth; ++x) {
            float u = static_cast<float>(x) / static_cast<float>(kWidth - 1);
            Vec3 dir = lower_left + u * horizontal + v * vertical - camera_pos;
            Ray ray{camera_pos, normalize(dir)};
            float brightness = trace(ray, spheres, light_pos, light_color);
            lines[y][x] = brightness_to_char(brightness);
        }
        lines[y][kWidth] = '\0';
    }

    return lines;
}

struct Image {
    static constexpr auto lines = render_ascii();
};

template <char... Cs>
struct LineTag {};

template <typename>
struct LineFromSeq;

template <char... Cs>
struct LineFromSeq<std::integer_sequence<char, Cs...>> {
    using type = LineTag<Cs...>;
};

template <std::size_t I, std::size_t... Js>
consteval auto make_line_seq(std::index_sequence<Js...>) {
    return std::integer_sequence<char, Image::lines[I][Js]...>{};
}

template <std::size_t I>
using LineType = typename LineFromSeq<decltype(make_line_seq<I>(std::make_index_sequence<kWidth>{}))>::type;

template <typename>
struct AlwaysFalseT : std::false_type {};

template <std::size_t I>
struct LineAssert {
    static_assert(AlwaysFalseT<LineType<I>>::value);
};

template <std::size_t... Is>
struct EmitLines : LineAssert<Is>... {};

template <typename>
struct EmitFromSeq;

template <std::size_t... Is>
struct EmitFromSeq<std::index_sequence<Is...>> {
    using type = EmitLines<Is...>;
};

using Trigger = EmitFromSeq<std::make_index_sequence<kHeight>>::type;
constexpr bool kTrigger = sizeof(Trigger) > 0;

}  // namespace rt

int main() { return rt::kTrigger; }
