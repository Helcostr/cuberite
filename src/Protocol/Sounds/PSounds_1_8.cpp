#include "../Protocol_1_8.h"


AString cProtocol_1_8_0::GetProtocolSoundEventAsString(SoundEvent a_SoundEvent) const
{
	switch (a_SoundEvent)
	{
		case SoundEvent::EnderEyeDeath: return "dig.glass";
		case SoundEvent::EnderEyeLaunch: return "random.bow";
	}
	return AString();
}
