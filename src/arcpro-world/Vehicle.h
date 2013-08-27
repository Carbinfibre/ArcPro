/*
 * ArcPro MMORPG Server
 * Copyright (c) 2011-2013 ArcPro Speculation <http://arcpro.sexyi.am/>
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

#ifndef __VEHICLE_H
#define __VEHICLE_H

/*enum VehicleSeatFlags
{
    VEHICLE_SEAT_FLAG_HAS_LOWER_ANIM_FOR_ENTER                         = 0x00000001,
    VEHICLE_SEAT_FLAG_HAS_LOWER_ANIM_FOR_RIDE                          = 0x00000002,
    VEHICLE_SEAT_FLAG_UNK3                                             = 0x00000004,
    VEHICLE_SEAT_FLAG_SHOULD_USE_VEH_SEAT_EXIT_ANIM_ON_VOLUNTARY_EXIT  = 0x00000008,
    VEHICLE_SEAT_FLAG_UNK5                                             = 0x00000010,
    VEHICLE_SEAT_FLAG_UNK6                                             = 0x00000020,
    VEHICLE_SEAT_FLAG_UNK7                                             = 0x00000040,
    VEHICLE_SEAT_FLAG_UNK8                                             = 0x00000080,
    VEHICLE_SEAT_FLAG_UNK9                                             = 0x00000100,
    VEHICLE_SEAT_FLAG_HIDE_PASSENGER                                   = 0x00000200, // Passenger is hidden
    VEHICLE_SEAT_FLAG_ALLOW_TURNING                                    = 0x00000400, // needed for CGCamera__SyncFreeLookFacing
    VEHICLE_SEAT_FLAG_CAN_CONTROL                                      = 0x00000800, // Lua_UnitInVehicleControlSeat
    VEHICLE_SEAT_FLAG_CAN_CAST_MOUNT_SPELL                             = 0x00001000, // Can cast spells with SPELL_AURA_MOUNTED from seat (possibly 4.x only, 0 seats on 3.3.5a)
    VEHICLE_SEAT_FLAG_UNCONTROLLED                                     = 0x00002000, // can override !& VEHICLE_SEAT_FLAG_CAN_ENTER_OR_EXIT
    VEHICLE_SEAT_FLAG_CAN_ATTACK                                       = 0x00004000, // Can attack, cast spells and use items from vehicle
    VEHICLE_SEAT_FLAG_SHOULD_USE_VEH_SEAT_EXIT_ANIM_ON_FORCED_EXIT     = 0x00008000,
    VEHICLE_SEAT_FLAG_UNK17                                            = 0x00010000,
    VEHICLE_SEAT_FLAG_UNK18                                            = 0x00020000, // Needs research and support (28 vehicles): Allow entering vehicles while keeping specific permanent(?) auras that impose visuals (states like beeing under freeze/stun mechanic, emote state animations).
    VEHICLE_SEAT_FLAG_HAS_VEH_EXIT_ANIM_VOLUNTARY_EXIT                 = 0x00040000,
    VEHICLE_SEAT_FLAG_HAS_VEH_EXIT_ANIM_FORCED_EXIT                    = 0x00080000,
    VEHICLE_SEAT_FLAG_PASSENGER_NOT_SELECTABLE                         = 0x00100000,
    VEHICLE_SEAT_FLAG_UNK22                                            = 0x00200000,
    VEHICLE_SEAT_FLAG_REC_HAS_VEHICLE_ENTER_ANIM                       = 0x00400000,
    VEHICLE_SEAT_FLAG_IS_USING_VEHICLE_CONTROLS                        = 0x00800000, // Lua_IsUsingVehicleControls
    VEHICLE_SEAT_FLAG_ENABLE_VEHICLE_ZOOM                              = 0x01000000,
    VEHICLE_SEAT_FLAG_CAN_ENTER_OR_EXIT                                = 0x02000000, // Lua_CanExitVehicle - can enter and exit at free will
    VEHICLE_SEAT_FLAG_CAN_SWITCH                                       = 0x04000000, // Lua_CanSwitchVehicleSeats
    VEHICLE_SEAT_FLAG_HAS_START_WARITING_FOR_VEH_TRANSITION_ANIM_ENTER = 0x08000000,
    VEHICLE_SEAT_FLAG_HAS_START_WARITING_FOR_VEH_TRANSITION_ANIM_EXIT  = 0x10000000,
    VEHICLE_SEAT_FLAG_CAN_CAST                                         = 0x20000000, // Lua_UnitHasVehicleUI
    VEHICLE_SEAT_FLAG_UNK2                                             = 0x40000000, // checked in conjunction with 0x800 in CastSpell2
    VEHICLE_SEAT_FLAG_ALLOWS_INTERACTION                               = 0x80000000
};

enum VehicleSeatFlagsB
{
    VEHICLE_SEAT_FLAG_B_NONE                     = 0x00000000,
    VEHICLE_SEAT_FLAG_B_USABLE_FORCED            = 0x00000002,
    VEHICLE_SEAT_FLAG_B_TARGETS_IN_RAIDUI        = 0x00000008,           // Lua_UnitTargetsVehicleInRaidUI
    VEHICLE_SEAT_FLAG_B_EJECTABLE                = 0x00000020,           // ejectable
    VEHICLE_SEAT_FLAG_B_USABLE_FORCED_2          = 0x00000040,
    VEHICLE_SEAT_FLAG_B_USABLE_FORCED_3          = 0x00000100,
    VEHICLE_SEAT_FLAG_B_USABLE_FORCED_4          = 0x02000000,
    VEHICLE_SEAT_FLAG_B_CAN_SWITCH               = 0x04000000,
    VEHICLE_SEAT_FLAG_B_VEHICLE_PLAYERFRAME_UI   = 0x80000000            // Lua_UnitHasVehiclePlayerFrameUI - actually checked for flagsb &~ 0x80000000
};*/

