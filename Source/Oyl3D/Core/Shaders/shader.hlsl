static float2 g_positions[3] = {
	float2(0.0, -0.5),
	float2(0.5, 0.5),
	float2(-0.5, 0.5)
};

static float3 g_colors[3] = {
	float3(1.0, 0.0, 0.0),
	float3(0.0, 1.0, 0.0),
	float3(0.0, 0.0, 1.0)
};

struct VertexOutput
{
	float4 sv_position : SV_Position;
	float3 color : Color;
};

VertexOutput
VertMain(uint a_vid : SV_VertexID)
{
	VertexOutput output;
	output.sv_position = float4(g_positions[a_vid], 0.0, 1.0);
	output.color = g_colors[a_vid];
	return output;
}

float4
FragMain(VertexOutput a_inVert) : SV_Target
{
	float3 color = a_inVert.color;
	return float4(color, 1.0);
}
