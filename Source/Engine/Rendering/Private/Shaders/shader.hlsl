struct VertexInput
{
	float3 position : Position;
	float3 color : Color;
};

struct UniformBuffer
{
	float4x4 model;
	float4x4 view;
	float4x4 projection;
};

ConstantBuffer<UniformBuffer> g_ubo;

struct VertexOutput
{
	float4 position : SV_Position;
	float3 color : Color;
};

[shader("vertex")]
VertexOutput
VertMain(VertexInput a_input)
{
	float4 modelPosition = mul(g_ubo.model, float4(a_input.position, 1.0));
	float4 modelViewPosition = mul(g_ubo.view, modelPosition);
	float4 modelViewProjPosition = mul(g_ubo.projection, modelViewPosition);

	VertexOutput output;
	output.position = modelViewProjPosition;
	output.color = a_input.color;
	return output;
}

[shader("fragment")]
float4
FragMain(VertexOutput a_inVert) : SV_Target
{
	return float4(a_inVert.color, 1.0);
}
