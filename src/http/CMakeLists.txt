add_library(cxxtools-http
	chunkedreader.cpp
	client.cpp
	clientimpl.cpp
	mapper.cpp
	messageheader.cpp
	notauthenticatedresponder.cpp
	notauthenticatedservice.cpp
	notfoundresponder.cpp
	notfoundservice.cpp
	parser.cpp
	request.cpp
	responder.cpp
	server.cpp
	serverimpl.cpp
	service.cpp
	socket.cpp
	worker.cpp
)

target_link_libraries(cxxtools-http cxxtools)

install(TARGETS cxxtools-http
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
