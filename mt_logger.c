/*
 * =====================================================================================
 *
 *       Filename:  mt_logger.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016/02/23 10时41分42秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "Zend/zend_API.h"
#include "Zend/zend_interfaces.h" 
#include "main/php_streams.h"
#include "Zend/zend_builtin_functions.h"
#include "php.h"

#include "ext/standard/php_smart_str.h"
#include "php_mt.h"
//#include "ext/standard/php_http.h"

#include "mt_init.h"
#include "mt_logger.h"
zend_class_entry *mt_logger_ce;

ZEND_BEGIN_ARG_INFO_EX(mt_logger_info_arginfo,0,0,0)

    ZEND_ARG_INFO(0,msg)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(mt_logger_notice_arginfo,0,0,0)
    ZEND_ARG_INFO(0,format)
    ZEND_ARG_INFO(0,string)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(mt_logger_warning_arginfo,0,0,0)
    ZEND_ARG_INFO(0,msg)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(mt_logger_error_arginfo,0,0,0)
    ZEND_ARG_INFO(0,format)
ZEND_END_ARG_INFO()

int get_class_entry(char *class_name,int class_name_len,zend_class_entry **ce){
    zend_class_entry **pce;

    char    *lower_cname    =   emalloc(strlen(class_name) + 1);
    zend_str_tolower_copy(lower_cname,class_name,strlen(class_name));

    if(zend_hash_find(CG(class_table),lower_cname,strlen(lower_cname)+1,(void**)&pce) == SUCCESS){
        *ce  =   *pce;
        efree(lower_cname);
        return SUCCESS;
    }
    ce  =   0;
    efree(lower_cname);
    return FAILURE;
}
static zval *get_app_name(){

    //return config.app_name;
    zval    *func_name,*obj,*res,*result    =NULL;
    zend_class_entry    *ce =   0;
    char    *class_name =   "Mt\\Init";

    int ret =   get_class_entry(class_name,strlen(class_name) + 1,&ce);

    if(ret == FAILURE){
        php_error_docref(NULL TSRMLS_CC,E_ERROR,"Class %s not found","Yaf\\Init");
    }
    MAKE_STD_ZVAL(obj);

    MAKE_STD_ZVAL(func_name);

    object_init_ex(obj,ce);

    ZVAL_STRING(func_name,"getAppName",1);
    call_user_function_ex(&(ce)->function_table,&obj,func_name,&result,0,NULL,0,NULL TSRMLS_DC);

    zval_ptr_dtor(&func_name);
    zval_ptr_dtor(&obj);
    return result;
}


static int output_log(char *error_type,zval *str){
    zval *app_name  =   NULL;
    zval *retval_ptr = NULL;
    app_name = get_app_name(); 

    if(app_name == NULL){

        php_error_docref0(NULL ,E_ERROR,"Cant not parse appName ."); 
    }

    char log_file_name[LOG_FILE_MAX_SIZE]   =   {0};

    sprintf(log_file_name,"%s/%s/%s.err.log",MT_G(log_path),Z_STRVAL_P(app_name),Z_STRVAL_P(app_name));

    struct timeval tv;
    char date_time[200]    =   {0};
    struct tm *tm_now   =   NULL;
    time_t  now;
    zval func;

    zval output;
    INIT_ZVAL(output);
#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 4))
    if (php_start_ob_buffer(NULL, 0, 0 TSRMLS_CC) != SUCCESS) {
#else
        if (php_output_start_user(NULL, 0, PHP_OUTPUT_HANDLER_STDFLAGS TSRMLS_CC) == FAILURE) {
#endif
            php_error_docref0(NULL ,E_ERROR,"call obstart error.");
        }

        ZVAL_STRINGL(&func,"debug_print_backtrace", sizeof("debug_print_backtrace") - 1,0);

        if(call_user_function_ex(EG(function_table),NULL,&func,&retval_ptr,0,NULL,0,NULL TSRMLS_CC) != SUCCESS){

            php_error_docref0(NULL ,E_ERROR ,"call debug_print_backtrace error.");
        }
#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 4))
    php_ob_get_buffer(&output TSRMLS_CC);
    php_end_ob_buffer(0, 0 TSRMLS_CC);

#else
    if(php_output_get_contents(&output TSRMLS_CC) == FAILURE){
        php_output_end(TSRMLS_C);

        php_error_docref0(NULL ,E_ERROR ,"call debug_print_backtrace error.");
    }
    php_output_discard(TSRMLS_C);
#endif

    if(retval_ptr){
        zval_ptr_dtor(&retval_ptr);
    }

    time(&now);
    tm_now  =   localtime(&now);
    strftime(date_time,200,"%Y-%m-%d %H:%M:%S",tm_now);

    size_t newlen;
    smart_str msg = {0};
    smart_str_alloc(&msg, 5120, 0);


	char* head = Z_STRVAL(output);
	while(*head != '\0' ){
		if(*head == '\n'){
	        *head = ' ';
	    }
	    head++;
	}

    char sep[2] = "\t";
    smart_str_appendl(&msg,date_time,strlen(date_time));
    smart_str_appendl(&msg,sep,strlen(sep));
    smart_str_appendl(&msg,error_type,strlen(error_type));
    smart_str_appendl(&msg,sep,strlen(sep));
    smart_str_appendl(&msg,Z_STRVAL_P(app_name),Z_STRLEN_P(app_name));
    smart_str_appendl(&msg,sep,strlen(sep));
    if(Z_TYPE_P(str) == IS_STRING){

        smart_str_appendl(&msg,Z_STRVAL_P(str),Z_STRLEN_P(str));
    }else if(Z_TYPE_P(str) == IS_LONG ){

       smart_str_append_unsigned(&msg,Z_LVAL_P(str));
    }else if(Z_TYPE_P(str) == IS_BOOL ){
      
       smart_str_append_unsigned(&msg,Z_DVAL_P(str));
    }
    smart_str_appendl(&msg,Z_STRVAL(output),Z_STRLEN(output));
    smart_str_appendl(&msg,"]",1);
    smart_str_appendl(&msg,"\n",1);
    smart_str_0(&msg);

    php_stream *stream ;
    stream  =   php_stream_open_wrapper(log_file_name,"a+",IGNORE_URL_WIN | REPORT_ERRORS,NULL);
    if(stream == NULL){

        zval_ptr_dtor(&app_name);
        smart_str_free(&msg);

        return FAILURE;
    }
    php_stream_write(stream,msg.c,msg.len);
    php_stream_close(stream);

    zval_dtor(&output);
    zval_ptr_dtor(&app_name);      
    smart_str_free(&msg);

    return SUCCESS;
}

PHP_METHOD(mt_logger,notice){
    zval *str;
    char error_type[] =   "E_NOTICE";
    /* /
    int i,argc = ZEND_NUM_ARGS();

    zval ***args;

    args = (zval ***)safe_emalloc(argc,sizeof(zval**),0);
    if(args == 0 || zend_get_parameters_ex(argc,args) == FAILURE){
        efree(args);
        WRONG_PARAM_COUNT;
    }

    for(i = 0; i < argc ;i++){

        args[i];
    }
    efree(args);
    */

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_DC,"z",&str) == FAILURE){
        WRONG_PARAM_COUNT;
    }
    output_log(error_type,str);
}

