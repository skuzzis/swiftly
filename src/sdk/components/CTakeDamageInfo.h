#include <memory/gamedata/gamedata.h>
#include <public/mathlib/vector.h>

typedef void (*CTakeDamageInfo_Constructor)(void*, void*, void*, void*, Vector*, Vector*, float, int, int, void*);

class CTakeDamageInfo
{
private:
	[[maybe_unused]] unsigned char __pad0000[0x8];

public:
	CTakeDamageInfo(void* pInflictor, void* pAttacker, void* pAbility, float flDamage, int bitsDamageType)
	{
        Vector vec3_origin(0.0,0.0,0.0);
        g_GameData.FetchSignature<CTakeDamageInfo_Constructor>("CTakeDamageInfo_Constructor")(this, pInflictor, pAttacker, pAbility, &vec3_origin, &vec3_origin, flDamage, bitsDamageType, 0, nullptr);
	}
};