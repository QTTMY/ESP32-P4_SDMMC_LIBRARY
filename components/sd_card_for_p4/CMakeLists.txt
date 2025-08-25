idf_component_register(
    SRCS "sd_card_for_p4.c"
    INCLUDE_DIRS "."
    REQUIRES sdmmc fatfs driver
)
