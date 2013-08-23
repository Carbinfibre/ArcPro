if(NOT BUILD_EXTRASCRIPTS )
  message("\tExtraScripts was skipped because BUILD_EXTRASCRIPTS is unchecked.")
  return()
endif(NOT BUILD_EXTRASCRIPTS)

project(ExtraScripts)

SET( prefix ${SCRIPTS_PATH}/${PROJECT_NAME} )
#SRCS - contains the source files that compose this project.

set( sources
Setup.cpp
alterac_mountains.cpp
arathi_highlands.cpp
ashenvale.cpp
azshara.cpp
azuremyst_isle.cpp
blades_edge_mountains.cpp
blasted_lands.cpp
bloodmyst_isle.cpp
borean_tundra.cpp
boss_azuregos.cpp
boss_doomlord_kazzak.cpp
boss_doomwalker.cpp
burning_steppes.cpp
crystalsong_forest.cpp
dalaran.cpp
darkshore.cpp
desolace.cpp
dragonblight.cpp
durotar.cpp
duskwood.cpp
dustwallow_marsh.cpp
eastern_plaguelands.cpp
eversong_woods.cpp
felwood.cpp
feralas.cpp
ghostlands.cpp
grizzly_hills.cpp
hellfire_peninsula.cpp
hinterlands.cpp
howling_fjord.cpp
icecrown.cpp
ironforge.cpp
isle_of_conquest.cpp
isle_of_queldanas.cpp
loch_modan.cpp
moonglade.cpp
nagrand.cpp
netherstorm.cpp
orgrimmar.cpp
redridge_mountains.cpp
shadowmoon_valley.cpp
shattrath_city.cpp
sholazar_basin.cpp
silithus.cpp
silvermoon_city.cpp
silverpine_forest.cpp
stonetalon_mountains.cpp
storm_peaks.cpp
stormwind_city.cpp
stranglethorn_vale.cpp
swamp_of_sorrows.cpp
tanaris.cpp
teldrassil.cpp
terokkar_forest.cpp
the_barrens.cpp
thousand_needles.cpp
thunder_bluff.cpp
tirisfal_glades.cpp
undercity.cpp
ungoro_crater.cpp
western_plaguelands.cpp
westfall.cpp
wetlands.cpp
wintergrasp.cpp
winterspring.cpp
zangarmarsh.cpp
zuldrak.cpp
NeutralGuards.cpp )

SET( headers
Setup.h )

foreach(src IN ITEMS ${sources} )
  set( SRCS ${SRCS} ${prefix}/${src} )
endforeach(src)
foreach(src IN ITEMS ${headers} )
  SET( HEADERS ${HEADERS} ${prefix}/${src} )
endforeach(src)

include_directories( ${include_dirs} )
add_library( ${PROJECT_NAME} SHARED ${SRCS} ${HEADERS} )
add_dependencies( ${PROJECT_NAME} ${dependancies} )
target_link_libraries( ${PROJECT_NAME} ${linkages} )

include(PCHSupport)
IF( CMAKE_GENERATOR MATCHES Visual*)
	ADD_NATIVE_PRECOMPILED_HEADER(${PROJECT_NAME} Setup.h ${prefix}/Setup.cpp)
ENDIF()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${ARCPRO_SCRIPTLIB_PATH} LIBRARY DESTINATION ${ARCPRO_SCRIPTLIB_PATH})
#Cleap up our variables(Not required.)
unset( prefix)
unset( SRCS )
unset( sources )
