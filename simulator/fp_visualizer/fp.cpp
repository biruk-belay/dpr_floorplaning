#include "fp.h"
#include "ui_fp.h"

#include <QString>
#include <QDebug>
#include <QFileDialog>
#include <iostream>
#include <string>

using namespace std;

fp::fp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::fp)
{
    unsigned long i;
    ui->setupUi(this);
    zynq = new fpga();
    virt = new virtex();

    init_gui();

    //scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(&scene);

    //fp::brush_background.setColor(Qt::black);
    //paint_zynq();

    //paint_virtex();                                                                                                                                                                                                                                                                  paint_virtex();
    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->setRowCount(10);

    connect(ui->set_button, SIGNAL(released()), this, SLOT(set_pressed()));
    connect(ui->FPGA, SIGNAL(released()), this, SLOT(fpga_pressed()));
    connect(ui->enter_button, SIGNAL(released()),this, SLOT(enter_pressed()));
    connect(ui->start_fp, SIGNAL(released()), this, SLOT(start_pressed()));
    connect(ui->browse_button, SIGNAL(released()), this, SLOT(set_browse()));
    connect(ui->set_util, SIGNAL(released()), this, SLOT(set_util()));

    for(i = 0; i < virt->num_forbidden_slots; i++)
        forbidden_region_virtex[i] = virt->forbidden_pos[i];

    for(i = 0; i < zynq->num_forbidden_slots; i++) {
        forbidden_region_zynq[i] = zynq->fbdn_pos[i];
        cout<< " fbdn" << forbidden_region_zynq[i].x << endl;
    }
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

void fp::paint_virtex()
{

    int i, j, k;
    int clk_reg_width, clk_reg_height;
    int frame_height, frame_width;

    int pos_ptr_x, pos_ptr_y, bram_pos_ptr, dsp_pos_ptr, forbdn_ptr;
    int brams = 0, dsps = 0, forbdns;
    int bound, rec_height;
    bool is_bram = false, is_dsp = false, is_forbidden = false;

    QPen clk_reg_pen, rec_pen;
    QBrush br(Qt::black);

    fp::clb_height = 6;
    fp::bram_height = 30;
    fp::dsp_height = 15;

    Qt::GlobalColor colors [] = {Qt::GlobalColor::red, Qt::GlobalColor::magenta,
                                 Qt::GlobalColor::blue, Qt::GlobalColor::cyan,
                                 Qt::GlobalColor::yellow};

    total_height = virt->num_rows * bram_height * virt->num_clk_reg / 2;
    //cout << "virt " << total_height << " " << virt->num_clk_reg << endl;

    frame_height = ui->graphicsView->geometry().height();
    frame_width = ui->graphicsView->geometry().width();

    for(i = 0; i < virt->num_clk_reg ; i++) {
        clk_reg_width = virt->clk_reg[i].clk_reg_pos.w * clb_width;
        clk_reg_height = virt->clk_reg[i].clb_per_column * clb_height;

       // cout << "height" << clk_reg_height << " " << clk_reg_width << endl;
        bram_pos_ptr =  0; //zynq.clk_reg[i].bram_num;
        dsp_pos_ptr = 0;
        forbdn_ptr = 0;

        pos_ptr_x = virt->clk_reg[i].clk_reg_pos.x;
        pos_ptr_y = virt->clk_reg[i].clk_reg_pos.y;
        brams = virt->clk_reg[i].bram_num;

        //cout << "brams " << brams <<endl;
        dsps = virt->clk_reg[i].dsp_num;
  //      forbdns = virt->clk_reg[i].forbidden_num;


        if(brams > 0)
            is_bram = true;

        if(dsps > 0)
            is_dsp = true;
/*
        if(forbdns > 0)
            is_forbidden = true;
*/
        clk_reg_pen.setColor(colors[2]);
        clk_reg_pen.setWidth(2);

        fp_rect = scene.addRect((virt->clk_reg[i].clk_reg_pos.x * clb_width) / virtex_scale,
                (virt->clk_reg[i].clk_reg_pos.y * clb_height) / virtex_scale,
                clk_reg_width / virtex_scale, clk_reg_height / virtex_scale, clk_reg_pen, brush);

        for(j = 0; j < virt->clk_reg[i].clk_reg_pos.w; j++) {
            rec_height = clb_height;
            bound = virt->clk_reg[i].clb_per_column;
            rec_pen.setColor(Qt::blue);
            rec_pen.setWidth(1);

            if(is_bram) {
                if(pos_ptr_x + j + 1 == virt->clk_reg[i].bram_pos[bram_pos_ptr]) {
                    rec_pen.setColor(Qt::red);
                    bound = virt->clk_reg[i].bram_per_column;
                    rec_height = bram_height;

                if(brams - 1 !=  0)
                    bram_pos_ptr += 1;
                else
                    is_bram = false;
                }
            }

            if(is_dsp) {
                if(pos_ptr_x + j + 1 == virt->clk_reg[i].dsp_pos[dsp_pos_ptr]) {
                    rec_pen.setColor(Qt::green);
                    bound = virt->clk_reg[i].dsp_per_column;
                    rec_height = dsp_height;

                if(dsps - 1 !=  0)
                    dsp_pos_ptr += 1;
                else
                    is_dsp = false;
                }
            }
/*
            if(is_forbidden) {
                if(pos_ptr_x + j + 1 == virt->clk_reg[i].forbidden_pos[forbdn_ptr]) {
                    rec_pen.setColor(Qt::black);
                    bound = 1; //zynq.clk_reg[i].dsp_per_column;
                    rec_height = clk_reg_height; // bram_height;

                if(forbdns - 1 !=  0)
                    forbdn_ptr += 1;
                else
                    is_forbidden = false;
                }
            }
*/
            for(k = 0; k < bound; k++) {
                fp_rect = scene.addRect((pos_ptr_x * clb_width +  j * clb_width) / virtex_scale,
                                         (pos_ptr_y * clb_height + k * rec_height) / virtex_scale,
                                        clb_width / virtex_scale, rec_height / virtex_scale, rec_pen, brush);
            }

            scene.setBackgroundBrush(fp::brush_background);
        }
    }

    for(i = 0; i < virt->num_forbidden_slots; i++) {
        rec_pen.setColor(Qt::black);
        fp_rect = scene.addRect((virt->forbidden_pos[i].x * clb_width) / 3,
                                (virt->forbidden_pos[i].y * clb_height)/ 3 ,
                                (virt->forbidden_pos[i].w * clb_width) / 3,
                                (virt->forbidden_pos[i].h * clb_height)/ 3, rec_pen, br);
    }
}

//this function first plots the FPGA visualizer
void fp::paint_zynq()
{
    int i, j, k;
    int clk_reg_width, clk_reg_height;

    int pos_ptr_x, pos_ptr_y, bram_pos_ptr, dsp_pos_ptr, forbdn_ptr;
    int brams = 0, dsps = 0, forbdns;
    int bound, rec_height;
    bool is_bram = false, is_dsp = false, is_forbidden = false;

    QPen clk_reg_pen, rec_pen;
    QBrush br(Qt::black);

    fp::clb_height = 8;
    fp::bram_height = 40;
    fp::dsp_height = 20;

    Qt::GlobalColor colors [] = {Qt::GlobalColor::red, Qt::GlobalColor::magenta,
                                 Qt::GlobalColor::blue, Qt::GlobalColor::cyan,
                                 Qt::GlobalColor::yellow};

    total_height = zynq->num_rows * bram_height * zynq->num_clk_reg / 2;

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

            for(k = 0; k < bound; k++) {
                fp_rect = scene.addRect(pos_ptr_x * clb_width +  j * clb_width,
                                         pos_ptr_y * clb_height + k * rec_height,
                                        clb_width, rec_height, rec_pen, brush);
            }
        }
    }


    for(i = 0; i < zynq->num_forbidden_slots; i++) {
        //rec_pen.setColor(Qt::black);
        fp_rect = scene.addRect((zynq->fbdn_pos[i].x * clb_width),
                                (zynq->fbdn_pos[i].y * clb_height) ,
                                (zynq->fbdn_pos[i].w * clb_width) ,
                                (zynq->fbdn_pos[i].h * clb_height), rec_pen, br);

    }
}

