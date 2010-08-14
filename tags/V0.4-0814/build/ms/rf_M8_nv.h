/*----------------------------------------------------------------
        本文件包含对于特定机型的通用校准参数，在文件系统重建
        时可以自动写入到NV中。
        
        请根据机型的具体情况改写本文件中的列表。
        
        一般的修改的修改方法：
        1.取10台工作良好的手机，计算Calibration Nv的均值
        2.通过QPST的工具设置好需要的NV值，保存为QCN文件。
        3.使用QCN View转为txt文件。
        4.将其中的值复制到本表中。
----------------------------------------------------------------*/
#include "rf_w203_nv.h"