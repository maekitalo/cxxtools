add_library(cxxtools-xmlrpc
	client.cpp
	clientimpl.cpp
	formatter.cpp
	httpclient.cpp
	httpclientimpl.cpp
	responder.cpp
	scanner.cpp
	service.cpp
)

target_link_libraries(cxxtools-xmlrpc cxxtools cxxtools cxxtools-http)
