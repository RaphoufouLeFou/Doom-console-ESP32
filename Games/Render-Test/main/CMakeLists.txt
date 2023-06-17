set(srcs "spi_master_example_main.c"
    "main.c"
    )

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "." "../components/McESP/"
                    )
