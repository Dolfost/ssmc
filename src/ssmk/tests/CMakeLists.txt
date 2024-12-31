file(GLOB TESTS CONFIGURE_DEPENDS
	*.cpp 
)

foreach(T ${TESTS})
	cmake_path(GET T STEM TNAME)
	add_executable(ssmk_${TNAME} 
		${T}
	)
	target_compile_definitions(ssmk_${TNAME} 
		PRIVATE TESTPATH="${CMAKE_CURRENT_LIST_DIR}"
		PRIVATE TESTFILE="${T}"
	)
	target_link_libraries(ssmk_${TNAME} 
		sm::ssmk
	)
	add_test(NAME ssmk_${TNAME}
		COMMAND ssmk_${TNAME}
	)
endforeach()