//////////////////////////////////////////////////////
//class VehicleSeat
//  Implements the seat functionality for Vehicles
//
//
//////////////////////////////////////////////////////
class VehicleSeat{
public:
	VehicleSeat( VehicleSeatEntry *info ){
		passenger = 0;
		seat_info = info;
	}


	/////////////////////////////////////////////////////
	//bool HasPassenger()
	//  Tells if the seat is occupied or not.
	//
	//Parameter(s)
	//  None
	//
	//Return Value
	// Returns true if the seat is occupied.
	// Returns false if the seat is empty.
	//
	//
	/////////////////////////////////////////////////////
	bool HasPassenger() const{
		if( passenger == 0 )
			return false;
		else
			return true;
	}


	/////////////////////////////////////////////////////
	//bool AddPassenger( uint64 passenger_guid )
	//  Adds a passenger to the seat
	//
	//Parameter(s)
	//  uint64 passenger_guid  -  GUID of the passenger
	//
	//Return Value
	//  Returns true on success.
	//  Returns false on failure.
	//
	//
	/////////////////////////////////////////////////////
	bool AddPassenger( uint64 passenger_guid ){
		if( HasPassenger() )
			return false;
		else
			passenger = passenger_guid;

		return true;
	}


	/////////////////////////////////////////////////////
	//uint64 GetPassengerGUID() const
	//  Returns the GUID of the passenger in the seat
	//
	//Parameter(s)
	//  None
	//
	//Return Value
	//  Returns the GUID of the passenger in the seat, if any
	//  Return 0 otherwise
	//
	//
	/////////////////////////////////////////////////////
	uint64 GetPassengerGUID() const{
		return passenger;
	}


	/////////////////////////////////////////////////////
	//void RemovePassenger()
	//  Removes the passenger from the seat
	//
	//Parameter(s)
	//  None
	//
	//Return Value
	//  None
	//
	//
	/////////////////////////////////////////////////////
	void RemovePassenger(){
		passenger = 0;
	}


	/////////////////////////////////////////////////////
	//VehicleSeatEntry* GetSeatInfo()
	//  Retrieves the seat information structure
	//
	//Parameter(s)
	//  None
	//
	//Return Value
	// Returns a pointer to a VehicleSeatEntry structure.
	//
	//
	/////////////////////////////////////////////////////
	VehicleSeatEntry* GetSeatInfo() const{
		return seat_info;
	}


	////////////////////////////////////////////////////
	//bool Usable()
	//  Tells if the seat is usable for passengers
	//
	//Parameter(s)
	//  None
	//
	//Return Value
	//  Returns true if the seat is usable.
	//  Returns false otherwise.
	//
	//
	///////////////////////////////////////////////////
	bool Usable() const{
		return seat_info->IsUsable();
	}


	///////////////////////////////////////////////////
	//bool Controller() const
	//  Tells if the seat can control the vehicle
	//
	//Parameter(s)
	//  None
	//
	//Return Value
	//  Returns true if the seat can control.
	//  Returns false otherwise.
	//
	//
	///////////////////////////////////////////////////
	bool Controller() const{
		return seat_info->IsController();
	}


