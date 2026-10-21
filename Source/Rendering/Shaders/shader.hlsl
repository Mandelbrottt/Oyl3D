struct VertexInput
{
	float2 position : Position;
	float3 color : Color;
};

struct VertexOutput
{
	float4 position : SV_Position;
	float3 color : Color;
};

[shader("vertex")]
VertexOutput
VertMain(VertexInput a_input)
{
	VertexOutput output;
	output.position = float4(a_input.position, 0.0, 1.0);
	output.color = a_input.color;
	return output;
}

[shader("fragment")]
float4
FragMain(VertexOutput a_inVert) : SV_Target
{
	return float4(a_inVert.color, 1.0);
}
