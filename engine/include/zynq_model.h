#include <vector>
#include "fpga.h"
/*
typedef struct {
    int x;
    int y;
    int w;
    int h;
}pos;
*/
typedef std::vector<unsigned long>    Vec;
typedef std::vector<pos>    Vecpos;

typedef struct {
    unsigned long num_slots;
    unsigned long forbidden_slots;
    unsigned long num_rows;
    unsigned long width;
    Vec *clb;
    Vec *bram;
    Vec *dsp;
    Vecpos *fbdn_slot;
}param_to_solver;

typedef  struct{
    std::vector<int> *x;
    std::vector<int> *y;
    std::vector<int> *w;
    std::vector<int> *h;
}param_from_solver;

int zynq_start_optimizer(param_to_solver *, param_from_solver *);
int virtex_start_optimizer(param_to_solver *, param_from_solver *);

