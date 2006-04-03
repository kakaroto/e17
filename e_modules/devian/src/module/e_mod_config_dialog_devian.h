#ifdef E_MOD_DEVIAN_TYPEDEFS

#else

#ifndef E_MOD_CONFIG_DIALOG_DEVIAN_H_INCLUDED
#define E_MOD_CONFIG_DIALOG_DEVIAN_H_INCLUDED

E_Config_Dialog *DEVIANF(config_dialog_devian)(E_Container *con, DEVIANN *devian);
void DEVIANF(config_dialog_devian_shutdown)(DEVIANN *devian);
#ifdef HAVE_RSS
void DEVIANF(config_dialog_devian_rss_doc_update)(Rss_Doc *doc);
#endif

#endif
#endif
