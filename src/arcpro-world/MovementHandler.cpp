/*
 * ArcPro MMORPG Server
 * Copyright (c) 2011-2013 ArcPro Speculation <http://arcpro.sexyi.am/>
 * Copyright (c) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 * Copyright (c) 2008-2013 ArcEmu Team <http://www.arcemu.org/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "StdAfx.h"
#define SWIMMING_TOLERANCE_LEVEL -0.08f
#define MOVEMENT_PACKET_TIME_DELAY 500

#ifdef WIN32

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#define DELTA_EPOCH_IN_USEC  11644473600000000ULL
uint32 TimeStamp()
{
	//return timeGetTime();

	FILETIME ft;
	uint64 t;
	GetSystemTimeAsFileTime(&ft);

	t = (uint64)ft.dwHighDateTime << 32;
	t |= ft.dwLowDateTime;
	t /= 10;
	t -= DELTA_EPOCH_IN_USEC;

	return uint32(((t / 1000000L) * 1000) + ((t % 1000000L) / 1000));
}

uint32 mTimeStamp()
{
	return timeGetTime();
}

#else

uint32 TimeStamp()
{
	struct timeval tp;
	gettimeofday(&tp, NULL);
	return (tp.tv_sec * 1000) + (tp.tv_usec / 1000);
}

uint32 mTimeStamp()
{
	struct timeval tp;
	gettimeofday(&tp, NULL);
	return (tp.tv_sec * 1000) + (tp.tv_usec / 1000);
}

#endif

void WorldSession::HandleMoveWorldportAckOpcode(WorldPacket & recv_data)
{
	GetPlayer()->SetPlayerStatus(NONE);
	if(_player->IsInWorld())
	{
		// get outta here
		return;
	}
	LOG_DEBUG("WORLD: got MSG_MOVE_WORLDPORT_ACK.");

	if(_player->m_CurrentTransporter && _player->GetMapId() != _player->m_CurrentTransporter->GetMapId())
	{
		/* wow, our pc must really suck. */
		Transporter* pTrans = _player->m_CurrentTransporter;

		float c_tposx = pTrans->GetPositionX() + _player->transporter_info.x;
		float c_tposy = pTrans->GetPositionY() + _player->transporter_info.y;
		float c_tposz = pTrans->GetPositionZ() + _player->transporter_info.z;


		_player->SetMapId(pTrans->GetMapId());
		_player->SetPosition(c_tposx, c_tposy, c_tposz, _player->GetOrientation());

		WorldPacket dataw(SMSG_NEW_WORLD, 20);

		dataw << pTrans->GetMapId();
		dataw << c_tposx;
		dataw << c_tposy;
		dataw << c_tposz;
		dataw << _player->GetOrientation();

		SendPacket(&dataw);
	}
	else
	{
		_player->m_TeleportState = 2;
		_player->AddToWorld();
	}
}

void WorldSession::HandleMoveTeleportAckOpcode(WorldPacket & recv_data)
{
	WoWGuid guid;
	recv_data >> guid;
	if(guid == _player->GetGUID())
	{
		if(sWorld.antihack_teleport && !(HasGMPermissions() && sWorld.no_antihack_on_gm) && _player->GetPlayerStatus() != TRANSFER_PENDING)
		{
			/* we're obviously cheating */
			sCheatLog.writefromsession(this, "Used teleport hack, disconnecting.");
			Disconnect();
			return;
		}

		if(sWorld.antihack_teleport && !(HasGMPermissions() && sWorld.no_antihack_on_gm) && _player->m_position.Distance2DSq(_player->m_sentTeleportPosition) > 625.0f)	/* 25.0f*25.0f */
		{
			/* cheating.... :( */
			sCheatLog.writefromsession(this, "Used teleport hack {2}, disconnecting.");
			Disconnect();
			return;
		}

		LOG_DEBUG("WORLD: got MSG_MOVE_TELEPORT_ACK.");
		GetPlayer()->SetPlayerStatus(NONE);
		if(GetPlayer()->m_rooted <= 0)
			GetPlayer()->SetMovement(MOVE_UNROOT, 5);
		_player->SpeedCheatReset();

		std::list<Pet*> summons = _player->GetSummons();
		for(std::list<Pet*>::iterator itr = summons.begin(); itr != summons.end(); ++itr)
		{
			// move pet too
			(*itr)->SetPosition((GetPlayer()->GetPositionX() + 2), (GetPlayer()->GetPositionY() + 2), GetPlayer()->GetPositionZ(), M_PI_FLOAT);
		}
		if(_player->m_sentTeleportPosition.x != 999999.0f)
		{
			_player->m_position = _player->m_sentTeleportPosition;
			_player->m_sentTeleportPosition.ChangeCoords(999999.0f, 999999.0f, 999999.0f);
		}
	}

}

