//Texture2D smap : register(t3);
//SamplerComparisonState ssam : register(s1);

//#define PCF_RANGE 1

//float ShadowLoop_(const in float3 spos)
//{
//    float shadowLevel = 0.0f;
//    [unroll]
//    for (int x = -PCF_RANGE; x <= PCF_RANGE; x++)
//    {
//        [unroll]
//        for (int y = -PCF_RANGE; y <= PCF_RANGE; y++)
//        {
//            shadowLevel += smap.SampleCmpLevelZero(ssam, spos.xy, spos.b, int2(x, y));
//        }
//    }
//    return shadowLevel / ((PCF_RANGE * 2 + 1) * (PCF_RANGE * 2 + 1));
//}

//float Shadow(const in float4 shadowHomoPos)
//{
//    float shadowLevel = 0.0f;
//    const float3 spos = shadowHomoPos.xyz / shadowHomoPos.w;
    
//    if (spos.z > 1.0f || spos.z < 0.0f)
//    {
//        shadowLevel = 1.0f;
//    }
//    else
//    {
//        shadowLevel = ShadowLoop_(spos);
//    }
//    return shadowLevel;
//}




//TextureCube smap : register(t3);
//SamplerComparisonState ssam : register(s1);
//SamplerState dgsam : register(s2);

//float ShadowLoop_(const in float4 spos)
//{
//    float len = length(spos.xyz);
//    float4 checkme = smap.Sample(dgsam, normalize(spos.xyz));
//    if (checkme.r < 1000.0f)
//    {
//        return smap.SampleCmpLevelZero(ssam, normalize(spos.xyz), len / 100.0f);
//    }
//    else
//    {
//        return 4.0f;
//    }
//}

//float Shadow(const in float4 shadowPos)
//{
//    return ShadowLoop_(shadowPos);
//}




TextureCube smap : register(t3);
SamplerComparisonState ssam : register(s1);

static const float zf = 100.0f;
static const float zn = 0.5f;
static const float c1 = zf / (zf - zn);
static const float c0 = -zn * zf / (zf - zn);

float CalculateShadowDepth(const in float4 shadowPos)
{
    // get magnitudes for each basis component
    const float3 m = abs(shadowPos).xyz;
    // get the length in the dominant axis
    // (this correlates with shadow map face and derives comparison depth)
    const float major = max(m.x, max(m.y, m.z));
    // converting from distance in shadow light space to projected depth
    return (c1 * major + c0) / major;
}

float Shadow(const in float4 shadowPos)
{
    return smap.SampleCmpLevelZero(ssam, shadowPos.xyz, CalculateShadowDepth(shadowPos));
}