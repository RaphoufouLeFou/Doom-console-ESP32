set(srcs "MC_main.c"
        "draw.c"
    )

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "."
                   )
