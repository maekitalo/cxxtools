add_executable(arg arg.cpp)
target_link_libraries(arg cxxtools)

add_executable(arg-set arg-set.cpp)
target_link_libraries(arg-set cxxtools)

add_executable(commandoutput commandoutput.cpp)
target_link_libraries(commandoutput cxxtools)

add_executable(cxxlog cxxlog.cpp)
target_link_libraries(cxxlog cxxtools)

add_executable(deserialization deserialization.cpp)
target_link_libraries(deserialization cxxtools)

add_executable(dir dir.cpp)
target_link_libraries(dir cxxtools)

add_executable(dlloader dlloader.cpp)
target_link_libraries(dlloader cxxtools)

add_executable(execLs execLs.cpp)
target_link_libraries(execLs cxxtools)

add_executable(getini getini.cpp)
target_link_libraries(getini cxxtools)

add_executable(httpclient httpclient.cpp)
target_link_libraries(httpclient cxxtools cxxtools-http)

add_executable(httprequest httprequest.cpp)
target_link_libraries(httprequest cxxtools cxxtools-http)

add_executable(httpserver httpserver.cpp)
target_link_libraries(httpserver cxxtools cxxtools-http)

add_executable(insrv insrv.cpp)
target_link_libraries(insrv cxxtools)

add_executable(json json.cpp)
target_link_libraries(json cxxtools cxxtools-json)

add_executable(jsonbeautify jsonbeautify.cpp)
target_link_libraries(jsonbeautify cxxtools)

add_executable(log log.cpp)
target_link_libraries(log cxxtools)

add_executable(logsh logsh.cpp)
target_link_libraries(logsh cxxtools)

add_executable(md5sum md5sum.cpp)
target_link_libraries(md5sum cxxtools)

add_executable(mime mime.cpp)
target_link_libraries(mime cxxtools)

add_executable(mime2json mime2json.cpp)
target_link_libraries(mime2json cxxtools)

add_executable(mimeextract mimeextract.cpp)
target_link_libraries(mimeextract cxxtools)

add_executable(mimemultipart mimemultipart.cpp)
target_link_libraries(mimemultipart cxxtools)

add_executable(msgclient msgclient.cpp)
target_link_libraries(msgclient cxxtools cxxtools-bin)

add_executable(msgserver msgserver.cpp)
target_link_libraries(msgserver cxxtools cxxtools-bin)

add_executable(multifstream multifstream.cpp)
target_link_libraries(multifstream cxxtools)

add_executable(netcat netcat.cpp)
target_link_libraries(netcat cxxtools)

add_executable(netio netio.cpp)
target_link_libraries(netio cxxtools)

add_executable(netmsg netmsg.cpp)
target_link_libraries(netmsg cxxtools)

add_executable(pipestream pipestream.cpp)
target_link_libraries(pipestream cxxtools)

add_executable(pool pool.cpp)
target_link_libraries(pool cxxtools)

add_executable(regex regex.cpp)
target_link_libraries(regex cxxtools)

add_executable(rpcaddclient rpcaddclient.cpp)
target_link_libraries(rpcaddclient cxxtools-json cxxtools-bin cxxtools-xmlrpc)

add_executable(rpcasyncaddclient rpcasyncaddclient.cpp)
target_link_libraries(rpcasyncaddclient cxxtools-bin cxxtools-xmlrpc cxxtools-json)

add_executable(rpcechoclient rpcechoclient.cpp)
target_link_libraries(rpcechoclient cxxtools-json cxxtools-xmlrpc cxxtools-bin)

add_executable(rpcparallelecho rpcparallelecho.cpp)
target_link_libraries(rpcparallelecho cxxtools-bin)

add_executable(rpcserver rpcserver.cpp)
target_link_libraries(rpcserver cxxtools-json cxxtools-xmlrpc cxxtools-bin)

add_executable(saml2 saml2.cpp)
target_link_libraries(saml2 cxxtools)

add_executable(serialization serialization.cpp)
target_link_libraries(serialization cxxtools)

add_executable(signals signals.cpp)
target_link_libraries(signals cxxtools)

add_executable(splitter splitter.cpp)
target_link_libraries(splitter cxxtools)

add_executable(sslshowcert sslshowcert.cpp)
target_link_libraries(sslshowcert cxxtools)

add_executable(tcpsocket tcpsocket.cpp)
target_link_libraries(tcpsocket cxxtools)

add_executable(timer timer.cpp)
target_link_libraries(timer cxxtools)

add_executable(timer-lambda timer-lambda.cpp)
target_link_libraries(timer-lambda cxxtools)

add_executable(uuencode uuencode.cpp)
target_link_libraries(uuencode cxxtools)

add_executable(xmlmsgclient xmlmsgclient.cpp)
target_link_libraries(xmlmsgclient cxxtools)

add_executable(xmlmsgserver xmlmsgserver.cpp)
target_link_libraries(xmlmsgserver cxxtools)

add_executable(xmlreader xmlreader.cpp)
target_link_libraries(xmlreader cxxtools)

add_executable(dynamicrpc dynamicrpc.cpp)
target_link_libraries(dynamicrpc cxxtools cxxtools-bin)

add_executable(dynamiccall dynamiccall.cpp)
target_link_libraries(dynamiccall cxxtools cxxtools-bin)

add_executable(rpcproxy rpcproxy.cpp)
target_link_libraries(rpcproxy cxxtools cxxtools-bin)
