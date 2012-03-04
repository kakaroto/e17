LIST(APPEND ECRIRE_CC_SOURCES
   main.c
   file_utils.c
   cfg.c
   ui/file_related.c
   ui/search_dialog.c
   ui/font_dialog.c
   ui/alerts.c
   )

include_directories(
   ${EINA_INCLUDE_DIRS}
   ${ELEMENTARY_INCLUDE_DIRS}
   ${EDJE_INCLUDE_DIRS}
   ${ECORE_INCLUDE_DIRS}
   ${ECORE_X_INCLUDE_DIRS}
   ${EFREET_INCLUDE_DIRS}
   ${EVAS_INCLUDE_DIRS}
   ${EET_INCLUDE_DIRS}
   )

add_executable(ecrire ${ECRIRE_CC_SOURCES})
target_link_libraries(ecrire
   ${EINA_LIBRARIES}
   ${ELEMENTARY_LIBRARIES}
   ${EDJE_LIBRARIES}
   ${ECORE_LIBRARIES}
   ${ECORE_X_LIBRARIES}
   ${EFREET_LIBRARIES}
   ${EVAS_LIBRARIES}
   ${EET_LIBRARIES}
   )

install(TARGETS ecrire DESTINATION bin)
