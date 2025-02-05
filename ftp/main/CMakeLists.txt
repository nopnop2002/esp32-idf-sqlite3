set(srcs "main.c" "sqlite.c" "ftp_client.c")

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS ".")

