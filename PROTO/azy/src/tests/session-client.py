import xmlrpclib

#
# Yeah, xmlrpclib is crap, but we still need to set http headers anyway...
#

class MySafeTransport(xmlrpclib.SafeTransport):
    def get_host_info(self, host):
        host, extra_headers, x509 = xmlrpclib.SafeTransport.get_host_info(self, host)
        extra_headers = [("X-SESSION-ID", "1"), ("X-SESSION-USE", "1")]
        return host, extra_headers, x509

s = xmlrpclib.ServerProxy("https://localhost:4444/TTest2", MySafeTransport())

#
# Call methods
#

print s.auth("root", "qwe")
print s.getUsername()
