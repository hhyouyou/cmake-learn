add_library(MathFunctions MySqrt.cpp)

install (TARGETS MathFunctions DESTINATION bin)
install (FILES MathFunctions.h DESTINATION include)

include(CheckFunctionExists)
check_function_exists(log "math.h" HAVE_LOG)
check_function_exists(exp "math.h" HAVE_EXP)
if(NOT (HAVE_LOG AND HAVE_EXP))
	unset(HAVE_LOG CACHE)
	unset(HAVE_EXP CACHE)
	set(CMAKE_REQUIRED_LIBRARIES "m")
	check_function_exists(log "math.h" HAVE_LOG)
	check_function_exists(exp "math.h" HAVE_EXP)
	if(HAVE_LOG AND HAVE_EXP)
		target_link_libraries(MathFunctions PRIVATE m)
	endif()
endif()

if(HAVE_LOG AND HAVE_EXP)
  target_compile_definitions(MathFunctions PRIVATE "HAVE_LOG" "HAVE_EXP")
endif()