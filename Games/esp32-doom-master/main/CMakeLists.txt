idf_component_register(SRCS "app_main.c"
                    INCLUDE_DIRS "." "../components/prboom-esp32-compat/include" "../components/i2cdev" "../components/prboom/include"
	            EMBED_TXTFILES ../partitions.csv)
