# Specify the maximum PreCompiled Header memory allocation limit
# Fixes a compiler-problem when using PCH - the /Ym flag is adjusted by the compiler in MSVC2012, hence we need to set an upper limit with /Zm to avoid discrepancies)
# (And yes, this is a verified , unresolved bug with MSVC... *sigh*)
string(REGEX REPLACE "/Zm[0-9]+ *" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
if(MSVC_VERSION LESS 1800) # MSVC2012 and below
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zm500" CACHE STRING "" FORCE)
endif()