	///////////////////////////////////////////////////
	//bool HidesPassenger() const
	//  Tells if the seat hides the passenger
	//
	//Parameter(s)
	//  None
	//
	//Return Value
	//  Returns true if the passenger is hidden in this seat
	//  Return false otherwise
	//
	//
	///////////////////////////////////////////////////
	bool HidesPassenger() const{
		return seat_info->HidesPassenger();
	}

private:
	uint64 passenger;              // GUID of the passenger
	VehicleSeatEntry *seat_info;   // Seat info structure
};


/////////////////////////////////////////////
//class Vehicle
//  Implements vehicles in the game
//
////////////////////////////////////////////
class SERVER_DECL Vehicle{
public:
	Vehicle();


	~Vehicle();


	///////////////////////////////////////////////////////////////////////////
	//void Load( Unit *owner, uint32 creature_entry, uint32 vehicleid )
	//  Loads and sets up the Vehicle
	//
	//Parameter(s)
	//  Unit *owner           -  Pointer to the Unit this vehicle belongs to
	//  uint32 creature_entry -  Host creature of the Vehicle
	//  uint32 vehicleid      -  Index of Vehicle.dbc
	//
	//Return Value
	//  None
	//
	//
	//////////////////////////////////////////////////////////////////////////
	void Load( Unit *owner, uint32 creature_entry, uint32 vehicleid );

	
	//////////////////////////////////////////////////////////////////////////
	//bool HasEmptySeat()
	//  Tells if the Vehicle has at least 1 empty seat.
	//
	//Parameter(s)
	//  None
	//
	//Return Value
	//  Returns true if there's at least 1 empty seat.
	//  Returns false otherwise.
	//
	//
	//////////////////////////////////////////////////////////////////////////
	bool HasEmptySeat();


	/////////////////////////////////////////////////////////////////////////
	//void AddPassenger( Unit *passenger )
	//  Adds a passenger to the vehicle
	//
	//Parameter(s)
	//  Unit *passenger  -  Passenger to add
	//
	//Return Value(s)
	//  None
	//
	//
	/////////////////////////////////////////////////////////////////////////
	void AddPassenger( Unit *passenger );

	
	////////////////////////////////////////////////////////////////////////
	//void AddPassengerToSeat( Unit *passenger, uint32 seatid )
	//  Adds the passenger to the specified seat if possible
	//
	//Parameter(s)
	//  Unit *passenger  -  Pointer to the passenger we want to add
	//  uint32 seatid    -  The id of the seat we want the passenger to be added
	//
	//Return Value
	//  None
	//
	//
	////////////////////////////////////////////////////////////////////////
	void AddPassengerToSeat( Unit *passenger, uint32 seatid );


	/////////////////////////////////////////////////////////////////////////
	//void EjectPassenger( Unit *passenger )
	//  Remove the passenger from the vehicle
	//
	//Parameter(s)
	//  Unit *passenger  -  Passenger to remove.
	//
	//Return Value
	//  None
	//
	//
	/////////////////////////////////////////////////////////////////////////
	void EjectPassenger( Unit *passenger );


	/////////////////////////////////////////////////////////////////////////
	//void EjectPassengerFromSeat( uint32 seatid )
	//  Ejects the passenger from this seat.
	//
	//Parameter(s)
	//  uint32 seatid  -  Identifier of the seat we want to evict
	//
	//Return Value
	// None
	//
	//
	/////////////////////////////////////////////////////////////////////////
	void EjectPassengerFromSeat( uint32 seatid );


	/////////////////////////////////////////////////////////////////////////
	//void EjectAllPassengers()
	//  Ejects all passengers from the vehicle
	//
	//Parameter(s)
	// None
	//
	//Return Value
	// None
	//
	//
	/////////////////////////////////////////////////////////////////////////
	void EjectAllPassengers();


	/////////////////////////////////////////////////////////////////////////
	//void MovePassengerToSeat( Unit *passenger, uint32 seat )
	//  Moves the passenger to the specified seat
	//
	//Parameter(s)
	//  Unit *passenger  -  The passenger we want to move
	//  uint32 seat      -  The seat where we want to move this passenger to
	//
	//Return Value
	//  None
	//
	//
	/////////////////////////////////////////////////////////////////////////
	void MovePassengerToSeat( Unit *passenger, uint32 seat );

	
	////////////////////////////////////////////////////////////////////////
	//void MovePassengerToNextSeat( Unit *passenger )
	//  Moves the specified passenger to the next seat
	//
	//Parameter(s)
	//  Unit *passenger  -  Pointer to the passnger we want to move
	//
	//Return Value
	//  None
	//
	//
	////////////////////////////////////////////////////////////////////////
	void MovePassengerToNextSeat( Unit *passenger );


