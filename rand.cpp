#include <iostream>
#include <cstdlib>

using namespace std;

#define CLB     0
#define BRAM    1
#define DSP     2

typedef struct {
    int clb;
    int bram;
    int dsp;
}slot;
    
int CLB_TOT = 1000, CLB_MIN = 200;
int BRAM_TOT = 40, BRAM_MIN = 0;
int DSP_TOT = 80, DSP_MIN = 0;
float u_t = 0.9;

bool is_compatible(slot *ptr, int size, int limit, int min, int type);

int main()
{
    int num_slots = 3;
    int i;

    //slot s1, s2, s3;
    slot sl_array[num_slots];
    srand((unsigned)time(0));
     
    do{ 
        for(i = 0; i < num_slots; i++)
            sl_array[i].clb = rand()%CLB_TOT;
    }while(is_compatible(sl_array, num_slots, CLB_TOT, CLB_MIN, CLB));
    
    do{
        for(i = 0; i < num_slots; i++)
            sl_array[i].bram = rand()%BRAM_TOT;
    }while(is_compatible(sl_array, num_slots, BRAM_TOT, BRAM_MIN, BRAM));

    do{
        for(i = 0; i < num_slots; i++)
            sl_array[i].dsp = rand()%DSP_TOT;
    }while(is_compatible(sl_array, num_slots, DSP_TOT, DSP_MIN, DSP));
    
    for(i = 0; i < num_slots; i++)
        cout<< "clb is " << sl_array[i].clb <<" " << sl_array[i].bram <<" "<< sl_array[i].dsp << endl;
}

bool is_compatible(slot *ptr, int slot_num, int max, int min, int type)
{   
    int i, temp = 0;

    for(i = 0; i < slot_num; i++) {
        if(type == CLB) {
            if((ptr + i)->clb < min)
                return true;
            temp += (ptr + i)->clb;
        }

        else if (type == BRAM){
            if((ptr + i)->bram < min)
                return true;
            temp += (ptr + i)->bram;
        }
        else {
            if((ptr + i)->dsp < min)
                return true;
            temp += (ptr + i)->dsp;
        }
    }
    
    if(temp >= (int) max * u_t)
            return true;
    else
        return false;
}
