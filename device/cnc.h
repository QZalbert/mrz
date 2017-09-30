extern double cnc_abs_cmd_pos(int channel,int axis);//命令坐标
extern int cnc_get_program_status(int channel); //获取运行的状态，0为IDLE，1为运行，2为暂停
extern int cnc_mode(int channel,int flag, int mode);    //获取系统的模式，0为手动，1为自动，2为MDI
extern int cnc_mdi(int channel,const char *text);   //发送MDI命令


extern int cnc_machine(int channel,int flag, int state);    //获取及设置机器开关
extern int cnc_io_input(int channel,int n);     //获取IO输入值
extern int cnc_io_output(int channel,int n);    //获取IO输出值

extern double cnc_abs_act_pos(int channel,int axis);
extern double cnc_rel_cmd_pos(int channel,int axis);
extern double cnc_rel_act_pos(int channel,int axis);
extern double cnc_abs_cmd_endpos(int channel,int axis);
extern double cnc_rel_cmd_endpos(int channel,int axis);


extern int cnc_current_vel(int channel);    //获取当前速度
extern int cnc_program_velocity(int channel);   //获取
extern int cnc_joint_homed(int channel,int joint);  //获取轴回零的结果
extern int cnc_joint_limit(int channel,int axis); //获取轴的限位情况，0表示在软限位内，1表示超出最小硬限位，2表示超出最小软限位，3表示超出最大软限位，4表示超出最大硬限位
extern int cnc_program_line(int channel);   //获取程序行号
extern int cnc_shell_input_bit(int channel,int num);    //获取num管脚的值
extern double cnc_set_jog_speed(int channel, int mode, double speed);   //获取及设置手动模式轴运动的默认速度
extern double cnc_set_jog_middlespeed(int channel, int mode, double speed);   //获取及设置手动模式轴运动的中等速度
extern double cnc_set_jog_highspeed(int channel, int mode, double speed);   //获取及设置手动模式轴运动的高速速度
extern int cnc_set_jog_speed_choice(int channel, int mode, int choice); //获取及设置手动模式轴运动的高低速选择
extern double cnc_set_jog_incr(int channel, int mode, double incr); //获取及设置手动模式轴增量运动的增量值
extern int cnc_set_jog_type(int channel, int mode, int type);  //获取及设置手动模式轴运动的模式，0为连续，1为增量，2为手轮
extern int cnc_feed_override(int channel,int flag, int percent);  //获取及设置进给倍率
extern int cnc_home(int channel,int axis);          //回零函数
extern int cnc_joint_homing(int channel,int axis);  //判断是否正在回零
extern int cnc_unhome(int channel,int axis);    //取消回零状态
extern int cnc_abort(int channel);          //退出运行状态
extern double cnc_get_parameters(int channel, int num); //获取#参数
extern int cnc_run(int channel, int line);  //运行加工文件
extern int cnc_pause(int channel);  //暂停
extern int cnc_resume(int channel); //恢复运行
extern int cnc_open(int channel, const char *filename); //打开加工文件
extern int cnc_task_plan_init(int channel); //初始化解析器

extern int cnc_error(int channel);  //获取错误信息，会退出当前运行状态
extern int cnc_operator_text(int channel);  //获取开发者信息，会退出当前运行状态
extern int cnc_operator_display(int channel);   //获取开发者信息，只显示
extern int cnc_jog(int channel, int axis, int direction);   //手动模式轴运动
extern int cnc_jog_stop(int channel, int axis);    //手动模式轴停止
extern float cnc_shell_input_float(int channel, int n); //float pin
extern int cnc_shell_input_u32(int channel, int n);//获取u32管脚的值
extern int cnc_shell_input_bit(int channel, int n);//获取bit管脚的值
extern float cnc_shell_input_float(int channel, int n);//获取float管脚的值
extern int cnc_spindle_override(int channel, int flag, int percent);    //主轴倍率
extern double cnc_set_spindle_speed(int channel, int flag, double speed);   //主轴指定转速
extern double cnc_spindle_speed(int channel);   //实际的主轴转速
extern double cnc_pos_offset(int channel, int axis);    //工件偏移值
extern int cnc_jog_enable(int channel, int flag, int onoff);    //手轮开关
extern float cnc_aux_pos_cmd(int channel, int num);	//获取命令值
extern float cnc_aux_pos_fb(int channel, int num);	//获取反馈值
extern int cnc_aux_homed(int channel, int num);	//获取回零状态
extern int cnc_aux_homing(int channel, int num);	//获取回零状态
extern float cnc_float_io(int channel, int flag, int num, float value);
extern int cnc_shell_input_u32(int channel, int n); //获取管脚的信号（u32），将自定义信号连到对应的管脚上就可以获取信号值.
extern int cnc_shell_input_s32(int channel, int n); //获取管脚的信号（s32），将自定义信号连到对应的管脚上就可以获取信号值.
extern float cnc_shell_input_float(int channel, int n); //获取管脚的信号（float），将自定义信号连到对应的管脚上就可以获取信号值.
extern int cnc_shell_input_bit(int channel, int n); //获取管脚的信号（bit），将自定义信号连到对应的管脚上就可以获取信号值.
extern int cnc_bit_out(int channel, int flag, int num, int state);//读写bit类型管脚

