#ifndef MT_INIT_H
#define MT_INIT_H


#define MDP_ROOT_PATH_NAME "MDP_ROOT_PATH"
#define MDP_ROOT_PATH_NAME_LEN  strlen(MDP_ROOT_PATH_NAME) 

#define APP_ROOT_PATH_NAME    "APP_ROOT_PATH"
#define APP_ROOT_PATH_NAME_LEN     strlen(APP_ROOT_PATH_NAME) 


extern zend_class_entry *mt_init_ce;

typedef struct application_config{

    zval *app_config;
    zval *app_config_val;
    zval *view_ext;
    zval *view;
    zval *directory;
    zval *app_name;

    char *app_path;
    char *mdp_path;
}application_config;

extern struct application_config config;

PHP_METHOD(mt_init,init);
PHP_METHOD(mt_init,getAppName);
PHP_METHOD(mt_init,setAppName);
PHP_METHOD(mt_init,getViewExt);

ZEND_MINIT_FUNCTION(mt_init);
#endif
