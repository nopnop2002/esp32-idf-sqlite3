set(COMPONENT_ADD_INCLUDEDIRS "include")

idf_component_register(SRCS "sqlite3.c" "sqllib.c" "esp32.c" "shox96_0_2.c"
                       INCLUDE_DIRS "include"
                       PRIV_INCLUDE_DIRS "private_include"
                       REQUIRES mbedtls
                       PRIV_REQUIRES console spiffs spi_flash esp_timer)


if(CONFIG_IDF_TARGET_ARCH_XTENSA)
target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu99 -g3 -fno-stack-protector -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls -nostdlib -Wpointer-arith -Wno-error=unused-value -Wno-error=unused-label -Wno-error=unused-function -Wno-error=unused-but-set-variable -Wno-error=unused-variable -Wno-error=deprecated-declarations -Wno-error=char-subscripts -Wno-error=maybe-uninitialized -Wno-unused-parameter -Wno-sign-compare -Wno-old-style-declaration -Wno-cast-function-type -Wno-unused-function -Wno-unused-variable -Wno-implicit-fallthrough -Wno-unused-but-set-variable -Wno-extra -Wno-char-subscripts -Wno-unused-label -Wno-unused-value -MMD -c -DF_CPU=240000000L -DESP32 -DCORE_DEBUG_LEVEL=0 -DNDEBUG)
else()
target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu99 -g3 -fno-stack-protector -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -nostdlib -Wpointer-arith -Wno-error=unused-value -Wno-error=unused-label -Wno-error=unused-function -Wno-error=unused-but-set-variable -Wno-error=unused-variable -Wno-error=deprecated-declarations -Wno-error=char-subscripts -Wno-error=maybe-uninitialized -Wno-unused-parameter -Wno-sign-compare -Wno-old-style-declaration -Wno-cast-function-type -Wno-unused-function -Wno-unused-variable -Wno-implicit-fallthrough -Wno-unused-but-set-variable -Wno-extra -Wno-char-subscripts -Wno-unused-label -Wno-unused-value -MMD -c -DF_CPU=240000000L -DESP32 -DCORE_DEBUG_LEVEL=0 -DNDEBUG)
endif()
