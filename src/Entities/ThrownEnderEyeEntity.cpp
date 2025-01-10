#include "Globals.h"  // NOTE: MSVC stupidness requires this to be the same across all modules

#include "ThrownEnderEyeEntity.h"
#include "Player.h"
#include "FastRandom.h"





cThrownEnderEyeEntity::cThrownEnderEyeEntity(cEntity * a_Creator, Vector3d a_Pos, Vector3d a_Speed):
    // Set speed to zero, but use the data for eye of ender calculations
	Super(pkEnderEye, a_Creator, a_Pos, {0,0,0}, 0.25f, 0.25f),
	b_SurviveAfterDeath(GetRandomProvider().RandBool(0.5)),
	f_tx(0), f_ty(0), f_tz(0), xRot0(0), yRot0(0)
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
		f_tx = a_Pos.x + d2 / f * 12.0;
		f_tz = a_Pos.z + d4 / f * 12.0;
		f_ty = a_Pos.y + 8.0;
	}
	else
	{
		f_tx = d;
		f_ty = n;
		f_tz = d3;
	}
}

void cThrownEnderEyeEntity::OnHitEntity(cEntity & a_EntityHit, Vector3d a_HitPos) { }

void cThrownEnderEyeEntity::OnHitSolidBlock(
	Vector3d a_HitPos, eBlockFace a_HitFace)
{
}

void cThrownEnderEyeEntity::Tick(std::chrono::milliseconds a_Dt, cChunk & a_Chunk) {
    // IIRC, obj stored into obj is bad? Improve robustness of Vector?
	Vector3d Pos = GetPosition();

	// This is a bit of a mess, but it's a direct port of the Java code.
	// TODO: move this into HandlePhysics?

	// oldPos = Pos; // unused in Java code?
	xRot0 = GetYaw();
	yRot0 = GetPitch();

	Super::Tick(a_Dt, a_Chunk);
	FLOGD("Eye of Ender at {0:.02f} with speed {1:.02f}", Pos,GetSpeed());
	Pos += deltaMovement;
	SetPosition(Pos);
	double f = sqrt(deltaMovement.x * deltaMovement.x
	  + deltaMovement.z * deltaMovement.z);
	static const float magicNumber = 57.2957763671875;
	double yRot = atan2(deltaMovement.x, deltaMovement.z) * magicNumber;
	double xRot = atan2(deltaMovement.y, f) * magicNumber;

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

	double d = f_tx - Pos.x;
	double d2 = f_tz - Pos.z;
	double f2 = sqrt(d * d + d2 * d2);
	double f3 = atan2(d2, d);
	double d3 = f + 0.0025 * (f2 - f);
	double d4 = deltaMovement.y;
	if (f2 < 1.0) {
		d3 *= .8;
		d4 *= .8;
	}
	int n = Pos.y < f_ty ? 1 : -1;
	deltaMovement.Set(
	  cos(f3) * d3,
	  d4 + (n - d4)*.015,
	  sin(f3) * d3
	);
	SetSpeed(deltaMovement.x * 20., deltaMovement.y * 20., deltaMovement.z * 20.);
	// print delta movement
	FLOGD("DeltaMovement: {0:.02f}", deltaMovement);

	// Death of the ender eye if old enough
	if (m_TicksAlive > 80) {
		Destroy();
		cWorld* a_World = GetWorld();
		a_World->BroadcastSoundEffect("entity.ender_eye.death", Pos, 0.5f, 0.4f / GetRandomProvider().RandReal(0.8f, 1.2f));
		if (b_SurviveAfterDeath) {
			cItems Pickups;
			Pickups.Add(static_cast<ENUM_ITEM_TYPE>(E_ITEM_EYE_OF_ENDER), 1);
			a_World->SpawnItemPickups(Pickups, Pos);
		} else {
			/**
			* Java level event #2003
				double d = (double)blockPos.getX() + 0.5;
                double d11 = blockPos.getY();
                double d12 = (double)blockPos.getZ() + 0.5;

				// Is this just the particles for the ender eye breaking???
				// If so, use???:
				// m_World->BroadcastEntityAnimation(*this, EntityAnimation::EggCracks);
                for (int i = 0; i < 8; ++i) {
                    this.addParticle(new ItemParticleOption(ParticleTypes.ITEM, new ItemStack(Items.ENDER_EYE)), d, d11, d12, random.nextGaussian() * 0.15, random.nextDouble() * 0.2, random.nextGaussian() * 0.15);
                }
                for (double d13 = 0.0; d13 < Math.PI * 2; d13 += 0.15707963267948966) {
                    this.addParticle(ParticleTypes.PORTAL, d + Math.cos(d13) * 5.0, d11 - 0.4, d12 + Math.sin(d13) * 5.0, Math.cos(d13) * -5.0, 0.0, Math.sin(d13) * -5.0);
                    this.addParticle(ParticleTypes.PORTAL, d + Math.cos(d13) * 5.0, d11 - 0.4, d12 + Math.sin(d13) * 5.0, Math.cos(d13) * -7.0, 0.0, Math.sin(d13) * -7.0);
                }
			*/
		}
	}
}

void cThrownEnderEyeEntity::HandlePhysics(std::chrono::milliseconds a_Dt, cChunk & a_Chunk) { }