	////////////////////////////////////////////////////////////////////////
	//void MovePassengerToPrevSeat( Unit *passenger )
	//  Moves the specified passenger to the previous seat
	//
	//Parameter(s)
	//  Unit *passenger  -  Pointer to the passenger we want to move
	//
	//Return Value
	//  None
	//
	//
	////////////////////////////////////////////////////////////////////////
	void MovePassengerToPrevSeat( Unit *passenger );


	////////////////////////////////////////////////////////////////////////
	//uint32 GetSeatEntryForPassenger( Unit *passenger )
	//  Retrieves the seat entry ID for this passenger
	//
	//Parameter(s)
	//  Unit *passenger  -  Pointer to the passenger
	//
	//Return Value
	//  Returns the entry ID of the seat this passenger occupies
	//
	//
	////////////////////////////////////////////////////////////////////////
	uint32 GetSeatEntryForPassenger( Unit *passenger );


	/////////////////////////////////////////////////////////////////////////
	//void MovePassengers( float x, float y, float z, float o )
	//  Moves the passengers on/in the vehicle to the specified coordinates
	//
	//Parameter(s)
	//  float x  -  destination X coordinate
	//  float y  -  destination Y coordinate
	//  float z  -  destination Z coordinate ( height )
	//  float o  -  destination orientation
	//
	//Return Value
	//  None
	//
	//
	/////////////////////////////////////////////////////////////////////////
	void MovePassengers( float x, float y, float z, float o );


	/////////////////////////////////////////////////////////////////////////
	//uint32 GetPassengerCount() const
	//  Tells the number of passengers currently in the vehicle
	//
	//Parameter(s)
	//  None
	//
	//Return Value
	//  Returns the number of passengers.
	//
	//
	/////////////////////////////////////////////////////////////////////////
	uint32 GetPassengerCount() const;

	
	/////////////////////////////////////////////////////////////////////////
	//Unit* GetOwner() const
	//  Retrieves a pointer to the owner of the vehicle.
	//  The owner is the unit which has this vehicle component.
	//
	//Parameter(s)
	//  None
	//
	//Return Value
	// Returns a pointer to the unit which owns this vehicle component.
	//
	//
	/////////////////////////////////////////////////////////////////////////
	Unit* GetOwner() const{ return owner; }


	/////////////////////////////////////////////////////////////////////////
	//uint16 GetMoveFlags2() const
	//  Retrieves the Move2 flags for this Vehicle
	//
	//Parameter(s)
	//  None
	//
	//Return Value
	//  Returns the extra movement flags for this vehicle
	//
	//
	/////////////////////////////////////////////////////////////////////////
	uint16 GetMoveFlags2() const;


	/////////////////////////////////////////////////////////////////////////
	//void InstallAccessories()
	//  Installs all accessories for this vehicle ( turrets for example )
	//
	//Parameter(s)
	//  None
	//
	//Return Value
	//  None
	//
	//
	/////////////////////////////////////////////////////////////////////////
	void InstallAccessories();


	/////////////////////////////////////////////////////////////////////////
	//void RemoveAccessories()
	//  Removes all installed vehicle accessories
	//
	//Parameter(s)
	//  None
	//
	//Return Value
	//  None
	//
	//
	/////////////////////////////////////////////////////////////////////////
	void RemoveAccessories();


	////////////////////////////////////////////////////////////////////////
	//bool HasAccessoryWithGUID( uint64 guid )
	//  Tells if this vehicle has the specified accessory attached to it
	//
	//Parameter(s)
	//  uint64 guid  -  GUID of the accessory
	//
	//Return Value
	//  Returns true if the vehicle has the accessory attached.
	//  Returns false otherwise.
	//
	////////////////////////////////////////////////////////////////////////
	bool HasAccessoryWithGUID( uint64 guid );

private:
	std::tr1::array< VehicleSeat*, MAX_VEHICLE_SEATS > seats;
	std::vector< uint64 > installed_accessories;
	uint32 creature_entry;
	Unit *owner;
	VehicleEntry *vehicle_info;
	uint32 passengercount;
	uint32 freeseats;
};

struct VehicleAccessoryEntry{
	uint32 accessory_entry;
	uint32 seat;
};


#endif
