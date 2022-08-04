#version 130

vec3 rgb_to_hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv_to_rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

in vec4 _vertex_color;
in vec2 _texture_coordinates;
in vec3 _vertex_position;

out vec4 _fragment_color;

uniform int _texture_set;
uniform sampler2D _texture;

uniform vec2 _canvas_size;

uniform vec4 _current_color_hsva;
uniform float _hue_offset;

void main()
{
    vec3 current_hsv = _current_color_hsva.xyz;
    vec2 pos = (_vertex_position.xy + vec2(1)) / 2;
    pos.x -= (0.5 - _hue_offset);

    const float cursor_width = 0.01;
    float cursor_frame_width = 1.f / _canvas_size.x;


    vec3 hsv = vec3(pos.x, current_hsv.y, current_hsv.z);
    _fragment_color = vec4(hsv_to_rgb(hsv), 1);
}