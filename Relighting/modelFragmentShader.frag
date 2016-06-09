#version 410 core

#define PI 3.1415926535897932384626433832795
#define INV_PI 0.31830988618379067153776752674503

in vec3 view_space_pos;
in vec3 view_space_normal;
in vec3 view_space_tangent;
in vec3 view_space_bitangent;

layout (location = 0) out vec4 fragColor;

uniform int mode;
uniform mat4 MV;
uniform samplerCube skybox;

uniform uint u_numberSamples;
uniform uint u_m;
uniform float u_binaryFractionFactor;

uniform float u_roughnessMaterial;
uniform float u_R0Material;

mat4 inv_MV;

// Generate 2d random samples.
vec2 hammersley(uint originalSample) {
    uint revertSample;

    // Revert bits by swapping blockwise. Lower bits are moved up and higher bits down.
    revertSample = (originalSample << 16u) | (originalSample >> 16u);
    revertSample = ((revertSample & 0x00ff00ffu) << 8u) | ((revertSample & 0xff00ff00u) >> 8u);
    revertSample = ((revertSample & 0x0f0f0f0fu) << 4u) | ((revertSample & 0xf0f0f0f0u) >> 4u);
    revertSample = ((revertSample & 0x33333333u) << 2u) | ((revertSample & 0xccccccccu) >> 2u);
    revertSample = ((revertSample & 0x55555555u) << 1u) | ((revertSample & 0xaaaaaaaau) >> 1u);

    // Shift back, as only m bits are used.
    revertSample = revertSample >> (32 - u_m);

    return vec2(float(revertSample) * u_binaryFractionFactor, float(originalSample) * u_binaryFractionFactor);
}

// The mapping from 2D random samples to points on the unit disk, (x, y).
// Then project them up to the unit sphere.
// see Physically Based Rendering Chapter 13.6.1 and 13.6.3
vec3 cosineWeightedSampling(vec2 e) {
    // UniformSampleDisk
    // Physically Based Rendering Chapter 13.6.2
    float r = sqrt(e.x);
    float theta = 2.0 * PI * e.y;
    float x = r * cos(theta);
    float y = r * sin(theta);

    // CosineSampleHemisphere
    // Physically Based Rendering Chapter 13.6.3
    float z = sqrt(max(0.0, 1 - x * x  - y * y));
    
    return vec3(x, y, z);
}

// One of the simplest BRDF model.
// https://en.wikipedia.org/wiki/Lambertian_reflectance
vec3 brdfLambert(vec3 N, vec3 L) {
    float NdotL = dot(N, L);


    vec3 L_in = texture(skybox, L).rgb;

    // L_out = BRDF * L_in * NdotL / PDF
    // BRDF is color / PI.
    // PDF is NdotL / PI.
    // NdotL and PI are canceled out, which results in this formula: Lo = color * L
    // L_out = u_colorMaterial * L_in;
    return L_in;
}

vec3 microfacetWeightedSampling(vec2 e) {
    float alpha = u_roughnessMaterial * u_roughnessMaterial;
    
    // Note: Polar Coordinates
    // x = sin(theta)*cos(phi)
    // y = sin(theta)*sin(phi)
    // z = cos(theta)
    
    float phi = 2.0 * PI * e.y;    
    float cosTheta = sqrt((1.0 - e.x) / (1.0 + (alpha*alpha - 1.0) * e.x));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta); 

    float x = sinTheta * cos(phi);
    float y = sinTheta * sin(phi);
    float z = cosTheta;

    return vec3(x, y, z);
}

// see http://en.wikipedia.org/wiki/Schlick%27s_approximation
float fresnelSchlick(float VdotH) {
    return u_R0Material + (1.0 - u_R0Material) * pow(1.0 - VdotH, 5.0);
}

// see http://graphicrants.blogspot.de/2013/08/specular-brdf-reference.html
// see http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
float geometricShadowingSchlickBeckmann(float NdotV, float k) {
    return NdotV / (NdotV * (1.0 - k) + k);
}

// see http://graphicrants.blogspot.de/2013/08/specular-brdf-reference.html
// see http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
float geometricShadowingSmith(float NdotL, float NdotV, float k) {
    return geometricShadowingSchlickBeckmann(NdotL, k) * geometricShadowingSchlickBeckmann(NdotV, k);
}

