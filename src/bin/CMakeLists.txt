add_library(cxxtools-bin
	deserializer.cpp
	formatter.cpp
	parser.cpp
	responder.cpp
	rpcclient.cpp
	rpcclientimpl.cpp
	rpcserver.cpp
	rpcserverimpl.cpp
	scanner.cpp
	serializer.cpp
	socket.cpp
	worker.cpp
)

target_link_libraries(cxxtools-bin cxxtools)

install(TARGETS cxxtools-bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
