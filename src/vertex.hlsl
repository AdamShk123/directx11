cbuffer cb : register(b0)
{
    row_major float4x4 projection : packoffset(c0);
    row_major float4x4 model : packoffset(c4);
    row_major float4x4 view : packoffset(c8);
};

struct VertexInput
{
    float3 position : POSITION;
    float3 color : COLOR;
    float2 tex : TEXTURE;
};

struct VertexOutput
{
    float3 color : COLOR;
    float4 position : SV_Position;
    float2 tex : TEXCOORD;
};

VertexOutput main(VertexInput vertexInput)
{
    VertexOutput output;
    output.color = vertexInput.color;
    output.position = mul(float4(vertexInput.position.x, vertexInput.position.y, vertexInput.position.z, 1.0), mul(model, mul(view, projection)));
    output.tex = vertexInput.tex;
    
    return output;
}