void _HandleBreathing(MovementInfo & movement_info, Player* _player, WorldSession* pSession)
{

	// no water breathing is required
	if(!sWorld.BreathingEnabled || _player->FlyCheat || _player->m_bUnlimitedBreath || !_player->isAlive() || _player->GodModeCheat)
	{
		// player is flagged as in water
		if(_player->m_UnderwaterState & UNDERWATERSTATE_SWIMMING)
			_player->m_UnderwaterState &= ~UNDERWATERSTATE_SWIMMING;

		// player is flagged as under water
		if(_player->m_UnderwaterState & UNDERWATERSTATE_UNDERWATER)
		{
			_player->m_UnderwaterState &= ~UNDERWATERSTATE_UNDERWATER;
			WorldPacket data(SMSG_START_MIRROR_TIMER, 20);
			data << uint32(TIMER_BREATH) << _player->m_UnderwaterTime << _player->m_UnderwaterMaxTime << int32(-1) << uint32(0);

			pSession->SendPacket(&data);
		}

		// player is above water level
		if(pSession->m_bIsWLevelSet)
		{
			if((movement_info.GetMovementPosZ() + _player->m_noseLevel) > pSession->m_wLevel)
			{
				_player->RemoveAurasByInterruptFlag(AURA_INTERRUPT_ON_LEAVE_WATER);

				// unset swim session water level
				pSession->m_bIsWLevelSet = false;
			}
		}

		return;
	}

	//player is swimming and not flagged as in the water
	if(movement_info.GetMovementFlags() & MOVEFLAG_SWIMMING && !(_player->m_UnderwaterState & UNDERWATERSTATE_SWIMMING))
	{
		_player->RemoveAurasByInterruptFlag(AURA_INTERRUPT_ON_ENTER_WATER);

		// get water level only if it was not set before
		if(!pSession->m_bIsWLevelSet)
		{
			// water level is somewhere below the nose of the character when entering water
			pSession->m_wLevel = movement_info.GetMovementPosZ() + _player->m_noseLevel * 0.95f;
			pSession->m_bIsWLevelSet = true;
		}

		_player->m_UnderwaterState |= UNDERWATERSTATE_SWIMMING;
	}

	// player is not swimming and is not stationary and is flagged as in the water
	if(!(movement_info.GetMovementFlags() & MOVEFLAG_SWIMMING) && (movement_info.GetMovementFlags() != MOVEFLAG_MOVE_STOP) && (_player->m_UnderwaterState & UNDERWATERSTATE_SWIMMING))
	{
		// player is above water level
		if((movement_info.GetMovementPosZ() + _player->m_noseLevel) > pSession->m_wLevel)
		{
			_player->RemoveAurasByInterruptFlag(AURA_INTERRUPT_ON_LEAVE_WATER);

			// unset swim session water level
			pSession->m_bIsWLevelSet = false;

			_player->m_UnderwaterState &= ~UNDERWATERSTATE_SWIMMING;
		}
	}

	// player is flagged as in the water and is not flagged as under the water
	if(_player->m_UnderwaterState & UNDERWATERSTATE_SWIMMING && !(_player->m_UnderwaterState & UNDERWATERSTATE_UNDERWATER))
	{
		//the player is in the water and has gone under water, requires breath bar.
		if((movement_info.GetMovementPosZ() + _player->m_noseLevel) < pSession->m_wLevel)
		{
			_player->m_UnderwaterState |= UNDERWATERSTATE_UNDERWATER;
			WorldPacket data(SMSG_START_MIRROR_TIMER, 20);
			data << uint32(TIMER_BREATH) << _player->m_UnderwaterTime << _player->m_UnderwaterMaxTime << int32(-1) << uint32(0);

			pSession->SendPacket(&data);
		}
	}

	// player is flagged as in the water and is flagged as under the water
	if(_player->m_UnderwaterState & UNDERWATERSTATE_SWIMMING && _player->m_UnderwaterState & UNDERWATERSTATE_UNDERWATER)
	{
		//the player is in the water but their face is above water, no breath bar needed.
		if((movement_info.GetMovementPosZ() + _player->m_noseLevel) > pSession->m_wLevel)
		{
			_player->m_UnderwaterState &= ~UNDERWATERSTATE_UNDERWATER;
			WorldPacket data(SMSG_START_MIRROR_TIMER, 20);
			data << uint32(TIMER_BREATH) << _player->m_UnderwaterTime << _player->m_UnderwaterMaxTime << int32(10) << uint32(0);
			pSession->SendPacket(&data);
		}
	}

	// player is flagged as not in the water and is flagged as under the water
	if(!(_player->m_UnderwaterState & UNDERWATERSTATE_SWIMMING) && _player->m_UnderwaterState & UNDERWATERSTATE_UNDERWATER)
	{
		//the player is out of the water, no breath bar needed.
		if((movement_info.GetMovementPosZ() + _player->m_noseLevel) > pSession->m_wLevel)
		{
			_player->m_UnderwaterState &= ~UNDERWATERSTATE_UNDERWATER;
			WorldPacket data(SMSG_START_MIRROR_TIMER, 20);
			data << uint32(TIMER_BREATH) << _player->m_UnderwaterTime << _player->m_UnderwaterMaxTime << int32(10) << uint32(0);
			pSession->SendPacket(&data);
		}
	}

}

