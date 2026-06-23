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

install(TARGETS cxxtools-xmlrpc
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