PHP_METHOD(mt_logger,error){
    zval *str;
    int len = 0;
    char error_type[] =   "E_ERROR";
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_DC,"z",&str,&len) == FAILURE){
        WRONG_PARAM_COUNT;
    }
    output_log(error_type,str);
    zend_bailout();
}

PHP_METHOD(mt_logger,debug){
    zval *str;

    char error_type[] =   "E_DEBUG";
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_DC,"z",&str) == FAILURE){
        WRONG_PARAM_COUNT;
    }
    output_log(error_type,str);
}
PHP_METHOD(mt_logger,warning){

    zval *str;

    char error_type[] =   "E_WARNING";
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_DC,"z",&str) == FAILURE){
        WRONG_PARAM_COUNT;
    }
    output_log(error_type,str);
}
PHP_METHOD(mt_logger,info){

}

PHP_METHOD(mt_logger,getLogPath){
    RETURN_STRING(MT_G(log_path), 1);
}


static zend_function_entry  mt_logger_methods[] =   {

    ZEND_ME(mt_logger,getLogPath,NULL,ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)

    ZEND_ME(mt_logger,info,mt_logger_info_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    ZEND_ME(mt_logger,notice,mt_logger_notice_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    ZEND_ME(mt_logger,debug,mt_logger_notice_arginfo,  ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    ZEND_ME(mt_logger,error,mt_logger_error_arginfo,    ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    ZEND_ME(mt_logger,warning,mt_logger_warning_arginfo,    ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)

    {NULL,NULL,NULL}
};

ZEND_MINIT_FUNCTION(mt_logger){

    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce,"Mt\\Logger",mt_logger_methods);

    mt_logger_ce    =   zend_register_internal_class_ex(&ce,NULL,NULL TSRMLS_DC);
    mt_logger_ce->ce_flags  |=  ZEND_ACC_FINAL_CLASS;

    zend_declare_property_null(mt_logger_ce,"logPath",strlen("logPath"),ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);
    return SUCCESS;
}



