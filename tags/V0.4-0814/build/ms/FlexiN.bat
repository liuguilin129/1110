@echo off

rem window默认字符编码GBK,用于解决某些系统字符编码为UTF-8出现乱码
set OUTPUT_CHARSET=GBK

call flexi preobjs
call XGConsole /command="flexi genobjs -j20" /openmonitor /profile="XGConsole.xml"
rem 删除由s文件生成的o文件
call \bin\sh RmEmptyFile.sh Flexi
call flexi







