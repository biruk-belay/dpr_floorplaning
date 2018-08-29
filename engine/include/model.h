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
typedef std::vector<int>    Vec;
typedef std::vector<pos>    Vecpos;

typedef struct {
    unsigned long num_slots;
    unsigned long forbidden_slots;
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

int init_vectors(param_to_solver *, param_from_solver *);
