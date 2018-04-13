/* *
 *  @desc Meitu_init类，用于初始化环境变量
 *
 *  @author：吴帅（wushuai@meitu.com)
 *  @date 20160216
 *
 * */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include "php.h"

#include "php_mt.h"
#include "mt_init.h"
#include "mt_logger.h"
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>



zend_class_entry *mt_init_ce;

ZEND_BEGIN_ARG_INFO_EX(mt_init_setappname_arginfo,0,0,0)
    ZEND_ARG_INFO(0,app_name)
ZEND_END_ARG_INFO()




static char *get_host_name(){

    static char host_name[128];

    gethostname(host_name,sizeof(host_name));

    return host_name;
}
static char *get_addr_ip(char *host_name){

    struct hostent *hst;

    hst = gethostbyname(host_name);

    static char ip[32]= {0};

    if(hst){

        inet_ntop(hst->h_addrtype,hst->h_addr,ip,sizeof(ip));
    }
    return ip;
}
PHP_METHOD(mt_init,getEnv){

    zval * envs = return_value;

    array_init(envs);

    char *host_name = get_host_name();

    //zend_hash_add(envs->value.ht,"host_name",strlen("host_name") +1,(void*)&host_name ,sizeof(host_name), NULL);

    add_assoc_string(envs,"host_name",host_name,1);
    
    char *str_ip   = get_addr_ip(host_name);
    //zend_hash_add(envs->value.ht,"local_ip",strlen("local_ip") +1,(void*)&ip,sizeof(ip),NULL);
    add_assoc_string(envs,"local_ip",str_ip,1);

}
static char *parse_modulename_from_path(zval *val){

    char *path = Z_STRVAL_P(val);

    static char result[50] = {0};

    char *delim = "/";

    char *pos =   strrchr(path,'/');

    strcpy(result,pos + 1);

    return result;
}


PHP_METHOD(mt_init,getViewExt){

    zval *_view_ext ;
    _view_ext  = zend_read_static_property(mt_init_ce,"viewExt",strlen("viewExt")+1,0 TSRMLS_CC);

    RETURN_ZVAL(_view_ext,1,1);
}

PHP_METHOD(mt_init,getAppName){

    zval *app_name;

    app_name = zend_read_static_property(mt_init_ce,"appName",strlen("appName") +1,0 TSRMLS_CC);

    RETURN_ZVAL(app_name,1,0);
}
PHP_METHOD(mt_init,setAppName){

    zval *val;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"z",&val) == FAILURE){
        return;
    }
    zend_update_static_property(mt_init_ce,"appName",strlen("appName")+ 1,val TSRMLS_CC);
}
static void define_constant(const char *name,int len,const char *val){
    zval *res;
    int ret = 0 ;
    ret = zend_get_constant(name,len,res);
    if(ret){

        return;
    }
    zval *zv_func_name,*result;
    zval *zv_name,*zv_val;
    MAKE_STD_ZVAL(zv_func_name);
    MAKE_STD_ZVAL(zv_name);
    MAKE_STD_ZVAL(zv_val);

    MAKE_STD_ZVAL(result);

    
    ZVAL_STRING(zv_func_name,"define", 1);
    ZVAL_STRING(zv_name,name,1);
    ZVAL_STRING(zv_val,val,1);

    zval *pars[2];

    pars[0] =   zv_name;
    pars[1] =   zv_val;
    //php_output_handler_create_internal;
    call_user_function(EG(function_table),NULL,zv_func_name,result,2,pars);

    zval_ptr_dtor(&result);
    zval_ptr_dtor(&zv_func_name);
    zval_ptr_dtor(&zv_name);
    zval_ptr_dtor(&zv_val);
}

static char *init_app_name(){
    zval *result;
    MAKE_STD_ZVAL(result);
    zval *func_name;

    MAKE_STD_ZVAL(func_name);

    ZVAL_STRING(func_name,"getcwd",1);
    call_user_function(EG(function_table),NULL,func_name,result,0,NULL);
    char *res;
    //解析出app_name;
    if(result){
        res = parse_modulename_from_path(result);
    }

    zval *z_app_name;
    MAKE_STD_ZVAL(z_app_name);
    ZVAL_STRING(z_app_name,res,1);
    zend_update_static_property(mt_init_ce,"appName",strlen("appName") + 1,z_app_name TSRMLS_CC);
    zval_ptr_dtor(&z_app_name);

    zval_ptr_dtor(&result);
    zval_ptr_dtor(&func_name);

    return res;
}


static char *get_full_path(){

    static char app_path[200]   =   {0};

    char *mdp_path= MT_G(mdp_path);

    char *app_name = init_app_name();

    sprintf(app_path,"%s/app/%s",mdp_path,app_name);
    return app_path;
}

