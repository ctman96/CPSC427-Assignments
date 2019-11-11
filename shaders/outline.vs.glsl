#version 330

// Input attributes
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_translate;
layout (location = 2) in vec2 in_scale;
layout (location = 3) in float in_rotation;
layout (location = 4) in vec4 in_color;

out vec4 vcolor;

// Application data
uniform mat3 projection;

void main()
{
    float c = cos(in_rotation);
    float s = sin(in_rotation);

    mat3 transform = mat3(
    in_scale.x * c, in_scale.y * s, 0.0,
    -in_scale.x * s, in_scale.y * c, 0.0,
    in_translate.x, in_translate.y, 1.0);

    vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
    gl_Position = vec4(pos.xy, in_position.z, 1.0);
    vcolor = in_color;
}