void fp::fpga_pressed()
{
    if(ui->comboBox->currentText() == "Zynq") {
        type = ZYNQ;
        fp::clb_height = 8;
        fp::bram_height = 40;
        fp::dsp_height = 20;
        scene.clear();
        paint_zynq();

        qDebug() << "selected zynq" << endl;
    }
    else if(ui->comboBox->currentText() == "Virtex") {
        type = VIRTEX;
        fp::clb_height = 6;
        fp::bram_height = 30;
        fp::dsp_height = 15;
        scene.clear();
        qDebug() << "selected zynq" << endl;
        paint_virtex();
    }
}

void fp::set_pressed()
{
     unsigned long int i;

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
     if(type == ZYNQ)
        paint_zynq();
     else
        paint_virtex();
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
    unsigned long m, scale;

    //send description of FPGA parameters and application parameters to optimizer
    param.bram = &bram_vector;
    param.clb  = &clb_vector;
    param.dsp  = &dsp_vector;
    param.num_slots = num_slots;

    scene.clear();
    if(type == ZYNQ)
       paint_zynq();
    else
       paint_virtex();

    if(type == VIRTEX) {
        param.forbidden_slots = virt->num_forbidden_slots;
        param.num_rows = virt->num_rows;
        param.width = virt->width;
        param.fbdn_slot = &forbidden_region_virtex;

        virtex_start_optimizer(&param, &from_solver);
        scale = 3;
    }

    if(type ==ZYNQ) {
        param.forbidden_slots = zynq->num_forbidden_slots;
        param.num_rows = zynq->num_rows;
        param.width = zynq->width;
        param.fbdn_slot = &forbidden_region_zynq;

        scale = 1;
        zynq_start_optimizer(&param, &from_solver);
    }

    //calibrate the data returned from optimizer for visualization
    plot_rects(&from_solver);

    //Plot slots
    QPen slot_pen(Qt::red);
    slot_pen.setWidth(4);

    for(m = 0; m < num_slots; m++) {
        scene.addRect(x_vector[m] / scale, y_vector[m] / scale, w_vector[m] / scale, h_vector[m] / scale, slot_pen, brush);
//        qDebug() << x_vector[m] << " " << y_vector[m] << " " << w_vector[m] << " " << h_vector[m] << endl;
    }
}

