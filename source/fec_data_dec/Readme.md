# fec_data_dec
说明：如果这份文档里的链接你打不开，说明没有权限
> 程序的目的是将上一步 [usb_data_seperate](./../usb_data_seperate/) 中分离出来的数据进行解码，分解成root格式
> 同时也支持输出为二进制文件格式，方便使用 MATLAB 进行做图分析，关于 MATLAB 做图的说明，参考这个 [readme](../../matlab/mat_fec_plot/readme.md)

## root 数据格式说明
关于 root 数据格式的说明，请见 [./src/dec_single_file.cc](./src/dec_single_file.cc) 的 init_tree 函数，创建 tree 的代码如下
``` c++
// Decleaer the variables   
TFile* out_file;
TTree* data_tree;
long time_stamp = 0;
int trigger_id = 0;
int nhits = 0;
int chip_id[cMAX_HITS];
int chn_id[cMAX_HITS];
int adc_data[cMAX_HITS][cSCA_NUM];
// Create the tree
out_file = new TFile(data_file_name.c_str(), "recreate");
data_tree = new TTree("fec_origin_data", "FEC DATA");
data_tree->SetAutoSave();
data_tree->Branch("time_stamp", &time_stamp, "time_stamp/L");
data_tree->Branch("trigger_id", &trigger_id, "trigger_id/I");
data_tree->Branch("nhits", &nhits, "nhits/I");
data_tree->Branch("chip_id", chip_id, "chip_id[nhits]/I");
data_tree->Branch("chn_id", chn_id, "chn_id[nhits]/I");
data_tree->Branch("adc_data", adc_data, "adc_data[nhits][512]/I");
```
### 变量说明
+ time_stamp 是 FEC 板自己记录到的时间戳信息，目前这个时间戳没有任何的作用，只是一个 48 bits 的数据
+ trigger_id 是触发号，目前的系统中触发号统一由 DAQ 产生，DAQ 默认每个触发来了之后，触发号+1
+ nhits 是这个触发中，总共有多少个 hit
+ chip_id 是每个 hit 对应的芯片号，目前的系统中，芯片号是从 0 开始的，最多 4 个芯片
+ chn_id 是每个 hit 对应的通道号，目前的系统中，通道号是从 0 开始的，最多 68 个通道 (包含4个监控通道，这四个监控通道的通道号是 11 23 45 56，监控通道没有接入探测器，在数据处理的时候注意把它们去掉)
+ adc_data 是每个 hit 对应的波形数据，目前的系统中，每个 hit 对应 512 个 ADC 数据，每个 ADC 数据是 12 bits，所以每个 hit 对应 512 个 12 bits 的 ADC 数据

## 编译
请使用 linux 系统进行编译（或者 wsl )，编译前请确认已经安装好 root 环境，按如下命令进行编译。
注意：我们的分发版本是编译好的，目前编译的系统是Ubuntu 22.04，如果不是这个系统，请联系我
``` bash    
mkdir out
cd out
cmake ..
make
```

## 使用说明
### 命令行参数
``` bash
./fec_data_dec -f [待处理的文件名] 
```