#ifndef ZYNQ_H
#define ZYNQ_H

#define MAX_SLOTS 100

//parameters for zynq
#define ZYNQ_FORBIDDEN 2
#define ZYNQ_NUM_ROWS 20
#define ZYNQ_WIDTH 29

#define ZYNQ_CLK_REG 4
#define ZYNQ_CLK00_BRAM 1
#define ZYNQ_CLK01_BRAM 1
#define ZYNQ_CLK10_BRAM 2
#define ZYNQ_CLK11_BRAM 2

#define ZYNQ_CLK00_DSP 1
#define ZYNQ_CLK01_DSP 1
#define ZYNQ_CLK10_DSP 1
#define ZYNQ_CLK11_DSP 1

//parameters for virtex
#define VIRTEX_FORBIDDEN 4
#define VIRTEX_NUM_ROWS 350
#define VIRTEX_WIDTH 98

#define VIRTEX_CLK_REG 14
//bram descritpion on virtex
#define VIRTEX_CLK00_BRAM 4
#define VIRTEX_CLK01_BRAM 4
#define VIRTEX_CLK02_BRAM 4
#define VIRTEX_CLK03_BRAM 4
#define VIRTEX_CLK04_BRAM 4
#define VIRTEX_CLK05_BRAM 2
#define VIRTEX_CLK06_BRAM 2
#define VIRTEX_CLK10_BRAM 3
#define VIRTEX_CLK11_BRAM 3
#define VIRTEX_CLK12_BRAM 3
#define VIRTEX_CLK13_BRAM 3
#define VIRTEX_CLK14_BRAM 3
#define VIRTEX_CLK15_BRAM 3
#define VIRTEX_CLK16_BRAM 3

#define VIRTEX_CLK00_DSP 4
#define VIRTEX_CLK01_DSP 4
#define VIRTEX_CLK02_DSP 4
#define VIRTEX_CLK03_DSP 4
#define VIRTEX_CLK04_DSP 4
#define VIRTEX_CLK05_DSP 2
#define VIRTEX_CLK06_DSP 2
#define VIRTEX_CLK10_DSP 3
#define VIRTEX_CLK11_DSP 3
#define VIRTEX_CLK12_DSP 3
#define VIRTEX_CLK13_DSP 3
#define VIRTEX_CLK14_DSP 3
#define VIRTEX_CLK15_DSP 3
#define VIRTEX_CLK16_DSP 3

typedef struct {
    int x;
    int y;
    int w;
    int h;
}pos;

typedef struct {
    pos clk_reg_pos;
    int clb_per_column;
    int bram_per_column;
    int dsp_per_column;
    int clb_num;
    int bram_num;
    int dsp_num;
    int forbidden_num;
    int *bram_pos;
    int *dsp_pos;
    int *forbidden_pos;
}fpga_clk_reg;


#define init_clk_reg(id, pos, clb, bram, dsp, num_bram, num_dsp, num_forbidden, \
                               pos_bram, pos_dsp, forbidden) \
                               clk_reg[id].clk_reg_pos = pos;    \
                               clk_reg[id].clb_per_column = clb;\
                               clk_reg[id].bram_per_column = bram; \
                               clk_reg[id].dsp_per_column = dsp;  \
                               clk_reg[id].bram_num = num_bram;  \
                               clk_reg[id].dsp_num = num_dsp;\
                               clk_reg[id].forbidden_num = num_forbidden;\
                               clk_reg[id].bram_pos = pos_bram;\
                               clk_reg[id].dsp_pos = pos_dsp; \
                               clk_reg[id].forbidden_pos = forbidden


class fpga
{
public:
    int num_clk_reg = ZYNQ_CLK_REG;
    fpga_clk_reg clk_reg[ZYNQ_CLK_REG];
    pos clk_reg_pos [ZYNQ_CLK_REG] = {{0,  50, 14, 50},
                                      {0,   0, 14, 50},
                                      {15, 50, 14, 50},
                                      {15,  0, 14, 50}};

    int clb_per_col  = 50;
    int bram_per_col = 10;
    int dsp_per_col  = 20;
    unsigned long num_rows = 20;
    unsigned long width = ZYNQ_WIDTH;

    int bram_in_reg[ZYNQ_CLK_REG] = {1, 1, 2, 2};
    int bram_pos[ZYNQ_CLK_REG][3] = {{4, 0, 0},  {4, 0, 0},
                                     {18, 25, 0}, {18, 25, 0}};

    int dsp_in_reg[ZYNQ_CLK_REG] = {1, 1, 1, 1};
    int dsp_pos[ZYNQ_CLK_REG][3] = {{7, 0, 0},
                                    {7, 0, 0},
                                    {22, 0, 0},
                                    {22, 0, 0}};

    unsigned long num_forbidden_slots = ZYNQ_FORBIDDEN;
    pos fbdn_pos[ZYNQ_FORBIDDEN + 1] =   {{10, 0, 1, ZYNQ_NUM_ROWS},
                                         {15, 0, 1, ZYNQ_NUM_ROWS}};

    int forbidden_regs[ZYNQ_CLK_REG] = {1, 1, 0, 0};
    int forbidden_pos[ZYNQ_CLK_REG][3] = {{10, 0 ,0},
                                          {10, 0, 0},
                                          {0, 0, 0,},
                                          {0, 0, 0}};

    void initialize_clk_reg();
    fpga();
};


class virtex
{
public:
    int num_clk_reg = ZYNQ_CLK_REG;
    fpga_clk_reg clk_reg[ZYNQ_CLK_REG];
    pos clk_reg_pos [ZYNQ_CLK_REG] = {{0,  50, 14, 50},
                                      {0,   0, 14, 50},
                                      {15, 50, 14, 50},
                                      {15,  0, 14, 50}};

    int clb_per_col  = 50;
    int bram_per_col = 10;
    int dsp_per_col  = 20;
    unsigned long num_rows = ZYNQ_NUM_ROWS;
    unsigned long width = ZYNQ_WIDTH;

    int bram_in_reg[ZYNQ_CLK_REG] = {1, 1, 2, 2};
    int bram_pos[ZYNQ_CLK_REG][3] = {{4, 0, 0},  {4, 0, 0},
                                     {18, 25, 0}, {18, 25, 0}};

    int dsp_in_reg[ZYNQ_CLK_REG] = {1, 1, 1, 1};
    int dsp_pos[ZYNQ_CLK_REG][3] = {{7, 0, 0},
                                    {7, 0, 0},
                                    {22, 0, 0},
                                    {22, 0, 0}};

    unsigned long num_forbidden_slots = ZYNQ_FORBIDDEN;
    pos fbdn_pos[ZYNQ_FORBIDDEN + 1] =   {{10, 0, 1, ZYNQ_NUM_ROWS},
                                         {15, 0, 1, ZYNQ_NUM_ROWS}};

    int forbidden_regs[ZYNQ_CLK_REG] = {1, 1, 0, 0};
    int forbidden_pos[ZYNQ_CLK_REG][3] = {{10, 0 ,0},
                                          {10, 0, 0},
                                          {0, 0, 0,},
                                          {0, 0, 0}};

    void initialize_clk_reg();
    virtex();
};
#endif