struct MovementFlagName
{
	uint32 flag;
	const char* name;
};

static MovementFlagName MoveFlagsToNames[] =
{
	{ MOVEFLAG_MOVE_STOP, "MOVEFLAG_MOVE_STOP" },
	{ MOVEFLAG_MOVE_FORWARD, "MOVEFLAG_MOVE_FORWARD" },
	{ MOVEFLAG_MOVE_BACKWARD, "MOVEFLAG_MOVE_BACKWARD" },
	{ MOVEFLAG_STRAFE_LEFT, "MOVEFLAG_STRAFE_LEFT" },
	{ MOVEFLAG_STRAFE_RIGHT, "MOVEFLAG_STRAFE_RIGHT" },
	{ MOVEFLAG_TURN_LEFT, "MOVEFLAG_TURN_LEFT" },
	{ MOVEFLAG_TURN_RIGHT, "MOVEFLAG_TURN_RIGHT" },
	{ MOVEFLAG_PITCH_DOWN, "MOVEFLAG_PITCH_DOWN" },
	{ MOVEFLAG_PITCH_UP, "MOVEFLAG_PITCH_UP" },
	{ MOVEFLAG_WALK, "MOVEFLAG_WALK" },
	{ MOVEFLAG_TRANSPORT, "MOVEFLAG_TRANSPORT" },
	{ MOVEFLAG_NO_COLLISION, "MOVEFLAG_NO_COLLISION" },
	{ MOVEFLAG_ROOTED, "MOVEFLAG_ROOTED" },
	{ MOVEFLAG_REDIRECTED, "MOVEFLAG_REDIRECTED" },
	{ MOVEFLAG_FALLING, "MOVEFLAG_FALLING" },
	{ MOVEFLAG_FALLING_FAR, "MOVEFLAG_FALLING_FAR" },
	{ MOVEFLAG_FREE_FALLING, "MOVEFLAG_FREE_FALLING" },
	{ MOVEFLAG_TB_PENDING_STOP, "MOVEFLAG_TB_PENDING_STOP" },
	{ MOVEFLAG_TB_PENDING_UNSTRAFE, "MOVEFLAG_TB_PENDING_UNSTRAFE" },
	{ MOVEFLAG_TB_PENDING_FALL, "MOVEFLAG_TB_PENDING_FALL" },
	{ MOVEFLAG_TB_PENDING_FORWARD, "MOVEFLAG_TB_PENDING_FORWARD" },
	{ MOVEFLAG_TB_PENDING_BACKWARD, "MOVEFLAG_TB_PENDING_BACKWARD" },
	{ MOVEFLAG_SWIMMING, "MOVEFLAG_SWIMMING" },
	{ MOVEFLAG_FLYING_PITCH_UP, "MOVEFLAG_FLYING_PITCH_UP" },
	{ MOVEFLAG_CAN_FLY, "MOVEFLAG_CAN_FLY" },
	{ MOVEFLAG_AIR_SUSPENSION, "MOVEFLAG_AIR_SUSPENSION" },
	{ MOVEFLAG_AIR_SWIMMING, "MOVEFLAG_AIR_SWIMMING" },
	{ MOVEFLAG_SPLINE_MOVER, "MOVEFLAG_SPLINE_MOVER" },
	{ MOVEFLAG_SPLINE_ENABLED, "MOVEFLAG_SPLINE_ENABLED" },
	{ MOVEFLAG_WATER_WALK, "MOVEFLAG_WATER_WALK" },
	{ MOVEFLAG_FEATHER_FALL, "MOVEFLAG_FEATHER_FALL" },
	{ MOVEFLAG_LEVITATE, "MOVEFLAG_LEVITATE" },
	{ MOVEFLAG_LOCAL, "MOVEFLAG_LOCAL" },
};

static const uint32 nmovementflags = sizeof(MoveFlagsToNames) / sizeof(MovementFlagName);

