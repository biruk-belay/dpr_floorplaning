#include "fp.h"
#include "ui_fp.h"
#include <QString>
#include <QDebug>


fp::fp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::fp)
{
    unsigned long i;
    ui->setupUi(this);
    zynq = new fpga();
    init_gui();

    //scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(&scene);

    fp::brush_background.setColor(Qt::black);
    paint_fp();

    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->setRowCount(10);

    connect(ui->set_button, SIGNAL(released()), this, SLOT(set_pressed()));
    connect(ui->enter_button, SIGNAL(released()), this, SLOT(enter_pressed()));
    connect(ui->start_fp, SIGNAL(released()), this, SLOT(start_pressed()));

    for(i = 0; i < zynq->num_forbidden_slots; i++)
        forbidden_region[i] = zynq->fbdn_pos[i];
}

fp::~fp()
{
    delete ui;
}

void fp::init_gui()
{
    ui->comboBox->addItem("Zynq");
    ui->comboBox->addItem("Virtex");
}

//this function first plots the FPGA visualizer
void fp::paint_fp()
{
    int i, j, k;
    int clk_reg_width, clk_reg_height;

    int pos_ptr_x, pos_ptr_y, bram_pos_ptr, dsp_pos_ptr, forbdn_ptr;
    int brams = 0, dsps = 0, forbdns;
    int bound, rec_height;
    bool is_bram = false, is_dsp = false, is_forbidden = false;

    QPen clk_reg_pen, rec_pen;

    Qt::GlobalColor colors [] = {Qt::GlobalColor::red, Qt::GlobalColor::magenta,
                                 Qt::GlobalColor::blue, Qt::GlobalColor::cyan,
                                 Qt::GlobalColor::yellow};

    total_height = zynq->num_rows * bram_height;

    for(i = 0; i < zynq->num_clk_reg ; i++) {
        clk_reg_width = zynq->clk_reg[i].clk_reg_pos.w * clb_width;
        clk_reg_height = zynq->clk_reg[i].clb_per_column * clb_height;

        bram_pos_ptr =  0; //zynq.clk_reg[i].bram_num;
        dsp_pos_ptr = 0;
        forbdn_ptr = 0;

        pos_ptr_x = zynq->clk_reg[i].clk_reg_pos.x;
        pos_ptr_y = zynq->clk_reg[i].clk_reg_pos.y;
        brams = zynq->clk_reg[i].bram_num;
        dsps = zynq->clk_reg[i].dsp_num;
        forbdns = zynq->clk_reg[i].forbidden_num;

        if(brams > 0)
            is_bram = true;

        if(dsps > 0)
            is_dsp = true;

        if(forbdns > 0)
            is_forbidden = true;

        QPen clk_reg_pen(colors[i]);
        clk_reg_pen.setWidth(2);

        fp_rect = scene.addRect(zynq->clk_reg[i].clk_reg_pos.x * clb_width,
                zynq->clk_reg[i].clk_reg_pos.y * clb_height,
                clk_reg_width, clk_reg_height, clk_reg_pen, brush);

        for(j = 0; j < zynq->clk_reg[i].clk_reg_pos.w; j++) {
            rec_height = clb_height;
            bound = zynq->clk_reg[i].clb_per_column;
            rec_pen.setColor(Qt::blue);
            rec_pen.setWidth(1);

            if(is_bram) {
                if(pos_ptr_x + j + 1 == zynq->clk_reg[i].bram_pos[bram_pos_ptr]) {
                    rec_pen.setColor(Qt::red);
                    bound = zynq->clk_reg[i].bram_per_column;
                    rec_height = bram_height;

                if(brams - 1 !=  0)
                    bram_pos_ptr += 1;
                else
                    is_bram = false;
                }
            }

            if(is_dsp) {
                if(pos_ptr_x + j + 1 == zynq->clk_reg[i].dsp_pos[dsp_pos_ptr]) {
                    rec_pen.setColor(Qt::green);
                    bound = zynq->clk_reg[i].dsp_per_column;
                    rec_height = dsp_height;

                if(dsps - 1 !=  0)
                    dsp_pos_ptr += 1;
                else
                    is_dsp = false;
                }
            }

            if(is_forbidden) {
                if(pos_ptr_x + j + 1 == zynq->clk_reg[i].forbidden_pos[forbdn_ptr]) {
                    rec_pen.setColor(Qt::black);
                    bound = 1; //zynq.clk_reg[i].dsp_per_column;
                    rec_height = clk_reg_height; // bram_height;

                if(forbdns - 1 !=  0)
                    forbdn_ptr += 1;
                else
                    is_forbidden = false;
                }
            }

            for(k = 0; k < bound; k++) {
                fp_rect = scene.addRect(pos_ptr_x * clb_width +  j * clb_width,
                                         pos_ptr_y * clb_height + k * rec_height,
                                        clb_width, rec_height, rec_pen, brush);
            }

            scene.setBackgroundBrush(fp::brush_background);
            //qDebug() << "bound is " << pos_ptr_x * clb_width +  j * clb_width << " brams " <<pos_ptr_y * clb_height + k * rec_height
              //       << "is bram "<< is_bram <<
              //        "bram pos " <<  clb_width << "rec height" << rec_height << endl;
        }
    }
}

