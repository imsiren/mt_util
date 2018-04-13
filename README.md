# 工具扩展类
## Mt\Init mdp的入口，进行环境初始化
## Mt\Logger日志类
## Requirement 
- PHP 5.2 +
## Install 
### Install Mt Util

### Compile Mt Util in Linux
```
$/path/to/phpize
$./configure --with-php-config=/path/to/php-config
$make && make install
```
## 类库
###
Mt\Logger：日志操作类
#### 方法：
```
//以下方法目前只提供字符串方式。
//抛出warning到模块日志
Mt\Logger::notice($str);
//抛出error到模块日志，抛出后会中断，后面的代码将不执行
Mt\Logger::error($str);
//抛出debug到模块日志
Mt\Logger::debug($str);
```
### 
Mt\Init: 环境类
```
#初始化
Mt\Init::init();
#设置app名,一般只用于caller或切换环境变量时使用，切勿随便调用此方法
Mt\Init::setAppName($appName);
#获取当前app的名称
Mt\Init::getAppName();
#获取环境信息
Mt\Init::getEnv();
```
## Turorial
日志目录依赖于php.ini的mt.log_path
每个模块调用都会产生一个以自己模块为名字的文件夹
如 log_path = /web/logs/
在blog模块调用时日志将写在
/web/log/blog/blog.err.log里
