idf_component_register(SRCS "Render.c" "app_main.c" "LCD_startup.c"
                    INCLUDE_DIRS "." "../components/prboom-esp32-compat/include" "../components" "../components/i2cdev" "../components/prboom/include" "/home/raphael/esp/esp-idf/examples/common_components/protocol_examples_common/include" 
	            EMBED_TXTFILES ../partitions.csv)