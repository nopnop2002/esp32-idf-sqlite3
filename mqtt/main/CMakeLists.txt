set(srcs "main.c" "sqlite.c" "mqtt_pub.c" "mqtt_sub.c")

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "." EMBED_TXTFILES root_cert.pem)