void WorldSession::HandleMovementOpcodes(WorldPacket & recv_data)
{
	CHECK_INWORLD_RETURN

	bool moved = true;

	if(_player->GetCharmedByGUID() || _player->GetPlayerStatus() == TRANSFER_PENDING || _player->GetTaxiState() || _player->getDeathState() == JUST_DIED)
		return;

	// spell cancel on movement, for now only fishing is added
	Object* t_go = _player->m_SummonedObject;
	if(t_go)
	{
		if(t_go->GetEntry() == GO_FISHING_BOBBER)
			TO_GAMEOBJECT(t_go)->EndFishing(GetPlayer(), true);
	}

	/************************************************************************/
	/* Clear standing state to stand.				                        */
	/************************************************************************/
	if(recv_data.GetOpcode() == MSG_MOVE_START_FORWARD)
		_player->SetStandState(STANDSTATE_STAND);

	/************************************************************************/
	/* Read Movement Data Packet                                            */
	/************************************************************************/
	uint64 MoverGuid = recv_data.unpackGUID();
	movement_info.Init(MoverGuid);
	movement_info.Read(recv_data);
	
	// Player is in control of some entity, so we move that instead of the player
	Unit *mover = _player->GetMapMgr()->GetUnit( movement_info.GetMovementGUID().GetOldGuid() );
	if( mover == NULL )
		return;

	/* Anti Multi-Jump Check */
	if(recv_data.GetOpcode() == MSG_MOVE_JUMP && _player->jumping == true && !GetPermissionCount())
	{
		sCheatLog.writefromsession(this, "Detected jump hacking");
		Disconnect();
		return;
	}
	if(recv_data.GetOpcode() == MSG_MOVE_FALL_LAND || movement_info.GetMovementFlags() & MOVEFLAG_SWIMMING)
		_player->jumping = false;
	if(!_player->jumping && (recv_data.GetOpcode() == MSG_MOVE_JUMP || movement_info.GetMovementFlags() & MOVEFLAG_FALLING))
		_player->jumping = true;

	/************************************************************************/
	/* Update player movement state                                         */
	/************************************************************************/

	uint16 opcode = recv_data.GetOpcode();
	switch(opcode)
	{
		case MSG_MOVE_START_FORWARD:
		case MSG_MOVE_START_BACKWARD:
			_player->moving = true;
			break;
		case MSG_MOVE_START_STRAFE_LEFT:
		case MSG_MOVE_START_STRAFE_RIGHT:
			_player->strafing = true;
			break;
		case MSG_MOVE_JUMP:
			_player->jumping = true;
			break;
		case MSG_MOVE_STOP:
			_player->moving = false;
			break;
		case MSG_MOVE_STOP_STRAFE:
			_player->strafing = false;
			break;
		case MSG_MOVE_FALL_LAND:
			_player->jumping = false;
			break;
		
		default:
			moved = false;
			break;
	}

#if 0

	LOG_DETAIL("Got %s", g_worldOpcodeNames[ opcode ].name);

	LOG_DETAIL("Movement flags");
	for(uint32 i = 0; i < nmovementflags; i++)
		if((movement_info.GetMovementFlags() & MoveFlagsToNames[ i ].flag) != 0)
			LOG_DETAIL("%s", MoveFlagsToNames[ i ].name);

#endif

	if(moved)
	{
		if(!_player->moving && !_player->strafing && !_player->jumping)
		{
			_player->m_isMoving = false;
		}
		else
		{
			_player->m_isMoving = true;
		}
	}

	// Rotating your character with a hold down right click mouse button
	if(_player->GetOrientation() != movement_info.GetOrientation())
		_player->isTurning = true;
	else
		_player->isTurning = false;


	/************************************************************************/
	/* Anti-Fly Hack Checks       - Alice : Disabled for now                                           */
	/************************************************************************/
	/*if( sWorld.antihack_flight && ( recv_data.GetOpcode() == CMSG_MOVE_FLY_START_AND_END || recv_data.GetOpcode() == CMSG_FLY_PITCH_DOWN_AFTER_UP ) && !( movement_info.GetMovementFlags() & MOVEFLAG_SWIMMING || movement_info.GetMovementFlags() & MOVEFLAG_FALLING || movement_info.GetMovementFlags() & MOVEFLAG_FALLING_FAR || movement_info.GetMovementFlags() & MOVEFLAG_FREE_FALLING ) && _player->flying_aura == 0 )
	{
		if( sWorld.no_antihack_on_gm && _player->GetSession()->HasGMPermissions() )
		{
			// Do nothing.
		}
		else
		{
			_player->BroadcastMessage( "Flyhack detected. In case the server is wrong then make a report how to reproduce this case. You will be logged out in 5 seconds." );
			sEventMgr.AddEvent( _player, &Player::_Kick, EVENT_PLAYER_KICK, 5000, 1, 0 );
		}
	} */

	if(!(HasGMPermissions() && sWorld.no_antihack_on_gm) && !_player->GetCharmedUnitGUID())
	{
		/************************************************************************/
		/* Anti-Teleport                                                        */
		/************************************************************************/
		if(sWorld.antihack_teleport && _player->m_position.Distance2DSq(movement_info.GetMovementPosX(), movement_info.GetMovementPosY()) > 3025.0f
		        && _player->m_runSpeed < 50.0f && !_player->transporter_info.guid)
		{
			sCheatLog.writefromsession(this, "Disconnected for teleport hacking. Player speed: %f, Distance traveled: %f", _player->m_runSpeed, sqrt(_player->m_position.Distance2DSq(movement_info.GetMovementPosX, movement_info.GetMovementPosY())));
			Disconnect();
			return;
		}
	}

	//update the detector
	if(sWorld.antihack_speed && !_player->GetTaxiState() && _player->transporter_info.guid == 0 && !_player->GetSession()->GetPermissionCount())
	{
		// simplified: just take the fastest speed. less chance of fuckups too
		float speed = (_player->flying_aura) ? _player->m_flySpeed : (_player->m_swimSpeed > _player-> m_runSpeed) ? _player->m_swimSpeed : _player->m_runSpeed;

		_player->SDetector->AddSample(movement_info.GetMovementPostX(), movement_info.GetMovementPosY(), getMSTime(), speed);

		if(_player->SDetector->IsCheatDetected())
			_player->SDetector->ReportCheater(_player);
	}

	/************************************************************************/
	/* Remove Emote State                                                   */
	/************************************************************************/
	if(_player->GetEmoteState())
		_player->SetEmoteState(0);

	/************************************************************************/
	/* Make sure the co-ordinates are valid.                                */
	/************************************************************************/
	if(!((movement_info.GetMovementPostY() >= _minY) && (movement_info.GetMovementPostY() <= _maxY)) || !((movement_info.GetMovementPostX() >= _minX) && (movement_info.GetMovementPostX() <= _maxX)))
	{
		Disconnect();
		return;
	}

	/************************************************************************/
	/* Dump movement flags - Wheee!                                         */
	/************************************************************************/
#if 0
	LOG_DEBUG("=========================================================");
	LOG_DEBUG("Full movement flags: 0x%.8X", movement_info.GetMovementFlags());
	uint32 z, b;
	for(z = 1, b = 1; b < 32;)
	{
		if(movement_info.GetMovementFlags() & z)
			LOG_DEBUG("   Bit %u (0x%.8X or %u) is set!", b, z, z);

		z <<= 1;
		b += 1;
	}
	LOG_DEBUG("=========================================================");
#endif

	/************************************************************************/
	/* Orientation dumping                                                  */
	/************************************************************************/
#if 0
	LOG_DEBUG("Packet: 0x%03X (%s)", recv_data.GetOpcode(), LookupName(recv_data.GetOpcode(), g_worldOpcodeNames));
	LOG_DEBUG("Orientation: %.10f", movement_info.GetOrientation());
#endif

	/************************************************************************/
	/* Copy into the output buffer.                                         */
	/************************************************************************/
	if(_player->m_inRangePlayers.size())
	{
		int32 move_time = movement_info.GetMovementTime()+_latency, mTime = getMSTime();
		if(move_time > mTime)
			move_time = mTime;

		WorldPacket data(recv_data.GetOpcode(), recv_data.size());
		data << mover->GetNewGUID();
		GetMovementInfo()->Write(data, move_time);
		_player->SendMessageToSet(&data, false);
	}

	/************************************************************************/
	/* Hack Detection by Classic	                                        */
	/************************************************************************/
	if(!movement_info.GetTransportGUID() && recv_data.GetOpcode() != MSG_MOVE_JUMP && !_player->FlyCheat && !_player->flying_aura && !(movement_info.GetMovementFlags() & MOVEFLAG_SWIMMING || movement_info.GetMovementFlags() & MOVEFLAG_FALLING) && movement_info.GetMovementPosZ() > _player->GetPositionZ() && movement_info.GetMovementPosX() == _player->GetPositionX() && movement_info.GetMovementPosY() == _player->GetPositionY())
	{
		WorldPacket data(SMSG_MOVE_UNSET_CAN_FLY, 13);
		data << _player->GetNewGUID();
		data << uint32(5);
		SendPacket(&data);
	}

	if((movement_info.GetMovementFlags() & MOVEFLAG_AIR_SWIMMING) && !(movement_info.GetMovementFlags() & MOVEFLAG_SWIMMING) && !(_player->flying_aura || _player->FlyCheat))
	{
		WorldPacket data(SMSG_MOVE_UNSET_CAN_FLY, 13);
		data << _player->GetNewGUID();
		data << uint32(5);
		SendPacket(&data);
	}

	/************************************************************************/
	/* Falling damage checks                                                */
	/************************************************************************/

	if(_player->blinked)
	{
		_player->blinked = false;
		_player->m_fallDisabledUntil = UNIXTIME + 5;
		_player->SpeedCheatDelay(2000);   //some say they managed to trigger system with knockback. Maybe they moved in air ?
	}
	else
	{
		if(recv_data.GetOpcode() == MSG_MOVE_FALL_LAND)
		{
			// player has finished falling
			//if z_axisposition contains no data then set to current position
			if(!mover->z_axisposition)
				mover->z_axisposition = movement_info.GetMovementPosZ();

			// calculate distance fallen
			uint32 falldistance = float2int32(mover->z_axisposition - movement_info.GetMovementPosZ());
			if(mover->z_axisposition <= movement_info.GetMovementPosZ())
				falldistance = 1;
			/*Safe Fall*/
			if((int)falldistance > mover->m_safeFall)
				falldistance -= mover->m_safeFall;
			else
				falldistance = 1;

			//checks that player has fallen more than 12 units, otherwise no damage will be dealt
			//falltime check is also needed here, otherwise sudden changes in Z axis position, such as using !recall, may result in death
			if( mover->isAlive() && !mover->bInvincible && ( falldistance > 12 ) && !mover->m_noFallDamage &&
				( ( mover->GetGUID() != _player->GetGUID() ) || ( !_player->GodModeCheat && ( UNIXTIME >= _player->m_fallDisabledUntil ) ) ) )
			{
				// 1.7% damage for each unit fallen on Z axis over 13
				uint32 health_loss = static_cast< uint32 >( mover->GetHealth() * ( falldistance - 12 ) * 0.017f );

				if( health_loss >= mover->GetHealth() )
					health_loss = mover->GetHealth();
#ifdef ENABLE_ACHIEVEMENTS
				else if( ( falldistance >= 65 ) && ( mover->GetGUID() == _player->GetGUID() ) )
				{
					// Rather than Updating achievement progress every time fall damage is taken, all criteria currently have 65 yard requirement...
					// Achievement 964: Fall 65 yards without dying.
					// Achievement 1260: Fall 65 yards without dying while completely smashed during the Brewfest Holiday.
					_player->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_FALL_WITHOUT_DYING, falldistance, Player::GetDrunkenstateByValue(_player->GetDrunkValue()), 0);
				}
#endif

				mover->SendEnvironmentalDamageLog(mover->GetGUID(), DAMAGE_FALL, health_loss);
				mover->DealDamage(mover, health_loss, 0, 0, 0);

				//_player->RemoveStealth(); // cebernic : why again? lost stealth by AURA_INTERRUPT_ON_ANY_DAMAGE_TAKEN already.
			}
			mover->z_axisposition = 0.0f;
		}
		else
			//whilst player is not falling, continuously update Z axis position.
			//once player lands this will be used to determine how far he fell.
			if(!(movement_info.GetMovementFlags() & MOVEFLAG_FALLING))
				mover->z_axisposition = movement_info.GetMovementPosZ();
	}

	/************************************************************************/
	/* Transporter Setup                                                    */
	/************************************************************************/
	if( ( mover->transporter_info.guid != 0 ) && ( movement_info.GetTransportGUID().GetOldGuid() == 0 ) ){
		/* we left the transporter we were on */

		Transporter *transporter = objmgr.GetTransporter( Arcpro::Util::GUID_LOPART( mover->transporter_info.guid ) );
		if( transporter != NULL )
			transporter->RemovePassenger( mover );

		mover->transporter_info.guid = 0;
		_player->SpeedCheatReset();

	}else{
		if( movement_info.GetTransportGUID().GetOldGuid() != 0 ){

			if( mover->transporter_info.guid == 0 ){
				Transporter *transporter = objmgr.GetTransporter( Arcpro::Util::GUID_LOPART( movement_info.GetTransportGUID() ) );
				if( transporter != NULL )
					transporter->AddPassenger( mover );
				
				/* Set variables */
				mover->transporter_info.guid = movement_info.GetTransportGUID();
				mover->transporter_info.x = movement_info.GetTransportPosX();
				mover->transporter_info.y = movement_info.GetTransportPosY();
				mover->transporter_info.z = movement_info.GetTransportPosZ();
				mover->transporter_info.flags = movement_info.GetTransportTime();
				mover->transporter_info.flags2 = movement_info.GetTransportTime2();
			
			}else{
				/* No changes */
				mover->transporter_info.x = movement_info.GetTransportPosX();
				mover->transporter_info.y = movement_info.GetTransportPosY();
				mover->transporter_info.z = movement_info.GetTransportPosZ();
				mover->transporter_info.flags = movement_info.GetTransportTime();
				mover->transporter_info.flags2 = movement_info.GetTransportTime2();
			}
		}
	}
	
	/*float x = movement_info.x - movement_info.transX;
	float y = movement_info.y - movement_info.transY;
	float z = movement_info.z - movement_info.transZ;
	Transporter* trans = _player->m_CurrentTransporter;
	if(trans) sChatHandler.SystemMessageToPlr(_player, "Client t pos: %f %f\nServer t pos: %f %f   Diff: %f %f", x,y, trans->GetPositionX(), trans->GetPositionY(), trans->CalcDistance(x,y,z), trans->CalcDistance(movement_info.x, movement_info.y, movement_info.z));*/

	/************************************************************************/
	/* Anti-Speed Hack Checks                                               */
	/************************************************************************/
	// Blank?

	/************************************************************************/
	/* Breathing System                                                     */
	/************************************************************************/
	_HandleBreathing(movement_info, _player, this);

	/************************************************************************/
	/* Remove Spells                                                        */
	/************************************************************************/
	uint32 flags = 0;
	if((movement_info.GetMovementFlags() & MOVEFLAG_MOTION_MASK) != 0)
		flags |= AURA_INTERRUPT_ON_MOVEMENT;

	if(!(movement_info.GetMovementFlags() & MOVEFLAG_SWIMMING || movement_info.GetMovementFlags() & MOVEFLAG_FALLING))
		flags |= AURA_INTERRUPT_ON_LEAVE_WATER;
	if(movement_info.GetMovementFlags() & MOVEFLAG_SWIMMING)
		flags |= AURA_INTERRUPT_ON_ENTER_WATER;
	if((movement_info.GetMovementFlags() & MOVEFLAG_TURNING_MASK) || _player->isTurning)
		flags |= AURA_INTERRUPT_ON_TURNING;
	if(movement_info.GetMovementFlags() & MOVEFLAG_REDIRECTED)
		flags |= AURA_INTERRUPT_ON_JUMP;

	_player->RemoveAurasByInterruptFlag(flags);

	/************************************************************************/
	/* Update our position in the server.                                   */
	/************************************************************************/

	// Player is the active mover
	if(movement_info.GetMovementGUID().GetOldGuid() == _player->GetGUID())
	{

		if(_player->m_CurrentTransporter == NULL)
		{
			if(!_player->SetPosition(movement_info.GetMovementPosX(), movement_info.GetMovementPosY(), movement_info.GetMovementPosZ(), movement_info.GetOrientation()))
			{
				//extra check to set HP to 0 only if the player is dead (KillPlayer() has already this check)
				if(_player->isAlive())
				{
					_player->SetHealth(0);
					_player->KillPlayer();
				}

				MapInfo* pMapinfo = WorldMapInfoStorage.LookupEntry(_player->GetMapId());
				if(pMapinfo != NULL)
				{

					if(pMapinfo->type == INSTANCE_NULL || pMapinfo->type == INSTANCE_BATTLEGROUND)
					{
						_player->RepopAtGraveyard(_player->GetPositionX(), _player->GetPositionY(), _player->GetPositionZ(), _player->GetMapId());
					}
					else
					{
						_player->RepopAtGraveyard(pMapinfo->repopx, pMapinfo->repopy, pMapinfo->repopz, pMapinfo->repopmapid);
					}
				}
				else
				{
					_player->RepopAtGraveyard(_player->GetPositionX(), _player->GetPositionY(), _player->GetPositionZ(), _player->GetMapId());
				} //Teleport player to graveyard. Stops players from QQing..
			}
		}
	}
	else
	{
		if( !mover->isRooted() )
			mover->SetPosition(movement_info.GetMovementPosX(), movement_info.GetMovementPosY(), movement_info.GetMovementPosZ(), movement_info.GetOrientation());
	}
}

