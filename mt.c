/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "Zend/zend_API.h"
#include "Zend/zend_hash.h"
#include "ext/standard/info.h"
#include "php_mt.h"

#include "mt_init.h"
#include "mt_logger.h"

/* If you declare any globals in php_mt.h uncomment this:
*/
ZEND_DECLARE_MODULE_GLOBALS(mt)

/* True global resources - no need for thread safety here */
static int le_mt;

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
*/
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("mt.mdp_path",      "", PHP_INI_ALL, OnUpdateString, mdp_path, zend_mt_globals, mt_globals)
    STD_PHP_INI_ENTRY("mt.log_path",      "", PHP_INI_ALL, OnUpdateString, log_path, zend_mt_globals, mt_globals)
    STD_PHP_INI_ENTRY("mt.view.ext",      "", PHP_INI_ALL, OnUpdateString, view_ext, zend_mt_globals, mt_globals)
    STD_PHP_INI_ENTRY("mt.debug",      0	, PHP_INI_ALL, OnUpdateLong, use_debug, zend_mt_globals, mt_globals)
//    STD_PHP_INI_ENTRY("mt.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_mt_globals, mt_globals)
PHP_INI_END()
/* }}} */

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_mt_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_mt_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "mt", arg);
	RETURN_STRINGL(strg, len, 0);
}

/* 
PHP_FUNCTION(Ds){

	//zval *zval_array;
	char *class_name,
		 *class_type;
	int class_name_len = 0,
		class_type_len = 0;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC ,"s|s",&class_name,&class_name_len,&class_type,&class_type_len) == FAILURE){

		//php_error_docref(NULL TSRMLS_CC ,E_WARNING,"error");
		RETURN_NULL();
	}
	char s_instance[]  = "_instance_ce";

	zend_eval_string("static $instance = 234234;",NULL,(char*)"" TSRMLS_CC);

	zval **static_instance ;

	//MAKE_STD_ZVAL(*static_instance);

	zend_class_entry *yaf_loader_ce = NULL;

	get_class_entry("Yaf\\Loader",strlen("Yaf\\Loader"),&yaf_loader_ce);

	if(yaf_loader_ce == NULL){

		php_error_docref(NULL TSRMLS_DC,E_ERROR ,"Class Yaf\\Loader not found.");
	}
	zval *obj;

	MAKE_STD_ZVAL(obj);
	object_init_ex(obj,yaf_loader_ce);

	char desc_class_name[255] ;
	sprintf(desc_class_name,"%s\\%s\\%sModel",class_type,class_name,class_name);

	zval *cls_name ;
	MAKE_STD_ZVAL(cls_name);

	ZVAL_STRING(cls_name,desc_class_name,1);

	zval *autoload_params[2];
	
	autoload_params[0] = cls_name;

	zval *autoload, *result;
	MAKE_STD_ZVAL(autoload);
	MAKE_STD_ZVAL(result);
	ZVAL_STRING(autoload,"autoload",1);

	call_user_function(&(yaf_loader_ce)->function_table, &obj,autoload,result,1,autoload_params);

	//加载类成功后 实例化类

	zend_class_entry *instance_ce ;

	get_class_entry(desc_class_name,strlen(desc_class_name),&instance_ce);
	if(instance_ce == NULL){

		php_error_docref(NULL TSRMLS_DC,E_ERROR ,"Class %s not found.",desc_class_name);
	}
	zval *instance_obj;
	MAKE_STD_ZVAL(instance_obj);
	object_init_ex(instance_obj,instance_ce);

	zval *construct ;
	MAKE_STD_ZVAL(construct);

	ZVAL_STRING(construct,"__construct",1);

	call_user_function(&(instance_ce)->function_table,&instance_obj,construct,result,0,NULL);

	zval_ptr_dtor(&obj);
	zval_ptr_dtor(&construct);
	zval_ptr_dtor(&autoload);
	zval_ptr_dtor(&cls_name);
	zval_ptr_dtor(&result);

	RETURN_ZVAL(instance_obj,1,1);
}
*/


/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/* {{{ php_mt_init_globals
 */
/* Uncomment this function if you have INI entries
*/
static void php_mt_init_globals(zend_mt_globals *mt_globals)
{
	//mt_globals->log_path	= NULL;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(mt)
{
	/* If you have INI entries, uncomment these lines 
	*/
	REGISTER_INI_ENTRIES();

	ZEND_MODULE_STARTUP_N(mt_logger) (INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(mt_init) (INIT_FUNC_ARGS_PASSTHRU);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(mt)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/

	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(mt)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(mt)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(mt)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "mt support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ mt_functions[]
 *
 * Every user visible function must have an entry in mt_functions[].
 */
const zend_function_entry mt_functions[] = {
	PHP_FE(confirm_mt_compiled,	NULL)		/* For testing, remove later. */
	//PHP_FE(Ds,NULL)
/*	PHP_FE_END	 Must be the last line in mt_functions[] */
	{NULL, NULL,NULL,0,0}
};
/* }}} */

/* {{{ mt_module_entry
 */
zend_module_entry mt_module_entry = {
	STANDARD_MODULE_HEADER,
	"mt",
	mt_functions,
	PHP_MINIT(mt),
	PHP_MSHUTDOWN(mt),
	PHP_RINIT(mt),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(mt),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(mt),
	PHP_MT_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_MT
ZEND_GET_MODULE(mt)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
