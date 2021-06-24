set(formatList
	JCGenXML-static
	JCGenLua-static)

set(test_name )
NEWTEST(parse-xml "test.cpp" test_name)

foreach(format IN LISTS formatList)
	target_link_libraries(${test_name} PRIVATE ${format})
endforeach()