static zval *init_directory(){


    zval *z_path;
    MAKE_STD_ZVAL(z_path);

    char *path_name = get_full_path();

    ZVAL_STRING(z_path,path_name,1);
    zend_update_static_property(mt_init_ce,"directory",strlen("directory") + 1,z_path TSRMLS_DC);

    define_constant(MDP_ROOT_PATH_NAME,MDP_ROOT_PATH_NAME_LEN,MT_G(mdp_path));

    define_constant(APP_ROOT_PATH_NAME,APP_ROOT_PATH_NAME_LEN,path_name);
    return z_path;
}

static zval *init_view(){

    zval *view = 0;
    zval *app_config_vals = 0;

    MAKE_STD_ZVAL(view);

    MAKE_STD_ZVAL(app_config_vals);

    array_init(app_config_vals);
    array_init(view);
    
    zval *z_tmp;
    MAKE_STD_ZVAL(z_tmp);
    ZVAL_STRING(z_tmp,MT_G(view_ext),1);
    zend_update_static_property(mt_init_ce,"viewExt",strlen("viewExt") + 1, z_tmp TSRMLS_CC);
    zval_ptr_dtor(&z_tmp);

    add_assoc_string(view,"ext",MT_G(view_ext),1);

    add_assoc_zval(app_config_vals,"view",view);

    return app_config_vals;
}

static zval *init_application_config(){

    zval *app_config = 0;
    MAKE_STD_ZVAL(app_config);
    array_init(app_config);

    init_app_name();

    zval *directory = init_directory();

    zval *app_config_vals = init_view();

    add_assoc_zval(app_config_vals,"directory",directory);

    add_assoc_zval(app_config,"application",app_config_vals);
    //php_var_dump(&app_config,1);
    return app_config;
}
static zval *init_application(zval *app_config){

    zval *object;

    MAKE_STD_ZVAL(object);

    zval *params[1];
    params[0] = app_config;

    zval *method;
    MAKE_STD_ZVAL(method);
    ZVAL_STRING(method,"__construct",1);

    zval *result;
    MAKE_STD_ZVAL(result);

    zend_class_entry    *yaf_application_ce =   NULL; 
    char class_name[]   =   "Yaf\\Application";
    get_class_entry(class_name,strlen(class_name),&yaf_application_ce);
    if(yaf_application_ce   ==  NULL){
        php_error_docref0(NULL TSRMLS_DC,E_ERROR,"Class Yaf\\Application not found.");
    }

    object_init_ex(object,yaf_application_ce);

    //调用构造函数
    call_user_function(&(yaf_application_ce)->function_table,&object,method,result,1,params);
/*

*/
    zval_ptr_dtor(&app_config);
    zval_ptr_dtor(&result);
    zval_ptr_dtor(&method);

    return object;
}

PHP_METHOD(mt_init,init){

    zval *app_config = init_application_config();
    zval *object = init_application(app_config);

    RETURN_ZVAL(object,1,1);
}


PHP_METHOD(mt_init,__construct){

}

PHP_METHOD(mt_init,__destruct){
}

zend_function_entry mt_init_methods[] = {
   ZEND_ME(mt_init,__construct,NULL,ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)

   ZEND_ME(mt_init,__destruct,NULL,ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
   ZEND_ME(mt_init,getViewExt,NULL,ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)

   ZEND_ME(mt_init,getAppName,NULL,ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)

   ZEND_ME(mt_init,setAppName,mt_init_setappname_arginfo,ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
   ZEND_ME(mt_init,getEnv,NULL ,ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)


   ZEND_ME(mt_init,init,NULL,ZEND_ACC_PUBLIC | ZEND_ACC_STATIC) 
   {NULL,NULL,NULL}
};



ZEND_MINIT_FUNCTION(mt_init){
	zend_class_entry ce;
    INIT_CLASS_ENTRY(ce,"Mt\\Init",mt_init_methods);
	mt_init_ce = zend_register_internal_class_ex(&ce, NULL , NULL TSRMLS_CC);

    zend_declare_property_null(mt_init_ce,"appName",strlen("appName") + 1,ZEND_ACC_PUBLIC | ZEND_ACC_STATIC);

    zend_declare_property_null(mt_init_ce,"viewExt",strlen("viewExt") + 1,ZEND_ACC_PUBLIC | ZEND_ACC_STATIC);

    zend_declare_property_null(mt_init_ce,"directory",strlen("directory") + 1,ZEND_ACC_PUBLIC | ZEND_ACC_STATIC);

	mt_init_ce ->ce_flags |= ZEND_ACC_FINAL_CLASS;


    //IS_CONSISTENT;
	return SUCCESS;
}

