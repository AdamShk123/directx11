cbuffer cb : register(b0)
{
    row_major float4x4 projection : packoffset(c0);
    row_major float4x4 model : packoffset(c4);
    row_major float4x4 view : packoffset(c8);
};

struct VertexInput
{
    float3 inPos : POSITION;
    float3 inColor : COLOR;
};

struct VertexOutput
{
    float3 color : COLOR;
    float4 position : SV_Position;
};

VertexOutput main(VertexInput vertexInput)
{
    VertexOutput output;
    output.color = vertexInput.inColor;
    output.position = mul(float4(vertexInput.inPos, 1.0), mul(model, mul(view, projection)));
    
    return output;
}