void WorldSession::HandleMoveTimeSkippedOpcode(WorldPacket & recv_data)
{

}

void WorldSession::HandleMoveNotActiveMoverOpcode(WorldPacket & recv_data)
{
	CHECK_INWORLD_RETURN

	WoWGuid guid;
	recv_data >> guid;

	if(guid == (GetMovementInfo()->GetMovementGUID().GetOldGuid()))
		return;

	if((guid != uint64(0)) && (guid == _player->GetCharmedUnitGUID()))
		GetMovementInfo()->Init(guid);
	else
		GetMovementInfo()->Init(_player->GetGUID());
		movement_info.Read(recv_data);
}


void WorldSession::HandleSetActiveMoverOpcode(WorldPacket & recv_data)
{
	CHECK_INWORLD_RETURN

	// set current movement object
	uint64 guid;
	recv_data >> guid;

	if(guid != (GetMovementInfo()->GetMovementGUID().GetOldGuid()))
	{
		// make sure the guid is valid and we aren't cheating
		if(!(_player->m_CurrentCharm == guid) &&
		        !(_player->GetGUID() == guid))
		{
			if( _player->GetCurrentVehicle()->GetOwner()->GetGUID() != guid )
				return;
		}

		// generate wowguid
		if(guid != 0)
			GetMovementInfo()->SetMovementGuid(guid);
		else
			GetMovementInfo()->SetMovementGuid(_player->GetGUID());
	}
}

