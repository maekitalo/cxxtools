add_library(cxxtools-json
	httpclient.cpp
	httpclientimpl.cpp
	httpresponder.cpp
	httpservice.cpp
	responder.cpp
	rpcclient.cpp
	rpcclientimpl.cpp
	rpcserver.cpp
	rpcserverimpl.cpp
	scanner.cpp
	socket.cpp
	worker.cpp
)

target_link_libraries(cxxtools-json cxxtools cxxtools-http)