void fp::plot_rects(param_from_solver *fs)
{
    unsigned long int i;

    //qDebug() <<"total height" << total_height <<endl;

    /*The information about the total height must be integral part of the FPGA description*/
    for(i = 0; i < num_slots; i++) {
        x_vector[i] = (*fs->x)[i] * clb_width;
        y_vector[i] = total_height - (((*fs->y)[i] + (*fs->h)[i]) * bram_height);
        h_vector[i] = (*fs->h)[i] * bram_height;
        w_vector[i] = (*fs->w)[i] * clb_width;

  //      qDebug() << "total height " <<total_height <<endl;
    }
}

void fp::set_browse()
{
    int row, col;
    int i , k;
    unsigned int ptr;
    string str;

    file_path = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("CSV (*.csv)" ));
    CSVData csv_data(file_path.toStdString());

    row = csv_data.rows();
    col = csv_data.columns();

    qDebug() << " row " <<row << endl;

    if(row > num_slots) {
        for(i = 0, ptr = 0, k = 0; i < num_slots; i++, ptr++) {
            str = csv_data.get_value(i, k++);
            clb_vector[ptr] = std::stoi(str);

            str = csv_data.get_value(i, k++);
            bram_vector[ptr] = std::stoi(str);

            str = csv_data.get_value(i, k++);
            dsp_vector[ptr] = std::stoi(str);
            k = 0;
        }
    }
}