void WorldSession::HandleMoveSplineCompleteOpcode(WorldPacket & recvPacket)
{

}

void WorldSession::HandleMountSpecialAnimOpcode(WorldPacket & recvdata)
{
	CHECK_INWORLD_RETURN

	WorldPacket data(SMSG_MOUNTSPECIAL_ANIM, 8);
	data << _player->GetGUID();
	_player->SendMessageToSet(&data, true);
}

void WorldSession::HandleWorldportOpcode(WorldPacket & recv_data)
{
	CHECK_INWORLD_RETURN

	uint32 unk;
	uint32 mapid;
	float x, y, z, o;
	recv_data >> unk >> mapid >> x >> y >> z >> o;

	//printf("\nTEST: %u %f %f %f %f", mapid, x, y, z, o);

	if(!HasGMPermissions())
	{
		SendNotification("You do not have permission to use this function.");
		return;
	}

	LocationVector vec(x, y, z, o);
	_player->SafeTeleport(mapid, 0, vec);
}

void WorldSession::HandleTeleportToUnitOpcode(WorldPacket & recv_data)
{
	CHECK_INWORLD_RETURN

	uint8 unk;
	Unit* target;
	recv_data >> unk;

	if(!HasGMPermissions())
	{
		SendNotification("You do not have permission to use this function.");
		return;
	}

	if((target = _player->GetMapMgr()->GetUnit(_player->GetSelection())) == NULL)
		return;

	_player->SafeTeleport(_player->GetMapId(), _player->GetInstanceID(), target->GetPosition());
}

