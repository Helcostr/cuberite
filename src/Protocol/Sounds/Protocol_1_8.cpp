#include "Globals.h"
#include "../Protocol_1_8.h"
#include "../Packetizer.h"

void cProtocol_1_8_0::SendSoundEffect(SoundEvent a_SoundEvent, Vector3d a_Origin, float a_Volume, float a_Pitch) {
	AString soundName;
	switch (a_SoundEvent)
	{
		case SoundEvent::EnderEyeDeath:
			soundName = "dig.glass";
			a_Pitch = 0.8f;
			break;
		case SoundEvent::EnderEyeLaunch:
			soundName = "random.bow";
			a_Pitch = .5f;
			break;
	}

	ASSERT(m_State == 3);  // In game mode?

	cPacketizer Pkt(*this, pktSoundEffect);
	Pkt.WriteString(soundName);
	Pkt.WriteBEInt32(static_cast<Int32>(a_Origin.x * 8.0));
	Pkt.WriteBEInt32(static_cast<Int32>(a_Origin.y * 8.0));
	Pkt.WriteBEInt32(static_cast<Int32>(a_Origin.z * 8.0));
	Pkt.WriteBEFloat(a_Volume);
	Pkt.WriteBEUInt8(static_cast<Byte>(a_Pitch * 63));
}
