#ifndef FP_H
#define FP_H

#include <QMainWindow>

namespace Ui {
class fp;
}

typedef struct {
    int x;
    int y;
    int w;
    int h;
}pos;

typedef struct {
    pos clk_reg_pos;
    unsigned int clb_per_clk_reg;
    unsigned int bram_per_clk_reg;
    unsigned int dsp_per_clk_reg;
    unsigned int bram_num;
    unsigned int dsp_num;
} fpga_clk_reg;

class fp : public QMainWindow
{
    Q_OBJECT

public:
    explicit fp(QWidget *parent = nullptr);
    ~fp();

    unsigned int num_clk_reg;
private:
    Ui::fp *ui;


};

#endif // FP_H
