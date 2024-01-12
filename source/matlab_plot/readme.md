# 处理数据后画波形

## 采集噪声和波形数据

+ 每次采数之前都要采集一次噪声的数据，这个噪声数据将用到最后的解包处理中
+ 采集完噪声之后配置成采数模式，进行采数即可

## 处理数据波形

+ 首先是 USB 解包程序 [usb_data_seperate](..\usb_data_separate) 对数据进行解包
+ 然后是使用 FEC 数据解包程序 [fec_data_dec](..\fec_data_dec) 将数据按照FEC板的格式进行拆分
  + 在调用的时候要加上 -T 参数，才能够输出二进制格式文件，用于 MATLAB 处理
+ 之后就调用 load_fec_binary_data.m 将解出来的二进制数据读到MATLAB的内存中
+ 再调用 load_noise_file.m 载入噪声，载入的时候选择result_link-*.txt这个文件

+ 数据载入完成之后，用 plot_hit_wave.m 进行画图

# 数据举例

得到的波形如下，说明信号的取件是在200-350个采样单元的位置，后续处理数据的时候可以用这个来进行处理

![image-20231019203212608](.\readme.assets\image-20231019203212608.png)