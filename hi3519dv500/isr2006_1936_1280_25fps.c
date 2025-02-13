td_s32 isr2006_linear_1936_1280_25fps_init_part(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += isr2006_write_register(vi_pipe, 0x007f, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x0082, 0x04); // 4d
    ret += isr2006_write_register(vi_pipe, 0x0083, 0xa0); // 160d
    ret += isr2006_write_register(vi_pipe, 0x0084, 0x05); // 5d

    ret += isr2006_write_register(vi_pipe, 0x0086, 0x03); // 3d 3为25fps 8翻倍为50fps

    ret += isr2006_write_register(vi_pipe, 0x0089, 0x05); // 5d
    ret += isr2006_write_register(vi_pipe, 0x008b, 0x05); // 5d
    // 0x01df,0x03 //diff
    ret += isr2006_write_register(vi_pipe, 0x01e0, 0x00); // 0d
    // 0x01e1,0x8c //diff
    ret += isr2006_write_register(vi_pipe, 0x01e2, 0x07); // 7d
    /*0x00ac 0x00ad 开窗总行数*/

    ret += isr2006_write_register(vi_pipe, 0x00ac, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x00ad, 0x05); // 5d


    ret += isr2006_write_register(vi_pipe, 0x01e5, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x01e6, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x01e7, 0x01); // 1d
    ret += isr2006_write_register(vi_pipe, 0x01e8, 0x01); // 1d
    ret += isr2006_write_register(vi_pipe, 0x00a5, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x00a7, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x00a4, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x00a9, 0x01); // 1d
    ret += isr2006_write_register(vi_pipe, 0x00aa, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x00ab, 0x2d); // 45d
    ret += isr2006_write_register(vi_pipe, 0x008f, 0x4f); // 79d
    ret += isr2006_write_register(vi_pipe, 0x0090, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x0091, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x0092, 0x20); // 32d
    ret += isr2006_write_register(vi_pipe, 0x0093, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x00e2, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x014f, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x0150, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x01d7, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x01d6, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x0058, 0x01); // 1d
    ret += isr2006_write_register(vi_pipe, 0x0059, 0x0f); // 15d
    ret += isr2006_write_register(vi_pipe, 0x005a, 0x01); // 1d
    ret += isr2006_write_register(vi_pipe, 0x005b, 0x03); // 3d
    ret += isr2006_write_register(vi_pipe, 0x1201, 0xf0); // 240d
    ret += isr2006_write_register(vi_pipe, 0x1202, 0x70); // 112d
    ret += isr2006_write_register(vi_pipe, 0x1203, 0x10); // 16d
    ret += isr2006_write_register(vi_pipe, 0x1204, 0x10); // 16d
    ret += isr2006_write_register(vi_pipe, 0x1070, 0x02); // 2d
    ret += isr2006_write_register(vi_pipe, 0x1205, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x1208, 0x01); // 1d
    ret += isr2006_write_register(vi_pipe, 0x1000, 0x10); // 16d
    ret += isr2006_write_register(vi_pipe, 0x1001, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x1070, 0x12); // 18d
    ret += isr2006_write_register(vi_pipe, 0x1070, 0x02); // 2d
    ret += isr2006_write_register(vi_pipe, 0x1024, 0x90); // 144d
    ret += isr2006_write_register(vi_pipe, 0x1025, 0x07); // 7d
    ret += isr2006_write_register(vi_pipe, 0x1026, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x1027, 0x05); // 5d
    ret += isr2006_write_register(vi_pipe, 0x1040, 0x8d); // 141d
    ret += isr2006_write_register(vi_pipe, 0x1020, 0x2a); // 42d
    ret += isr2006_write_register(vi_pipe, 0x1042, 0x0f); // 15d
    ret += isr2006_write_register(vi_pipe, 0x1028, 0x90); // 144d
    ret += isr2006_write_register(vi_pipe, 0x1029, 0x07); // 7d
    ret += isr2006_write_register(vi_pipe, 0x102a, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x102b, 0x05); // 5d
    ret += isr2006_write_register(vi_pipe, 0x102c, 0x90); // 144d
    ret += isr2006_write_register(vi_pipe, 0x102d, 0x07); // 7d
    ret += isr2006_write_register(vi_pipe, 0x102e, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x102f, 0x05); // 5d
    ret += isr2006_write_register(vi_pipe, 0x1030, 0x90); // 144d
    ret += isr2006_write_register(vi_pipe, 0x1031, 0x07); // 7d
    ret += isr2006_write_register(vi_pipe, 0x1032, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x1033, 0x05); // 5d
    ret += isr2006_write_register(vi_pipe, 0x01e3, 0x1e); // 30d
    ret += isr2006_write_register(vi_pipe, 0x00a4, 0x01); // 1d
    ret += isr2006_write_register(vi_pipe, 0x1040, 0x8d); // 140d
    ret += isr2006_write_register(vi_pipe, 0x005b, 0x03); // 3d
    ret += isr2006_write_register(vi_pipe, 0x005d, 0x0f); // 15d
    ret += isr2006_write_register(vi_pipe, 0x0042, 0xaa); // 170d
    ret += isr2006_write_register(vi_pipe, 0x009b, 0x00); // 0d // 9b 9c 9d 9e 写0为HCG 写1为LCG
    ret += isr2006_write_register(vi_pipe, 0x009c, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x009d, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x009e, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x00a5, 0x01); // 1d
    ret += isr2006_write_register(vi_pipe, 0x00a6, 0x01); // 1d
    ret += isr2006_write_register(vi_pipe, 0x00a6, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x0261, 0x0f); // 15d //调整列条纹
    ret += isr2006_write_register(vi_pipe, 0x0262, 0x81); // 129d //调整列条纹
    ret += isr2006_write_register(vi_pipe, 0x002b, 0x70); // 112d
    ret += isr2006_write_register(vi_pipe, 0x002c, 0x01); // 1d
    ret += isr2006_write_register(vi_pipe, 0x0030, 0x32); // 50d // 低帧率 30寄存器的值需要往大写
    ret += isr2006_write_register(vi_pipe, 0x01d7, 0x01); // 1d
    ret += isr2006_write_register(vi_pipe, 0x01d8, 0x03); // 3d
    ret += isr2006_write_register(vi_pipe, 0x0045, 0x20); // 136d //ADC的ADC_DRST值,对应时序1，12bit,tim_mode1
    ret += isr2006_write_register(vi_pipe, 0x0046, 0x7d); // 125d //ADC的ADC_DRST值,对应时序1，12bit,tim_mode1
    ret += isr2006_write_register(vi_pipe, 0x0150, 0x01); // 1d //DPC 模块使能
    ret += isr2006_write_register(vi_pipe, 0x0090, 0x05); // 5d
    ret += isr2006_write_register(vi_pipe, 0x008f, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x0027, 0x01); // 1d //矫正太阳黑子使能
    ret += isr2006_write_register(vi_pipe, 0x0025, 0x08); // 8d //调太阳黑子阈值 默认为08
    return ret;
}