void fp::set_pressed()
{
     unsigned long int i;
     enum fpga_type type;

     if(ui->comboBox->currentText() == "ZYNQ")
         type = ZYNQ;

     else if(ui->comboBox->currentText() == "VIRTEX")
         type = VIRTEX;

     str = ui->num_slotsLineEdit_2->text();
     num_slots = (unsigned long) str.toInt();
     ui->comboBox_2->clear();

     for(i = 0; i < num_slots; i++) {
        ui->comboBox_2->addItem(QString::number(i));
     }

     if(num_slots > 0) {
         clb_vector.clear();
         bram_vector.clear();
         dsp_vector.clear();

         eng_x.clear();
         eng_y.clear();
         eng_w.clear();
         eng_h.clear();

         x_vector.clear();
         y_vector.clear();
         w_vector.clear();
         h_vector.clear();
     }

     scene.clear();
     paint_fp();
}

void fp::enter_pressed()
{
    int clb = 0, bram = 0 , dsp = 0;
    QString st, temp;
    unsigned int num;

    st = ui->comboBox_2->currentText();
    num = (unsigned int) st.toInt();

    temp = ui->cLBLineEdit->text();
    clb = temp.toInt();
    clb_vector[num] = clb;

    temp = ui->bRAMLineEdit->text();
    bram = temp.toInt();
    bram_vector[num] = bram;

    temp = ui->dSPLineEdit->text();
    dsp =  temp.toInt();
    dsp_vector[num] = dsp;

}

void fp::start_pressed()
{
    unsigned long m;

    //send description of FPGA parameters and application parameters to optimizer
    param.num_slots = num_slots;
    param.forbidden_slots = zynq->num_forbidden_slots;
    param.num_rows = zynq->num_rows;
    param.width = zynq->width;
    param.bram = &bram_vector;
    param.clb  = &clb_vector;
    param.dsp  = &dsp_vector;
    param.fbdn_slot = &forbidden_region;

    //start optimizer
    init_vectors(&param, &from_solver);

    //calibrate the data returned from optimizer for visualization
    plot_rects(&from_solver);

    //Plot slots
    QPen slot_pen(Qt::red);
    slot_pen.setWidth(2);

    for(m = 0; m < num_slots; m++) {
        scene.addRect(x_vector[m], y_vector[m], w_vector[m], h_vector[m], slot_pen, brush);
        qDebug() << x_vector[m] << " " << y_vector[m] << " " << w_vector[m] << " " << h_vector[m] << endl;
    }
}

void fp::plot_rects(param_from_solver *fs)
{
    unsigned long int i;

    qDebug() <<"total height" << total_height <<endl;
    /*The information about the total height must be integral part of the FPGA description*/
    for(i = 0; i < num_slots; i++) {
        x_vector[i] = (*fs->x)[i] * clb_width;
        y_vector[i] = total_height - (((*fs->y)[i] + (*fs->h)[i]) * bram_height);
        h_vector[i] = (*fs->h)[i] * bram_height;
        w_vector[i] = (*fs->w)[i] * clb_width;
    }
}
