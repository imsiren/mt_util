/*
 * =====================================================================================
 *
 *       Filename:  mt_logger.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016/02/23 10时42分28秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#define LOG_FILE_MAX_SIZE   300
#define MT_YAF_INIT_CLASS    "yaf_init"

extern zend_class_entry *mt_logger_ce;

int get_class_entry(char *class_name,int class_name_len,zend_class_entry **ce);

ZEND_MINIT_FUNCTION(mt_logger);

PHP_METHOD(mt_logger,notice);
PHP_METHOD(mt_logger,error);
PHP_METHOD(mt_logger,debug);

