Releasenotes cxxtools 2.2.1
===========================

2.2.1 is a bugfix release. It fixes a major bug when parsing query parameters in http communication. Query parameters containing two percent signs resulted in an recursive loop, which results in a crash. Since query parameters are typically received from the network, it is a major problem, since there is no control over the input parameters sent.
