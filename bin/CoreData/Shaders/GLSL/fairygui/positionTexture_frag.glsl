#ifdef GL_ES
precision lowp float;
#endif

varying vec2 v_texCoord;

uniform sampler2D u_texture;

void main()
{
    gl_FragColor =  texture2D(u_texture, v_texCoord);
}

// Texture2D    u_texture;
// SamplerState u_texture_sampler; // By convention, texture samplers must use the '_sampler' suffix

// struct PSInput
// {
//     float4 Pos          : SV_POSITION;
//     float2 v_texCoord   : TEX_COORD;
// };

// struct PSOutput
// {
//     float4 Color : SV_TARGET;
// };

// void main(in PSInput PSIn, out PSOutput PSOut)
// {
//     PSOut.Color = u_texture.Sample(u_texture_sampler, PSIn.v_texCoord);
// }