void fp::set_util()
{
    unsigned long i;
    int clb_tot, bram_tot, dsp_tot;
    int clb_min, bram_min, dsp_min;
    int clb_mod;
    QString str;
    int temp_util = 0;
    float util_temp = 0.0;

    //slot s1, s2, s3;
    //slot sl_array[num_slots];
     srand((unsigned)time(0));

     str = ui->utilLineEdit->text();
     utilization = str.toFloat();

     qDebug() << "util is" << utilization << endl;

     if(type == ZYNQ) {
         clb_tot  = ZYNQ_CLB_TOT;
         bram_tot = ZYNQ_BRAM_TOT;
         dsp_tot  = ZYNQ_DSP_TOT;
         clb_min  = ZYNQ_CLB_MIN;
         bram_min = ZYNQ_BRAM_MIN;
         dsp_min  = ZYNQ_DSP_MIN;
         clb_mod = clb_tot * utilization;
     }

     else if(type == VIRTEX) {
         clb_tot = VIRTEX_CLB_TOT;
         bram_tot = VIRTEX_BRAM_TOT;
         dsp_tot = VIRTEX_DSP_TOT;
         clb_min = VIRTEX_CLB_MIN;
         bram_min = VIRTEX_BRAM_MIN;
         dsp_min = VIRTEX_DSP_MIN;
         clb_mod = clb_tot * utilization;
     }

     int mod_clb = (clb_tot * 2 * utilization) / num_slots;
     int mod_bram = (bram_tot * 1 * utilization) / num_slots;
     int mod_dsp = (dsp_tot * 1 * utilization) / num_slots;

     do{
         for(i = 0; i < num_slots; i++) {
             sl_array[i].clb = rand()%mod_clb;
         }
     }while(is_compatible(sl_array, num_slots, clb_tot, clb_min, CLB));
/*
     do{
         for(i = 0; i < num_slots; i++)
             sl_array[i].bram = rand()%mod_bram;//bram_tot;
     }while(is_compatible(sl_array, num_slots, bram_tot, bram_min, BRAM));

     do{
         for(i = 0; i < num_slots; i++)
             sl_array[i].dsp = rand()%mod_dsp; //dsp_tot;
     }while(is_compatible(sl_array, num_slots, dsp_tot, dsp_min, DSP));
*/
     for(i = 0; i < num_slots; i++) {
         qDebug() << "in slot " << i << " " << sl_array[i].clb <<" " << sl_array[i].bram <<" "<< sl_array[i].dsp << endl;
         temp_util += sl_array[i].clb;
     }
    util_temp = (float) temp_util / clb_tot;
    qDebug() << "utilization " << util_temp << endl;
    if(num_slots != 0) {
        for(i = 0; i < num_slots; i++) {
            clb_vector[i] = sl_array[i].clb;
            bram_vector[i] = sl_array[i].bram;
            dsp_vector[i] = sl_array[i].dsp;
        }
    }
}

bool fp::is_compatible(std::vector<slot> ptr, unsigned long slot_num, int max, unsigned long min, int type)
{
    unsigned long i, temp = 0;
    unsigned long bram_max, dsp_max;

    if(fp::type == ZYNQ) {
        bram_max = ZYNQ_BRAM_TOT / 4;
        dsp_max = ZYNQ_DSP_TOT / 4;
    }
    else{
        bram_max = 50; //VIRTEX_BRAM_TOT / 20;
        dsp_max = 100; //VIRTEX_DSP_TOT / 20;
    }

    for(i = 0; i < slot_num; i++) {
        if(type == CLB) {
            if((ptr[i]).clb < min)
                return true;
            temp += (ptr[i]).clb;
        }
/*
        else if (type == BRAM){
            if((ptr[i]).bram < min || (ptr[i]).bram > bram_max)
                return true;
            temp += (ptr[i]).bram;
        }
        else {
            if((ptr[i]).dsp < min || (ptr[i]).dsp > dsp_max)
                return true;
            temp += (ptr[i]).dsp;
        }*/
    }

    if(temp >= (int) max * utilization) {
            return true;
    }
    else {
        qDebug() << "total " << type << " " << temp << endl;
        return false;
    }
}
