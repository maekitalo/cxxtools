add_library(cxxtools-unit
	application.cpp
	assertion.cpp
	jsonreporter.cpp
	reporter.cpp
	testcase.cpp
	testcontext.cpp
	test.cpp
	testprotocol.cpp
	testsuite.cpp
)

target_link_libraries(cxxtools-unit cxxtools)
