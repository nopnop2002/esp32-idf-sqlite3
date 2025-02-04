set(srcs "main.c" "sqlite.c")

if(CONFIG_REDIRECT_TO_UDP)
	list(APPEND srcs "udp_client.c")
elseif(CONFIG_REDIRECT_TO_MQTT)
	list(APPEND srcs "mqtt_pub.c")
endif()

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "." EMBED_TXTFILES root_cert.pem)

