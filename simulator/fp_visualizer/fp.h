#ifndef FP_H
#define FP_H

#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <QGraphicsScene>

//#include "gurobi_c++.h"
#include "../../engine/include/model.h"
#include "fpga.h"

namespace Ui {
class fp;
}

enum fpga_type {
    ZYNQ,
    VIRTEX
};

typedef std::vector<pos> position_vec;

class fp : public QDialog
{
    Q_OBJECT

public:
    explicit fp(QWidget *parent = nullptr);
    ~fp();

    QGraphicsScene scene;
    QGraphicsRectItem *fp_rect;
    QBrush brush, brush_background;

    fpga *zynq;
    QString str;
    param_to_solver param;

    unsigned long int num_slots = 0;
    bool paint_rect = false;

    std::vector<int> clb_vector =  std::vector<int>(MAX_SLOTS);
    std::vector<int> bram_vector = std::vector<int>(MAX_SLOTS);
    std::vector<int> dsp_vector =  std::vector<int>(MAX_SLOTS);

    std::vector<int> eng_x =  std::vector<int>(MAX_SLOTS);
    std::vector<int> eng_y = std::vector<int>(MAX_SLOTS);
    std::vector<int> eng_w =  std::vector<int>(MAX_SLOTS);
    std::vector<int> eng_h =  std::vector<int>(MAX_SLOTS);

    std::vector<int> x_vector =  std::vector<int>(MAX_SLOTS);
    std::vector<int> y_vector = std::vector<int>(MAX_SLOTS);
    std::vector<int> w_vector =  std::vector<int>(MAX_SLOTS);
    std::vector<int> h_vector =  std::vector<int>(MAX_SLOTS);

    position_vec forbidden_region = position_vec(MAX_SLOTS);

    param_from_solver from_solver = {&eng_x, &eng_y,
                                   &eng_w, &eng_h};

    const int clb_width = 20;
    const int clb_height = 8, bram_height = 40, dsp_height = 20;
    const int x_start = 400, y_start = 10;
    int total_height;

    void init_fpga(enum fpga_type);
    void init_gui();
    void plot_rects(param_from_solver *);
    void paint_fp();

private:
    Ui::fp *ui;

private slots:
    void set_pressed();
    void enter_pressed();
    void start_pressed();
};

#endif // FP_H