vec3 brdfCookTorrance(vec3 H, vec3 N, vec3 V, float k) {
    // Note: reflect takes incident vector.
    vec3 L = reflect(-V, H);
    // L = vec3(inv_MV * vec4(L, 0.0));

    float NdotL = dot(N, L);
    float NdotV = dot(N, V);
    float NdotH = dot(N, H);
    
    // Lighted and visible
    if (NdotL > 0.0 && NdotV > 0.0) {
        float VdotH = dot(V, H);

        // Fresnel reflect term
        float F = fresnelSchlick(VdotH);
        
        // Geometric attenuation term
        float G = geometricShadowingSmith(NdotL, NdotV, k);
        
        // Beckmann distribution factor
        // float temp = (NdotH * NdotH - 1) / (u_roughnessMaterial * u_roughnessMaterial * NdotH * NdotH);
        // float D = (exp(temp)) / (pow(u_roughnessMaterial, 2) * pow(NdotH,4.0));

        // Lo = BRDF * L * NdotL / PDF
        //
        // BRDF is D * F * G / (4 * NdotL * NdotV).
        // PDF is D * NdotH / (4 * VdotH).
        // D and 4 * NdotL are canceled out, which results in this formula: Lo = color * L * F * G * VdotH / (NdotV * NdotH)
        float colorFactor = F * G * VdotH / (NdotV * NdotH);
        
        // Note: Needed for robustness. With specific parameters, a NaN can be the result.
        if (isnan(colorFactor)) {
            return vec3(0.0, 0.0, 0.0);
        }

        return texture(skybox, L).rgb * colorFactor;
        // return vec3(255, 255, 255);
    }
    
    return vec3(0.0, 0.0, 0.0);
}

void main() {
    mat4 inv_MV = inverse(MV);

    if (mode == 1) {
        vec3 outgoing_direction;

        // Reflection
        outgoing_direction = reflect(normalize(view_space_pos), normalize(view_space_normal));
        // convert from eye to world space
        outgoing_direction = vec3(inv_MV * vec4(outgoing_direction, 0.0));

        fragColor = texture(skybox, outgoing_direction);

    } else if (mode == 2) {
        vec3 outgoing_direction;

        // Refraction
        outgoing_direction = refract(normalize(view_space_pos), normalize(view_space_normal), 0.658);
        // convert from eye to world space
        outgoing_direction = vec3(inv_MV * vec4(outgoing_direction, 0.0));

        fragColor = texture(skybox, outgoing_direction);

    } else if (mode == 3) {
        vec3 color = vec3(0.0, 0.0, 0.0);

        vec3 tangent = normalize(view_space_tangent);
        vec3 bitangent = normalize(view_space_bitangent);
        vec3 normal = normalize(view_space_normal);
        mat3 basis = mat3(tangent, bitangent, normal);

        for (uint sampleIndex = 0; sampleIndex < u_numberSamples; sampleIndex++) {
            vec2 randomPoint = hammersley(sampleIndex);

            // Transform L to world space.
            vec3 tangent_space_light = cosineWeightedSampling(randomPoint);
            vec3 view_space_light = basis * tangent_space_light;
            vec3 world_space_light = vec3(inv_MV * vec4(view_space_light, 0.0));

            vec3 world_space_normal = vec3(inv_MV * vec4(normalize(view_space_normal), 0.0));

            // Diffuse
            color += brdfLambert(world_space_normal, world_space_light);
        }

        fragColor = vec4(color / float(u_numberSamples), 1.0);

    } else if (mode == 4) {
        vec3 color = vec3(0.0, 0.0, 0.0);

        vec3 tangent = normalize(view_space_tangent);
        vec3 bitangent = normalize(view_space_bitangent);
        vec3 normal = normalize(view_space_normal);
        mat3 basis = mat3(tangent, bitangent, normal);

        float k = (u_roughnessMaterial + 1.0) * (u_roughnessMaterial + 1.0) / 8.0;

        // Incident direction
        // Only calculate in this way can make result correct.
        vec3 eye_direction = vec3(inv_MV * vec4(vec3(0, 0, 0) - view_space_pos, 0.0));

        for (uint sampleIndex = 0; sampleIndex < u_numberSamples; sampleIndex++) {
            vec2 randomPoint = hammersley(sampleIndex);

            // Transform L to world space.
            vec3 tangent_space_light = cosineWeightedSampling(randomPoint);
            vec3 view_space_light = basis * tangent_space_light;
            vec3 world_space_light = vec3(inv_MV * vec4(view_space_light, 0.0));

            vec3 world_space_normal = vec3(inv_MV * vec4(normalize(view_space_normal), 0.0));

            // Diffuse
            color += brdfLambert(world_space_normal, world_space_light);

            // Transform H to world space.
            vec3 tangent_space_H = microfacetWeightedSampling(randomPoint);
            vec3 view_space_H = basis * tangent_space_H;
            vec3 world_space_H = vec3(inv_MV * vec4(view_space_H, 0.0));

            // Specular
            color += brdfCookTorrance(world_space_H, world_space_normal, eye_direction, k);
        }

        fragColor = vec4(color / float(u_numberSamples), 1.0);
    }
}