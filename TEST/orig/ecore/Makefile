FLAGS = `pkg-config ecore ecore-evas ecore-job ecore-config ecore-x evas --cflags`
LIBS  = `pkg-config ecore ecore-evas ecore-job ecore-config ecore-x evas --libs`

BINS  = ecore_config ecore_evas_test ecore_test con_client_udp_example con_mcast_example con_server_udp_example con_tcp_example con_udp_example con_tcp_client_example con_udp_client_example
all: $(BINS)

ecore_config_SRCS = ecore_config.c
ecore_config_OBJS = $(ecore_config_SRCS:.c=.o)
ecore_config: $(ecore_config_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAGS) $(ecore_config_OBJS) -o $@

ecore_evas_test_SRCS = ecore_evas_test.c ecore_evas_test_app.c ecore_evas_test_bg.c ecore_evas_test_calibrate.c
ecore_evas_test_OBJS = $(ecore_evas_test_SRCS:.c=.o)
ecore_evas_test: $(ecore_evas_test_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAGS) $(ecore_evas_test_OBJS) -o $@

ecore_test_SRCS = ecore_test.c
ecore_test_OBJS = $(ecore_test_SRCS:.c=.o)
ecore_test: $(ecore_test_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAGS) $(ecore_test_OBJS) -o $@

ecore_dbus_test_SRCS = ecore_dbus_test.c
ecore_dbus_test_OBJS = $(ecore_dbus_test_SRCS:.c=.o)
ecore_dbus_test: $(ecore_dbus_test_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAGS) $(ecore_dbus_test_OBJS) -o $@

ecore_dbus_hal_test_SRCS = ecore_dbus_hal_test.c
ecore_dbus_hal_test_OBJS = $(ecore_dbus_hal_test_SRCS:.c=.o)
ecore_dbus_hal_test: $(ecore_dbus_hal_test_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAGS) $(ecore_dbus_hal_test_OBJS) -o $@

ecore_dbus_receiver_test_SRCS = ecore_dbus_receiver_test.c
ecore_dbus_receiver_test_OBJS = $(ecore_dbus_receiver_test_SRCS:.c=.o)
ecore_dbus_receiver_test: $(ecore_dbus_receiver_test_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAGS) $(ecore_dbus_receiver_test_OBJS) -o $@

con_client_udp_example_SRCS = con_client_udp_example.c
con_client_udp_example_OBJS = $(con_client_udp_example_SRCS:.c=.o)
con_client_udp_example: $(con_client_udp_example_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAGS) $(con_client_udp_example_OBJS) -o $@

con_server_udp_example_SRCS = con_server_udp_example.c
con_server_udp_example_OBJS = $(con_server_udp_example_SRCS:.c=.o)
con_server_udp_example: $(con_server_udp_example_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAGS) $(con_server_udp_example_OBJS) -o $@

con_mcast_example_SRCS = con_mcast_example.c
con_mcast_example_OBJS = $(con_mcast_example_SRCS:.c=.o)
con_mcast_example: $(con_mcast_example_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAGS) $(con_mcast_example_OBJS) -o $@

con_tcp_example_SRCS = con_tcp_example.c
con_tcp_example_OBJS = $(con_tcp_example_SRCS:.c=.o)
con_tcp_example: $(con_tcp_example_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAFS) $(con_tcp_example_OBJS) -o $@

con_udp_example_SRCS = con_udp_example.c
con_udp_example_OBJS = $(con_udp_example_SRCS:.c=.o)
con_udp_example: $(con_udp_example_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAFS) $(con_udp_example_OBJS) -o $@

con_tcp_client_example_SRCS = con_tcp_client_example.c
con_tcp_client_example_OBJS = $(con_tcp_client_example_SRCS:.c=.o)
con_tcp_client_example: $(con_tcp_client_example_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAFS) $(con_tcp_client_example_OBJS) -o $@

con_udp_client_example_SRCS = con_udp_client_example.c
con_udp_client_example_OBJS = $(con_udp_client_example_SRCS:.c=.o)
con_udp_client_example: $(con_udp_client_example_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAFS) $(con_udp_client_example_OBJS) -o $@

con_tcp_client_ssl_example_SRCS = con_tcp_client_ssl_example.c
con_tcp_client_ssl_example_OBJS = $(con_tcp_client_ssl_example_SRCS:.c=.o)
con_tcp_client_ssl_example: $(con_tcp_client_ssl_example_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAFS) $(con_tcp_client_ssl_example_OBJS) -o $@

con_tcp_ssl_example_SRCS = con_tcp_ssl_example.c
con_tcp_ssl_example_OBJS = $(con_tcp_ssl_example_SRCS:.c=.o)
con_tcp_ssl_example: $(con_tcp_ssl_example_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAFS) $(con_tcp_ssl_example_OBJS) -o $@

############################################################################
#### boilerplate
.c.o:
	$(CC) $(FLAGS) $(CFLAGS) -c $< -o $@
clean::
	rm -rf $(BINS) *.o *~
