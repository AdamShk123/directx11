Texture2D mytexture : register(t0);
SamplerState mysampler : register(s0);

struct PixelInput
{
    float3 color : COLOR;
    float4 position : SV_Position;
    float2 tex : TEXCOORD;
};

struct PixelOutput
{
    float4 attachment0 : SV_Target0;
};

PixelOutput main(PixelInput pixelInput)
{
    PixelOutput output;
    // output.attachment0 = float4(pixelInput.color, 0.0f);
    output.attachment0 = mytexture.Sample(mysampler, float2(pixelInput.tex.x, -pixelInput.tex.y)) * float4(pixelInput.color, 1.0f);
    
    return output;
}