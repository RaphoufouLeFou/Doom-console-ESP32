idf_component_register(SRCS "Render.c" "app_main.c" "LCD_startup.c"
                    INCLUDE_DIRS "." "../components/prboom-esp32-compat/include" "../components" "../components/i2cdev" "../components/prboom/include"
	            EMBED_TXTFILES ../partitions.csv)
set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)