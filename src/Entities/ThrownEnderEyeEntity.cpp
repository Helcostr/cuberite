#include "Globals.h"  // NOTE: MSVC stupidness requires this to be the same across all modules

#include "ThrownEnderEyeEntity.h"
#include "Player.h"
#include "FastRandom.h"





cThrownEnderEyeEntity::cThrownEnderEyeEntity(cEntity * a_Creator, Vector3d a_Pos, Vector3d a_Speed):
	// Set speed to zero, but use the data for eye of ender calculations
	Super(pkEnderEye, a_Creator, a_Pos, {0,0,0}, 0.25f, 0.25f),
	m_SurviveAfterDeath(GetRandomProvider().RandBool(0.5)),	
	m_Target_X(0), m_Target_Y(0), m_Target_Z(0), xRot0(0), yRot0(0)
{
	// SetGravity(0.0f);
	double d = a_Speed.x;
	int n = a_Speed.y;
	double d2 = d - a_Pos.x;
	double d3 = a_Speed.z;
	double d4 = d3 - a_Pos.z;
	
	double f = sqrt(d2 * d2 + d4 * d4);
	if (f > 12.f)
	{
		m_Target_X = a_Pos.x + d2 / f * 12.0;
		m_Target_Z = a_Pos.z + d4 / f * 12.0;
		m_Target_Y = a_Pos.y + 8.0;
	}
	else
	{
		m_Target_X = d;
		m_Target_Y = n;
		m_Target_Z = d3;
	}
}





void cThrownEnderEyeEntity::OnHitEntity(cEntity & a_EntityHit, Vector3d a_HitPos) { }





void cThrownEnderEyeEntity::OnHitSolidBlock(Vector3d a_HitPos, eBlockFace a_HitFace) { }





void cThrownEnderEyeEntity::Tick(std::chrono::milliseconds a_Dt, cChunk & a_Chunk) {
	// IIRC, obj stored into obj is bad? Improve robustness of Vector?
	Vector3d Pos = GetPosition();

	// This is a bit of a mess, but it's a direct port of the Java code.
	// TODO: move this into HandlePhysics?

	// oldPos = Pos; // unused in Java code?
	xRot0 = GetYaw();
	yRot0 = GetPitch();

	Super::Tick(a_Dt, a_Chunk);
	// FLOGD("Eye of Ender at {0:.02f} with speed {1:.02f}", Pos,GetSpeed());
	Pos += m_DeltaMovement;
	SetPosition(Pos);
	double f = sqrt(m_DeltaMovement.x * m_DeltaMovement.x
	  + m_DeltaMovement.z * m_DeltaMovement.z);
	static const float magicNumber = 57.2957763671875;
	double yRot = atan2(m_DeltaMovement.x, m_DeltaMovement.z) * magicNumber;
	double xRot = atan2(m_DeltaMovement.y, f) * magicNumber;

	while (xRot - xRot0 < -180.0f) { xRot0 -= 360.0f; }
	while (xRot - xRot0 >= 180.0f) { xRot0 += 360.0f; }
	// Trying not to use while loops? Investigate later.
	// double xNorm = ((xRot - xRot0) + 180.0);
	// xRot0 += xNorm / 360.0f + fmod(xNorm,360.0f);
	while (yRot - yRot0 < -180.0f) { yRot0 -= 360.0f; }
	while (yRot - yRot0 >= 180.0f) { yRot0 += 360.0f; }
	// print rot
	// FLOGD("OldRotations: {0}, {1}", xRot0, yRot0);
	// FLOGD("Rotations: {0}, {1}", xRot, yRot);
	SetYaw(xRot0 + 0.2 * (xRot-xRot0));
	SetPitch(yRot0 + 0.2 * (yRot - yRot0));

	double d = m_Target_X - Pos.x;
	double d2 = m_Target_Z - Pos.z;
	double f2 = sqrt(d * d + d2 * d2);
	double f3 = atan2(d2, d);
	double d3 = f + 0.0025 * (f2 - f);
	double d4 = m_DeltaMovement.y;
	if (f2 < 1.0) {
		d3 *= .8;
		d4 *= .8;
	}
	int n = Pos.y < m_Target_Y ? 1 : -1;
	m_DeltaMovement.Set(
	  cos(f3) * d3,
	  d4 + (n - d4)*.015,
	  sin(f3) * d3
	);
	SetSpeed(m_DeltaMovement.x * 20., m_DeltaMovement.y * 20., m_DeltaMovement.z * 20.);

	// Death of the ender eye if old enough
	if (m_TicksAlive > 80) {
		Destroy();
		cWorld* a_World = GetWorld();
		// Hardcoding pitch might pose a problem
		a_World->BroadcastSoundEffect(SoundEvent::EnderEyeDeath, Pos, 1.0f, 0.8f);
		if (m_SurviveAfterDeath) {
			cItems Pickups;
			Pickups.Add(static_cast<ENUM_ITEM_TYPE>(E_ITEM_EYE_OF_ENDER), 1);
			a_World->SpawnItemPickups(Pickups, Pos);
		} else {
			// TODO: Java level event #2003
		}
	}
}

void cThrownEnderEyeEntity::HandlePhysics(std::chrono::milliseconds a_Dt, cChunk & a_Chunk) { }