void WorldSession::HandleTeleportCheatOpcode(WorldPacket & recv_data)
{

	CHECK_INWORLD_RETURN

	float x, y, z, o;
	LocationVector vec;

	if(!HasGMPermissions())
	{
		SendNotification("You do not have permission to use this function.");
		return;
	}

	recv_data >> x >> y >> z >> o;
	vec.ChangeCoords(x, y, z, o);
	_player->SafeTeleport(_player->GetMapId(), _player->GetInstanceID(), vec);
}

// Movement Info
MovementInfo::MovementInfo()
{
	MoverGuid = 0;
	MovementFlags = 0;
	MovementFlags2 = 0;
	PosX = PosY = PosZ = Orientation = 0.0f;
	MoveTime = 0;
	TransGuid = 0;
	TransSeat = 0;
	TransX = TransY = TransZ = TransO = 0.0f;
	TransTime = TransTime2 = 0;
	Pitch = 0.0f;
	FallTime = 0;
	JumpVelocity = JumpSinAngle = JumpCosAngle = JumpXYSpeed = 0.0f;
	SplineAngle = 0.0f;
	m_MovementCounter = 0;
}

MovementInfo::~MovementInfo()
{

}

// Reset our Data
void MovementInfo::Init(uint64 guid)
{
	MoverGuid = guid;
	MovementFlags = 0;
	MovementFlags2 = 0;
	PosX = PosY = PosZ = Orientation = 0.0f;
	MoveTime = 0;
	TransGuid = 0;
	TransSeat = 0;
	TransX = TransY = TransZ = TransO = 0.0f;
	TransTime = TransTime2 = 0;
	Pitch = 0.0f;
	FallTime = 0;
	JumpVelocity = JumpSinAngle = JumpCosAngle = JumpXYSpeed = 0.0f;
	SplineAngle = 0.0f;
	m_MovementCounter = 0;
}

