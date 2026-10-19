#version 440 core

layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;

layout(binding = 1) uniform sampler2D source;
layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    float radius;
    float width;
    float height;
} ubuf;

// 有符号距离场：圆角矩形
float sdRoundedBox(vec2 p, vec2 b, vec4 r)
{
    // 选择当前象限对应的圆角半径：
    // r.x = 左上, r.y = 右上, r.z = 右下, r.w = 左下
    float radius = (p.x < 0.0)
        ? ((p.y < 0.0) ? r.w : r.x)
        : ((p.y < 0.0) ? r.z : r.y);

    vec2 q = abs(p);
    vec2 bMinusR = b - vec2(radius);
    vec2 offset = max(q - bMinusR, vec2(0.0));
    return length(offset) - radius;
}

void main()
{
    vec4 tex = texture(source, qt_TexCoord0);
    vec2 sourceSize = vec2(ubuf.width, ubuf.height);
    // 纹理坐标转像素坐标（中心为原点）
    vec2 pixelCoord = (qt_TexCoord0 - 0.5) * sourceSize;
    
    // 计算到圆角矩形边界的距离
    vec2 halfSize = sourceSize * 0.5;
    vec4 cornerRadius = vec4(ubuf.radius); // 可分别设置四个角
    
    float d = sdRoundedBox(pixelCoord, halfSize, cornerRadius);
    float alpha = 1.0 - smoothstep(-1.0, 1.0, d);
    fragColor = tex * ubuf.qt_Opacity * alpha;

    // // 将 d 映射到 0..1, 并量化为 16 个等级
    // float mapped = clamp((d / max(halfSize.x, halfSize.y)) * 0.5 + 0.5, 0.0, 1.0);
    // float quantized = floor(mapped * 16.0 + 0.5) / 16.0;
    
    // // 内部（d<0）使用蓝色调，外部（d>=0）使用红色调
    // vec3 color = mix(vec3(1.0, 0.2, 0.2), vec3(0.2, 0.4, 1.0), step(d, 0.0));
    // fragColor = vec4(color * quantized, 1.0);
}