void MovementInfo::Read(ByteBuffer data)
{
	data >> MovementFlags >> MovementFlags2 >> MoveTime;
	data >> PosX >> PosY >> PosZ >> Orientation;
	
	if(MovementFlags & MOVEFLAG_TRANSPORT)
	{
		data >> TransGuid;
		data >> TransX;
		data >> TransY;
		data >> TransZ;
		data >> TransO;
		data >> TransTime;
		data >> TransSeat;
		
		if(MovementFlag2 & MOVEFLAG2_INTERPOLATED_MOVE)
			data >> TransTime2;
	}

	if((MovementFlags & MOVEFLAG_SWIMMING|MOVEFLAG_AIR_SWIMMING) || (MovementFlags2 & MOVEFLAG2_ALLOW_PITCHING))
	{
		data >> Pitch;
	}

	if(MovementFlags2 & MOVEFLAG2_INTERPOLATED_TURN)
	{
		data >> FallTime;
		data >> JumpVelocity;

		if(MovementFlags & MOVEFLAG_REDIRECTED)
		{
			data >> JumpSinAngle;
			data >> JumpCosAngle;
			data >> JumpXYSpeed;
		}
	}

	if (MovementFlags & MOVEFLAG_SPLINE_MOVER)
	{
		data >> SplineAngle;
	} 
}

void MovementInfo::Write(ByteBuffer data, uint32 time)
{
	if(time == 0)
		time = getMSTime();
	
	printf("%u %u %f %f %f %f\n", MovementFlags, MovementFlags2, PosX, PosY, PosZ, Orientation);
	data << MovementFlags << MovementFlags2 << time;
	data << PosX << PosY << PosZ << Orientation;
	
	if(MovementFlags & MOVEFLAG_TRANSPORT)
	{
		data << TransGuid;
		data << TransX;
		data << TransY;
		data << TransZ;
		data << TransO;
		data << TransTime;
		data << TransSeat;
		
		if(MovementFlags2 & MOVEFLAG2_INTERPOLATED_MOVE)
			data << TransTime2;
	}
	
	if((MovementFlags & MOVEFLAG_SWIMMING|MOVEFLAG_AIR_SWIMMING) || (MovementFlags2 & MOVEFLAG2_ALLOW_PITCHING)) 
	{
		data << Pitch;
	}
	
	if(MovementFlags2 & MOVEFLAG2_INTERPOLATED_TURN)
	{
		data << FallTime;
		data << JumpVelocity;
		
		if(MovementFlags & MOVEFLAG_REDIRECTED)
		{
			data << JumpSinAngle;
			data << JumpCosAngle;
			data << JumpXYSpeed;
		}
	}
	
	if(MovementFlags & MOVEFLAG_SPLINE_MOVER)
	{
		data << SplineAngle;
	}
}

void MovementInfo::SetMovementPos(float x, float y, float z, float o)
{
	PosX = x;
	PosY = y;
	PosZ = z;
	Orentation = o;
	MoveTime = getMSTime();
}

void MovementInfo::SetTransportPosition(float x, float y, float z, float o)
{
	TransX = x;
	TransY = y;
	TransZ = z;
	TransO = o;
	TransTime = getMSTime();
}

// End MovementInfo Class