#include <iostream>
#include <vector>
#include "/opt/gurobi702/linux64/include/gurobi_c++.h"
#include "../../../floorplan/engine/include/zynq_model.h"

//Gurobi data types

typedef vector<GRBVar>          GRBVarArray;
typedef vector<GRBVarArray>     GRBVar2DArray;
typedef vector<GRBVar2DArray>   GRBVar3DArray;
typedef vector<GRBVar3DArray>   GRBVar4DArray;

using namespace std;

#define MAX_SLOTS 100

static unsigned long H, W;
static unsigned long num_slots;
static unsigned long num_rows;
static unsigned long num_forbidden_slots;
static unsigned long BIG_M = 10000000;
static unsigned long num_clk_regs = 7;

static int status;
static unsigned long delta_size;

static unsigned long clb_max = 10000;
static unsigned long bram_max = 10000;
static unsigned long dsp_max = 10000;

vector<int> clb_req (MAX_SLOTS);
vector<int> bram_req(MAX_SLOTS);
vector<int> dsp_req(MAX_SLOTS);

Vecpos fs(MAX_SLOTS);

int solve_milp_virtex(param_from_solver *to_sim)
{
    unsigned long status, i ,k, j, l;
    //define variables
    try {
        GRBEnv env = GRBEnv();
        GRBConstr* c = NULL;
        GRBModel model = GRBModel(env);

        if(num_slots >= num_forbidden_slots)
            delta_size = num_slots;
        else
            delta_size = num_forbidden_slots;

        //Variable definition

        /**********************************************************************
         name: x
         type: integer
         func: x[i][k] represent the left and right x coordinate of slot 'i'
        ***********************************************************************/

        GRBVar2DArray x(num_slots);
        for(i = 0; i < num_slots; i++) {
            GRBVarArray each_slot(2);
            x[i] = each_slot;

            for(k = 0; k < 2; k++)
                x[i][k] = model.addVar(0.0, W, 0.0, GRB_INTEGER);
        }

        /**********************************************************************
         name: y
         type: integer
         func: y[i] represents the bottom left y coordinate of slot 'i'
        ***********************************************************************/

        GRBVarArray y (num_slots);
        for(i = 0; i < num_slots; i++) {
            y[i] = model.addVar(0.0, H, 0.0, GRB_INTEGER);
        }

        /**********************************************************************
         name: w
         type: integer
         func: w[i] represents the width of the slot 'i'
        ***********************************************************************/

        GRBVarArray w (num_slots);
        for(i = 0; i < num_slots; i++) {
            w[i] = model.addVar(0.0, W, 0.0, GRB_INTEGER);
        }

        /**********************************************************************
         name: h
         type: integer
         func: h[i] represents the height of slot 'i'
        ***********************************************************************/

        GRBVarArray h (num_slots);
        for(i = 0; i < num_slots; i++) {
            h[i] = model.addVar(0.0, H, 0.0, GRB_INTEGER);
        }

        /**********************************************************************
         name: z
         type: binary
         func: z[i][k][][] is used to define the constraints on the distribution of
               resource on the FPGA fabric

               z[0][i][x_1/2][] == for clb
               z[1][i][x_1/2][] == for bram
               z[2][i][x_1/2][] == for dsp
        ***********************************************************************/

        GRBVar4DArray z(3);
        for(i = 0; i < 3; i++) {
            GRBVar3DArray each_slot(num_slots);
            z[i] = each_slot;

            for(k = 0; k < num_slots; k++)  {
                GRBVar2DArray x_coord(2);
                z[i][k] = x_coord;

                for(j = 0; j < 2; j++) {
                    GRBVarArray constrs(200);
                    z[i][k][j] = constrs;

                    for(l = 0; l < 200; l++)
                        z[i][k][j][l] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY);
                }
            }
        }

       /**********************************************************************
         name: clb
         type: integer
         func: clb[i][k] represents the number of clbs in (0, x_1) & (0, x_2)
               in a single row.
               'k' = 0 -> x_1
               'k' = 1 -> x_2

               the total numbe clb in slot 'i' is then calculated by
                clb in 'i' = clb[i][1] - clb[i][0]
        ***********************************************************************/

        GRBVar2DArray clb (num_slots);
        for(i = 0; i < num_slots; i++) {
            GRBVarArray each_slot(2);
            clb[i] = each_slot;

            for(k = 0; k < 2; k++)
                clb[i][k] = model.addVar(0.0, clb_max, 0.0, GRB_INTEGER);
        }

       /**********************************************************************
         name: bram
         type: integer
         func: bram[i][k] represents the number of brams in (0, x_1) & (0, x_2)
               in a single row.
               'k' = 0 -> x_1
               'k' = 1 -> x_2

               the total numbe brams in slot 'i' is then calculated by
                bram in 'i' = bram[i][1] - bram[i][0]
        ***********************************************************************/

        GRBVar2DArray bram (num_slots);
        for(i = 0; i < num_slots; i++) {
            GRBVarArray each_slot(2);
            bram[i] = each_slot;

            for(k = 0; k < 2; k++)
                bram[i][k] = model.addVar(1.0, GRB_INFINITY, 0.0, GRB_INTEGER);
        }
//#ifdef dspp
        /**********************************************************************
         name: dsp
         type: integer
         func: dsp[i][k] represents the number of dsp in (0, x_1) & (0, x_2)
               in a single row.
               'k' = 0 -> x_1
               'k' = 1 -> x_2

               the total numbe brams in slot 'i' is then calculated by
                dsp in 'i' = dsp[i][1] - dsp[i][0]
        ***********************************************************************/

        GRBVar2DArray dsp (num_slots);
        for(i = 0; i < num_slots; i++) {
            GRBVarArray each_slot(2);
            dsp[i] = each_slot;

            for(k = 0; k < 2; k++)
                dsp[i][k] = model.addVar(1.0, GRB_INFINITY, 0.0, GRB_INTEGER);
        }
//#endif
        /**********************************************************************
         name: beta
         type: binary
         func: beta[i][k] = 1 if clock region k is part of slot 'i'
        ***********************************************************************/

        GRBVar3DArray beta (num_slots);
        for(i = 0; i < num_slots; i++) {
                GRBVar2DArray for_each_clk_reg(num_clk_regs);
                beta[i] = for_each_clk_reg;

                for(j = 0; j < num_clk_regs; j++) {
                    GRBVarArray each_region(num_rows);
                    beta[i][j] = each_region;

                   for(k = 0; k < num_rows; k++)
                        beta[i][j][k] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY);
            }
        }

        /**********************************************************************
         name: tau
         type: integer
         func: tau[i][k] is used to linearize the function which is used to compute
               the number of available resources. The first index is used to
               denote the type of resource and the second is used to denote
               the slot
        ***********************************************************************/
        GRBVar4DArray tau (3); //for clb, bram, dsp
        for(i = 0; i < 3; i++) {
            GRBVar3DArray each_slot(num_slots);
            tau[i] = each_slot;

            for(l = 0; l < num_slots; l++) {
                GRBVar2DArray for_each_clk_reg(num_clk_regs);

                tau[i][l] = for_each_clk_reg;
                for(k = 0; k < num_clk_regs; k++) {
                    GRBVarArray each_slot_1(num_rows);

                    tau[i][l][k] = each_slot_1;
                    for(j = 0; j < num_rows; j++)
                        tau[i][l][k][j] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_INTEGER);
                }
            }
        }

       /**********************************************************************
         name: gamma
         type: binary
         func: gamma[i][k] = 1 iff bottom left x coordinate of slot 'i' is found
               to the left of the bottom left x coordinate of slot 'k'

               gamma[i][k] = 1 if x_i <= x_k
        ***********************************************************************/

        GRBVar2DArray gamma (num_slots);
        for(i = 0; i < num_slots; i++) {
            GRBVarArray each_slot(num_slots);
            gamma[i] = each_slot;

            for(k = 0; k < num_slots; k++)
                gamma[i][k] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY);
        }

        /**********************************************************************
         name: theta
         type: binary
         func: theta[i][k] = 1 iff the bottom left y coordinate of slot 'i' is
               found below the bottom left y coordinate of slot 'k'

               theta[i][k] = 1 if y_i <= y_k
        ***********************************************************************/

        GRBVar2DArray theta (num_slots);
        for(i = 0; i < num_slots; i++) {
            GRBVarArray each_slot(num_slots);
            theta[i] = each_slot;

            for(k = 0; k < num_slots; k++)
                theta[i][k] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY);
        }

        /**********************************************************************
         name: Gamma
         type: binary
         func: Gamma[i][k] = 1 iff x_i + w_i >= x_k
        ***********************************************************************/

        GRBVar2DArray Gamma (num_slots);
        for(i = 0; i < num_slots; i++) {
            GRBVarArray each_slot(num_slots);

            Gamma[i] = each_slot;
            for(k = 0; k < num_slots; k++)
                Gamma[i][k] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY);
        }

        /**********************************************************************
         name: Alpha
         type: binary
         func: Alpha[i][k] = 1 iff x_k + w_k >= x_i
        ***********************************************************************/

        GRBVar2DArray Alpha (num_slots);
        for(i = 0; i < num_slots; i++) {
            GRBVarArray each_slot(num_slots);
            Alpha[i] = each_slot;

            for(k = 0; k < num_slots; k++)
                Alpha[i][k] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY);
        }


        /**********************************************************************
         name: Omega
         type: binary
         func: Omega[i][k] = 1 iff if y_i + h_i >= y_k
        ***********************************************************************/

        GRBVar2DArray Omega(num_slots);
        for(i = 0; i < num_slots; i++) {
            GRBVarArray each_slot(num_slots);
            Omega[i] = each_slot;

           for(k = 0; k < num_slots; k++)
                Omega[i][k] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY);
        }

        /**********************************************************************
         name: Psi
         type: binary
         func: Psi[i][k] = 1 iff  y_k + h_k >= y_i
        ***********************************************************************/

        GRBVar2DArray Psi(num_slots);
        for(i = 0; i < num_slots; i++) {
            GRBVarArray each_slot(num_slots);
            Psi[i] = each_slot;

            for(k = 0; k < num_slots; k++)
                Psi[i][k] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY);
        }

        /**********************************************************************
         name: delta
         type: binary
         func: delta[i][k] = 1 if slot 'i' and 'k' share at least one tile
        ***********************************************************************/

        GRBVar3DArray delta (2);
        for(j = 0; j < 2; j++) {
            GRBVar2DArray each_slot(delta_size);

            delta[j] = each_slot;
            for(i = 0; i < delta_size; i++) {
                GRBVarArray each_slot_1(delta_size);

                delta[j][i] = each_slot_1;
                for(k = 0; k < delta_size; k++)
                    delta[j][i][k] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY);
            }
        }
//#ifdef fbdn

        /**********************************************************************
         name: mu
         type: binary
         func: mu[i][k] = 1 iff bottom left x coordinate of slot 'i' is found
               to the left of the bottom left x coordinate of slot 'k'

               mu[i][k] = 1 if x_i <= x_k
        ***********************************************************************/

        GRBVar2DArray mu (num_forbidden_slots);
        for(i = 0; i < num_forbidden_slots; i++) {
            GRBVarArray each_slot(num_slots);
            mu[i] = each_slot;

            for(k = 0; k < num_slots; k++)
                mu[i][k] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY);
        }

        /**********************************************************************
         name: nu
         type: binary
         func: nu[i][k] = 1 iff bottom left x coordinate of slot 'i' is found
               to the left of the bottom left x coordinate of slot 'k'

               nu[i][k] = 1 if x_i <= x_k
        ***********************************************************************/
        GRBVar2DArray nu (num_forbidden_slots);
        for(i = 0; i < num_forbidden_slots; i++) {
            GRBVarArray each_slot(num_slots);
            nu[i] = each_slot;

            for(k = 0; k < num_slots; k++)
                nu[i][k] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY);
        }

        /**********************************************************************
         name: fbdn_1
         type: binary
         func: fbdn_1[i][k] = 1 iff forbidden slot 'i' x variable interferes
               with slot 'k'
        ***********************************************************************/

        GRBVar2DArray fbdn_1 (num_forbidden_slots);
        for(i = 0; i < num_forbidden_slots; i++) {
            GRBVarArray each_slot(num_slots);

            fbdn_1[i] = each_slot;
            for(k = 0; k < num_slots; k++)
                fbdn_1[i][k] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY);
        }

        /**********************************************************************
         name: fbdn_2
         type: binary
         func: fbdn_2[i][k] = 1 iff forbidden slot 'i' x variable interferes
               with slot 'k'
        ***********************************************************************/

        GRBVar2DArray fbdn_2 (num_forbidden_slots) ;
        for(i = 0; i < num_forbidden_slots; i++) {
            GRBVarArray each_slot(num_slots);

            fbdn_2[i] = each_slot;
            for(k = 0; k < num_slots; k++)
                fbdn_2[i][k] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY);
        }

        /**********************************************************************
         name: fbdn_3
         type: binary
         func: fbdn_3[i][k] = 1 iff forbidden slot 'i' x variable interferes
               with slot 'k'
        ***********************************************************************/

        GRBVar2DArray fbdn_3 (num_forbidden_slots);
        for(i = 0; i < num_forbidden_slots; i++) {
            GRBVarArray each_slot(num_slots);

            fbdn_3[i] = each_slot;
            for(k = 0; k < num_slots; k++)
                fbdn_3[i][k] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY);
        }

        /**********************************************************************
         name: fbdn_4
         type: binary
         func: fbdn_4[i][k] = 1 iff forbidden slot 'i' x variable interferes
               with slot 'k'
        ***********************************************************************/
        GRBVar2DArray fbdn_4 (num_forbidden_slots);
        for(i = 0; i < num_forbidden_slots; i++) {
            GRBVarArray each_slot(num_slots);

            fbdn_4[i] = each_slot;
            for(k = 0; k < num_slots; k++)
                fbdn_4[i][k] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY);
        }
//#endif
     model.update();

        /********************************************************************
        Constr 1.1: The x coordinates must be constrained not to exceed
                      the boundaries of the fabric
        ********************************************************************/
        for(i = 0; i < num_slots; i++) {
            //for(k = 0; k< num_slots; k++) {
                model.addConstr(w[i] == x[i][1] - x[i][0], "1");
                //model.addConstr(x[i][0] <= W-1, "2");
                //model.addConstr(x[i][1] <= W, "3");
                model.addConstr(x[i][1] - x[i][0] >= 1, "4");
            //}
        }

        /********************************************************************
        Constr 1.2: The binary variables representing the rows must be
                    contigious i.e, if a region occupies clock region 1 and 3
                    then it must also occupy region 2
        ********************************************************************/
        /*for(i = 0; i < num_slots; i++) {
            GRBLinExpr exp;
            for(k = 0; k < (num_rows - 2); k++) {
                if(num_rows > 2)
                    model.addConstr(beta[i][k+1] >= beta[i][k] + beta[i][k+2] - 1, "5");
                exp += beta[i][k];
                //if(k >= 1)

            }
                //model.addConstr(exp <= 5, "98");
        }
*/
              for(i = 0; i < num_slots; i++) {
                  for(j = 0; j < num_clk_regs; j++) {
                      GRBLinExpr exp = 0;
                      for(k = 0; k < num_rows; k++) {
                          exp += beta[i][j][k];
                      }

                      for(l = 0; l < num_rows; l++) {
                          model.addConstr(beta[i][j][l] >= (exp - beta[i][j][l]) / (num_rows - 1), "500");
                      }
                  }
              }

        /************************************************************************
        Constr 1.3: The height of slot 'i' must be the sum of all clbs in the slot
        *************************************************************************/

        for(i = 0; i < num_slots; i++) {
            GRBLinExpr exp;
            for(j = 0; j < num_clk_regs; j++) {
                for(k = 0; k < num_rows; k++) {
                    exp += beta[i][j][k];
            }
            //model.addConstr(h[i] <= 8, "7");
            }
            model.addConstr(h[i] == exp, "6");
        }

        /******************************************************************
        Constr 1.4: y_i must be constrained not to be greater than the
                    lowest row
        ******************************************************************/
        for(i = 0; i < num_slots; i++) {
            GRBLinExpr exp_y;
             //for(j = 0; j < num_clk_regs; j++) {
                for(k = 0; k < num_rows; k++) {
                 //   model.addConstr(y[i] <= (H - beta[i][j][k] * (H - k)), "99");
               // }
                model.addConstr(y[i] + h[i] <= H, "100");
             }

        }

        //Resource Constraints
        /******************************************************************
        Constr 2.0: The clb on the FPGA is described using the following
                    piecewise function.
                    x       0  <= x < 5
                    x-1     5  <= x < 8
                    x-2     8  <= x < 13
                    x-3     13 <= x < 16
                    x-4     16 <= x < 21
                    x-5     21 <= x < 24
                    x-6     24 <= x < 33
                    x-7     33 <= x < 36
                    x-8     36 <= x < 54
                    x-9     54 <= x < 56
                    x-10    56 <= x < 60
                    x-11    60 <= x < 74
                    x-12    74 <= x < 84
                    x-13    84 <= x < 87
                    x-14    87 <= x < 93
                    x-15    93 <= x < 96
                    x-16    97 <= x < W

                    The piecewise function is then transformed into a set
                    of MILP constraints using the intermediate variable z
        ******************************************************************/
        for(i =0; i < num_slots; i++) {
            cout << "adding resource constraint" << endl;
            for(k = 0; k < 2; k++) {
                l = 0;
                model.addConstr(BIG_M * z[0][i][k][l++]  >= 5 - x[i][k], "1");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= x[i][k] - 4, "2");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= 8 - x[i][k], "3");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= x[i][k] - 7, "4");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= 13 - x[i][k], "5");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= x[i][k] - 12,  "6");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= 16 - x[i][k], "7");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= x[i][k] - 15, "8");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= 21 - x[i][k], "9");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= x[i][k] - 20, "10");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= 24 - x[i][k], "11");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= x[i][k] - 23, "12");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= 33 - x[i][k], "13");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= x[i][k] - 32, "14");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= 36 - x[i][k], "711");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= x[i][k] - 35, "811");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= 54 - x[i][k], "911");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= x[i][k] - 53, "1220");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= 56 - x[i][k], "1111");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= x[i][k] - 55, "1112");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= 60 - x[i][k], "1311");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= x[i][k] - 59, "1411");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= 74 - x[i][k], "997");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= x[i][k] - 73, "822");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= 84 - x[i][k], "9212");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= x[i][k] - 83, "1110");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= 87 - x[i][k], "1122");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= x[i][k] - 86, "1222");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= 93 - x[i][k], "1322");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= x[i][k] - 92, "1414");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= 96 - x[i][k], "9");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= x[i][k] - 95, "10");
                model.addConstr(BIG_M * z[0][i][k][l++]  >= W + 1 - x[i][k],  "15");


            }
        }


        //constr for clbs
        for(i = 0; i < num_slots; i++) {
            for(k = 0; k < 2; k++) {
                l = 0;
                model.addConstr(clb[i][k] >= x[i][k] - BIG_M * (1 - z[0][i][k][l++]), "8");

                model.addConstr(clb[i][k] >= (x[i][k] - 1)  - BIG_M * (1 - z[0][i][k][l++]) -
                                                           BIG_M * (1 - z[0][i][k][l++]), "9");

                model.addConstr(clb[i][k] >= (x[i][k] - 2)  - BIG_M * (1 - z[0][i][k][l++]) -
                                                           BIG_M * (1 - z[0][i][k][l++]), "10");

                model.addConstr(clb[i][k] >= (x[i][k] - 3)  - BIG_M * (1 - z[0][i][k][l++]) -
                                                           BIG_M * (1 - z[0][i][k][l++]), "11");

                model.addConstr(clb[i][k] >= (x[i][k] - 4)  - BIG_M * (1 - z[0][i][k][l++]) -
                                                           BIG_M * (1 - z[0][i][k][l++]), "12");

                model.addConstr(clb[i][k] >= (x[i][k] - 5)  - BIG_M * (1 - z[0][i][k][l++]) -
                                                           BIG_M * (1 - z[0][i][k][l++]), "13");

                model.addConstr(clb[i][k] >= (x[i][k] - 6)  - BIG_M * (1 - z[0][i][k][l++]) -
                                                           BIG_M * (1 - z[0][i][k][l++]), "14");

                model.addConstr(clb[i][k] >= (x[i][k] - 7)  - BIG_M * (1 - z[0][i][k][l++]) -
                                                           BIG_M * (1 - z[0][i][k][l++]), "15");

                model.addConstr(clb[i][k] >= (x[i][k] - 8)  - BIG_M * (1 - z[0][i][k][l++]) -
                                                           BIG_M * (1 - z[0][i][k][l++]), "229");

                model.addConstr(clb[i][k] >= (x[i][k] - 9)  - BIG_M * (1 - z[0][i][k][l++]) -
                                                           BIG_M * (1 - z[0][i][k][l++]), "2210");

                model.addConstr(clb[i][k] >= (x[i][k] - 10)  - BIG_M * (1 - z[0][i][k][l++]) -
                                                           BIG_M * (1 - z[0][i][k][l++]), "2211");

                model.addConstr(clb[i][k] >= (x[i][k] - 11)  - BIG_M * (1 - z[0][i][k][l++]) -
                                                           BIG_M * (1 - z[0][i][k][l++]), "2212");

                model.addConstr(clb[i][k] >= (x[i][k] - 12)  - BIG_M * (1 - z[0][i][k][l++]) -
                                                           BIG_M * (1 - z[0][i][k][l++]), "2213");

                model.addConstr(clb[i][k] >= (x[i][k] - 13)  - BIG_M * (1 - z[0][i][k][l++]) -
                                                           BIG_M * (1 - z[0][i][k][l++]), "2214");


                model.addConstr(clb[i][k] >= (x[i][k] - 14)  - BIG_M * (1 - z[0][i][k][l++]) -
                                                           BIG_M * (1 - z[0][i][k][l++]), "2211");

                model.addConstr(clb[i][k] >= (x[i][k] - 15)  - BIG_M * (1 - z[0][i][k][l++]) -
                                                           BIG_M * (1 - z[0][i][k][l++]), "2212");

                model.addConstr(clb[i][k] >= (x[i][k] - 16)  - BIG_M * (1 - z[0][i][k][l++]) -
                                                           BIG_M * (1 - z[0][i][k][l++]), "2213");

            }

            for( k = 0; k < 2; k++) {
                l = 0;
                model.addConstr(x[i][k] >= clb[i][k] - BIG_M * (1 - z[0][i][k][l++]), "16");

                model.addConstr(x[i][k] - 1 >= (clb[i][k])  - BIG_M * (1 - z[0][i][k][l++]) -
                                                       BIG_M * (1 - z[0][i][k][l++]), "17");

                model.addConstr(x[i][k] - 2 >= (clb[i][k])  - BIG_M * (1 - z[0][i][k][l++]) -
                                                       BIG_M * (1 - z[0][i][k][l++]), "18");

                model.addConstr(x[i][k] - 3 >= (clb[i][k])  - BIG_M * (1 - z[0][i][k][l++]) -
                                                       BIG_M * (1 - z[0][i][k][l++]), "19");

                model.addConstr(x[i][k] - 4 >= (clb[i][k])  - BIG_M * (1 - z[0][i][k][l++]) -
                                                       BIG_M * (1 - z[0][i][k][l++]), "20");

                model.addConstr(x[i][k] - 5 >= (clb[i][k])  - BIG_M * (1 - z[0][i][k][l++]) -
                                                       BIG_M * (1 - z[0][i][k][l++]), "21");

                model.addConstr(x[i][k] - 6 >= (clb[i][k])  - BIG_M * (1 - z[0][i][k][l++]) -
                                                       BIG_M * (1 - z[0][i][k][l++]), "22");

                model.addConstr(x[i][k] - 7 >= (clb[i][k])  - BIG_M * (1 - z[0][i][k][l++]) -
                                                       BIG_M * (1 - z[0][i][k][l++]), "23");

                model.addConstr(x[i][k] - 8 >= (clb[i][k])  - BIG_M * (1 - z[0][i][k][l++]) -
                                                       BIG_M * (1 - z[0][i][k][l++]), "17");

                model.addConstr(x[i][k] - 9 >= (clb[i][k])  - BIG_M * (1 - z[0][i][k][l++]) -
                                                       BIG_M * (1 - z[0][i][k][l++]), "18");

                model.addConstr(x[i][k] - 10 >= (clb[i][k])  - BIG_M * (1 - z[0][i][k][l++]) -
                                                       BIG_M * (1 - z[0][i][k][l++]), "19");

                model.addConstr(x[i][k] - 11 >= (clb[i][k])  - BIG_M * (1 - z[0][i][k][l++]) -
                                                       BIG_M * (1 - z[0][i][k][l++]), "20");

                model.addConstr(x[i][k] - 12 >= (clb[i][k])  - BIG_M * (1 - z[0][i][k][l++]) -
                                                       BIG_M * (1 - z[0][i][k][l++]), "21");

                model.addConstr(x[i][k] - 13 >= (clb[i][k])  - BIG_M * (1 - z[0][i][k][l++]) -
                                                       BIG_M * (1 - z[0][i][k][l++]), "22");

                model.addConstr(x[i][k] - 14 >= (clb[i][k])  - BIG_M * (1 - z[0][i][k][l++]) -
                                                       BIG_M * (1 - z[0][i][k][l++]), "17");

                model.addConstr(x[i][k] - 15 >= (clb[i][k])  - BIG_M * (1 - z[0][i][k][l++]) -
                                                       BIG_M * (1 - z[0][i][k][l++]), "18");

                model.addConstr(x[i][k] - 16 >= (clb[i][k])  - BIG_M * (1 - z[0][i][k][l++]) -
                                                       BIG_M * (1 - z[0][i][k][l++]), "19");
            }
        }

        /******************************************************************
        Constr 2.1: The same thing as constr 1.0.0 is done for the bram
                      which has the following piecewise distribution on
                      the fpga fabric
                    0     0  <=  x  < 5
                    1     5  <=  x  < 16
                    2     16 <=  x  < 21
                    3     21 <=  x  < 36
                    4     36 <=  x  < 57
                    5     57 <=  x  < 74
                    6     74 <=  x  < 87
                    7     87 <=  x  < 93
                    8     93 <=  x < W
        ******************************************************************/
        for(i =0; i < num_slots; i++) {
            for(k = 0; k < 2; k++) {
                l = 0;
                model.addConstr(BIG_M * z[1][i][k][l++]  >= 5 - x[i][k], "32");
                model.addConstr(BIG_M * z[1][i][k][l++]  >= x[i][k] - 4, "33");
                model.addConstr(BIG_M * z[1][i][k][l++]  >= 16 - x[i][k], "34");
                model.addConstr(BIG_M * z[1][i][k][l++]  >= x[i][k] - 15, "35");
                model.addConstr(BIG_M * z[1][i][k][l++]  >= 36 - x[i][k], "36");
                model.addConstr(BIG_M * z[1][i][k][l++]  >= x[i][k] - 35, "37");

                model.addConstr(BIG_M * z[1][i][k][l++]  >= 57 - x[i][k], "32");
                model.addConstr(BIG_M * z[1][i][k][l++]  >= x[i][k] - 56, "33");
                model.addConstr(BIG_M * z[1][i][k][l++]  >= 74 - x[i][k], "34");
                model.addConstr(BIG_M * z[1][i][k][l++]  >= x[i][k] - 73, "35");
                model.addConstr(BIG_M * z[1][i][k][l++]  >= 87 - x[i][k], "36");
                model.addConstr(BIG_M * z[1][i][k][l++]  >= x[i][k] - 86, "37");
                model.addConstr(BIG_M * z[1][i][k][l++]  >= 93 - x[i][k], "36");
                model.addConstr(BIG_M * z[1][i][k][l++]  >= x[i][k] - 92, "37");
                model.addConstr(BIG_M * z[1][i][k][l++]  >= W + 1 - x[i][k], "38");
            }
        }

        for(i = 0; i < num_slots; i++) {
            for(k = 0; k < 2; k++) {
                l = 0;
                model.addConstr(bram[i][k] >= 0 - BIG_M * (1 - z[1][i][k][l++]), "39");

                model.addConstr(bram[i][k] >= 1  - BIG_M * (1 - z[1][i][k][l++]) -
                                                           BIG_M * (1 - z[1][i][k][l++]), "40");

                model.addConstr(bram[i][k] >= 2  - BIG_M * (1 - z[1][i][k][l++]) -
                                                           BIG_M * (1 - z[1][i][k][l++]), "41");

                model.addConstr(bram[i][k] >= 3  - BIG_M * (1 - z[1][i][k][l++]) -
                                                           BIG_M * (1 - z[1][i][k][l++]), "42");


                model.addConstr(bram[i][k] >= 4  - BIG_M * (1 - z[1][i][k][l++]) -
                                                           BIG_M * (1 - z[1][i][k][l++]), "40");

                model.addConstr(bram[i][k] >= 5  - BIG_M * (1 - z[1][i][k][l++]) -
                                                           BIG_M * (1 - z[1][i][k][l++]), "41");

                model.addConstr(bram[i][k] >= 6  - BIG_M * (1 - z[1][i][k][l++]) -
                                                           BIG_M * (1 - z[1][i][k][l++]), "42");


                model.addConstr(bram[i][k] >= 7  - BIG_M * (1 - z[1][i][k][l++]) -
                                                           BIG_M * (1 - z[1][i][k][l++]), "40");

                model.addConstr(bram[i][k] >= 8  - BIG_M * (1 - z[1][i][k][l++]) -
                                                           BIG_M * (1 - z[1][i][k][l++]), "41");

            }

            for( k = 0; k < 2; k++) {
                l = 0;
                model.addConstr(0 >= bram[i][k] - BIG_M * (1 - z[1][i][k][l++]), "43");

                model.addConstr(1 >= (bram[i][k])  - BIG_M * (1 - z[1][i][k][l++]) -
                                                       BIG_M * (1 - z[1][i][k][l++]), "44");

                model.addConstr(2 >= (bram[i][k])  - BIG_M * (1 - z[1][i][k][l++]) -
                                                       BIG_M * (1 - z[1][i][k][l++]), "45");

                model.addConstr(3 >= (bram[i][k])  - BIG_M * (1 - z[1][i][k][l++]) -
                                                       BIG_M * (1 - z[1][i][k][l++]), "46");

                model.addConstr(4 >= (bram[i][k])  - BIG_M * (1 - z[1][i][k][l++]) -
                                                       BIG_M * (1 - z[1][i][k][l++]), "44");

                model.addConstr(5 >= (bram[i][k])  - BIG_M * (1 - z[1][i][k][l++]) -
                                                       BIG_M * (1 - z[1][i][k][l++]), "45");

                model.addConstr(6 >= (bram[i][k])  - BIG_M * (1 - z[1][i][k][l++]) -
                                                       BIG_M * (1 - z[1][i][k][l++]), "46");

                model.addConstr(7 >= (bram[i][k])  - BIG_M * (1 - z[1][i][k][l++]) -
                                                       BIG_M * (1 - z[1][i][k][l++]), "44");

                model.addConstr(8 >= (bram[i][k])  - BIG_M * (1 - z[1][i][k][l++]) -
                                                       BIG_M * (1 - z[1][i][k][l++]), "45");
            }
        }

//#ifdef dspp
        /******************************************************************
        Constr 2.2: Same thing is done for the dsp on the FPGA
                    0     0  <=  x  < 8
                    1     8  <=  x  < 13
                    2     15 <=  x  < 24
                    3     24 <=  x  < 33
                    4     33 <=  x  < 60
                    5     60 <=  x  < 84
                    6     84 <=  x <  96
                    7     96 <=  x < W
        ******************************************************************/
        for(i =0; i < num_slots; i++) {
            for(k = 0; k < 2; k++) {
                l = 0;
                model.addConstr(BIG_M * z[2][i][k][l++]  >= 8 - x[i][k], "47");
                model.addConstr(BIG_M * z[2][i][k][l++]  >= x[i][k] - 7, "48");
                model.addConstr(BIG_M * z[2][i][k][l++]  >= 13 - x[i][k], "49");
                model.addConstr(BIG_M * z[2][i][k][l++]  >= x[i][k] - 12, "50");

                model.addConstr(BIG_M * z[2][i][k][l++]  >= 24 - x[i][k], "47");
                model.addConstr(BIG_M * z[2][i][k][l++]  >= x[i][k] - 23, "48");
                model.addConstr(BIG_M * z[2][i][k][l++]  >= 33 - x[i][k], "49");
                model.addConstr(BIG_M * z[2][i][k][l++]  >= x[i][k] - 32, "50");

                model.addConstr(BIG_M * z[2][i][k][l++]  >= 60 - x[i][k], "47");
                model.addConstr(BIG_M * z[2][i][k][l++]  >= x[i][k] - 59, "48");
                model.addConstr(BIG_M * z[2][i][k][l++]  >= 84 - x[i][k], "49");
                model.addConstr(BIG_M * z[2][i][k][l++]  >= x[i][k] - 83, "50");

                model.addConstr(BIG_M * z[2][i][k][l++]  >= 96 - x[i][k], "47");
                model.addConstr(BIG_M * z[2][i][k][l++]  >= x[i][k] - 95, "48");
                model.addConstr(BIG_M * z[2][i][k][l++]  >= W + 1 - x[i][k], "51");
           }
        }

        for(i = 0; i < num_slots; i++) {
            for(k = 0; k < 2; k++) {
                l = 0;
                model.addConstr(dsp[i][k] >= 0 - BIG_M * (1 - z[2][i][k][l++]), "52");

                model.addConstr(dsp[i][k] >= 1  - BIG_M * (1 - z[2][i][k][l++]) -
                                                  BIG_M * (1 - z[2][i][k][l++]), "53");

                model.addConstr(dsp[i][k] >= 2  - BIG_M * (1 - z[2][i][k][l++]) -
                                                  BIG_M * (1 - z[2][i][k][l++]), "54");

                model.addConstr(dsp[i][k] >= 3  - BIG_M * (1 - z[2][i][k][l++]) -
                                                  BIG_M * (1 - z[2][i][k][l++]), "53");

                model.addConstr(dsp[i][k] >= 4  - BIG_M * (1 - z[2][i][k][l++]) -
                                                  BIG_M * (1 - z[2][i][k][l++]), "54");

                model.addConstr(dsp[i][k] >= 5  - BIG_M * (1 - z[2][i][k][l++]) -
                                                  BIG_M * (1 - z[2][i][k][l++]), "53");

                model.addConstr(dsp[i][k] >= 6  - BIG_M * (1 - z[2][i][k][l++]) -
                                                  BIG_M * (1 - z[2][i][k][l++]), "54");

                model.addConstr(dsp[i][k] >= 7  - BIG_M * (1 - z[2][i][k][l++]) -
                                                  BIG_M * (1 - z[2][i][k][l++]), "53");
            }

            for( k = 0; k < 2; k++) {
                l = 0;
                model.addConstr(0 >= dsp[i][k] - BIG_M * (1 - z[2][i][k][l++]), "55");

                model.addConstr(1 >= (dsp[i][k])  - BIG_M * (1 - z[2][i][k][l++]) -
                                                    BIG_M * (1 - z[2][i][k][l++]), "56");

                model.addConstr(2 >= (dsp[i][k])  - BIG_M * (1 - z[2][i][k][l++]) -
                                                    BIG_M * (1 - z[2][i][k][l++]), "57");


                model.addConstr(3 >= (dsp[i][k])  - BIG_M * (1 - z[2][i][k][l++]) -
                                                    BIG_M * (1 - z[2][i][k][l++]), "56");

                model.addConstr(4 >= (dsp[i][k])  - BIG_M * (1 - z[2][i][k][l++]) -
                                                    BIG_M * (1 - z[2][i][k][l++]), "57");


                model.addConstr(5 >= (dsp[i][k])  - BIG_M * (1 - z[2][i][k][l++]) -
                                                    BIG_M * (1 - z[2][i][k][l++]), "56");

                model.addConstr(6 >= (dsp[i][k])  - BIG_M * (1 - z[2][i][k][l++]) -
                                                    BIG_M * (1 - z[2][i][k][l++]), "57");

                model.addConstr(7 >= (dsp[i][k])  - BIG_M * (1 - z[2][i][k][l++]) -
                                                    BIG_M * (1 - z[2][i][k][l++]), "56");
            }
        }
//#endif
/*
        for(i = 0; i < num_slots; i++) {
            model.addConstr(BIG_M * res_constr[0][i] >= (bram_req[i] - 9), "333");
            model.addConstr(BIG_M * res_constr[1][i] >= (dsp_req[i]), "334");
            model.addConstr(BIG_M * kappa[1][i] >= res_constr[0][i] + res_constr[1][i] - 1, "335");
            model.addConstr(kappa[1][i] >= 0);
        }*/

      //constr for res
      /*********************************************************************
        Constr 2.3: There must be enough clb, bram and dsp inside the slot
      **********************************************************************/
        for(i = 0; i < num_slots; i++) {
            GRBLinExpr exp_tau, exp_res, exp_bram, exp_dsp;
            for(j = 0; j < num_clk_regs; j++) {
            for(k = 0; k < num_rows; k++) {
                model.addConstr(tau[0][i][j][k] <= 1000 * beta[i][j][k], "58");
                model.addConstr(tau[0][i][j][k] <= clb[i][1] - clb[i][0], "59");
                model.addConstr(tau[0][i][j][k] >= (clb[i][1] - clb[i][0]) - (1 - beta[i][j][k]) * clb_max, "60");
                model.addConstr(tau[0][i][j][k] >= 0, "15");

                model.addConstr(tau[1][i][j][k] <= 1000 * beta[i][j][k], "61");
                model.addConstr(tau[1][i][j][k] <= bram[i][1] - bram[i][0], "62");
                model.addConstr(tau[1][i][j][k] >= (bram[i][1] - bram[i][0]) - (1 - beta[i][j][k]) * bram_max, "63");
                model.addConstr(tau[1][i][j][k] >= 0, "53");
//#ifdef dspp
                model.addConstr(tau[2][i][j][k] <= 1000 * beta[i][j][k], "64");
                model.addConstr(tau[2][i][j][k] <= dsp[i][1] - dsp[i][0], "65");
                model.addConstr(tau[2][i][j][k] >= (dsp[i][1] - dsp[i][0]) - (1 - beta[i][j][k]) * dsp_max, "66");
                model.addConstr(tau[2][i][j][k] >= 0, "67");

                exp_dsp  += tau[2][i][j][k];
//#endif
                exp_res += tau[0][i][j][k];
                exp_bram += tau[1][i][j][k];
            }
            }
            model.addConstr(5 * exp_res >= clb_req[i],"68");
            //model.addConstr((5 * exp_res) - clb_req[i] <= 100,"168");
            model.addConstr(exp_bram >= bram_req[i],"69");
//            model.addConstr(exp_bram - bram_req[i] <= (1 - constr 9,"169");
//#ifdef dspp
            model.addConstr(2 * exp_dsp >= dsp_req[i],"70");
            //model.addConstr((2 * exp_dsp) - dsp_req[i] <= (BIG_M * kappa[1][i] + 19 *(1 - kappa[1][i])), "170");
//#endif
        }

        //Interference constraints
        /***********************************************************************
        Constraint 3.0: The semantics of Gamma, Alpha, Omega & Psi must be fixed
        ***********************************************************************/
        for(i = 0; i < num_slots; i++) {
            for(k = 0; k < num_slots; k++) {
                if(i == k)
                    continue;
                model.addConstr(BIG_M * gamma[i][k] >= x[k][0] - x[i][0], "63");
                model.addConstr(BIG_M * theta[i][k] >= y[k] - y[i], "64");
                model.addConstr(BIG_M * Gamma[i][k] >= x[i][1] - x[k][0] + 1, "65");
                model.addConstr(BIG_M * Alpha[i][k] >= x[k][1] - x[i][0] + 1, "66");
                model.addConstr(BIG_M * Omega[i][k] >= y[i] + h[i] - y[k], "67");
                model.addConstr(BIG_M * Psi[i][k]   >= y[k] + h[k] - y[i], "68");
            }
        }

        /***********************************************************************
        Constraint 3.1 Non interference between slot 'i' and 'k'
        ************************************************************************/

        for(i = 0; i < num_slots; i++) {
            GRBLinExpr exp_delta;
            for(k = 0; k < num_slots; k++) {
                if(i == k)
                    continue;
                model.addConstr(delta[0][i][k] >= gamma[i][k] + theta[i][k] +
                                Gamma[i][k] + Omega[i][k] - 3, "69");
                model.addConstr(delta[0][i][k] >= (1- gamma[i][k]) + theta[i][k] +
                                Alpha[i][k] + Omega[i][k] - 3, "70");
                model.addConstr(delta[0][i][k] >= gamma[i][k] + (1 - theta[i][k]) +
                                Gamma[i][k] + Psi[i][k] - 3, "71");
                model.addConstr(delta[0][i][k] >= (1 - gamma[i][k]) + (1 - theta[i][k]) +
                                Alpha[i][k] + Psi[i][k] - 3, "72");
                model.addConstr(delta[0][i][k] == 0, "73");
            }
        }

//#ifdef fbdn
        //Non Interference between global resoureces and slots
        /*************************************************************************
        Constriant 4.0: Global Resources should not be included inside slots
        *************************************************************************/
        for(i = 0; i < num_forbidden_slots; i++) {
            for(k = 0; k < num_slots; k++) {
                model.addConstr(BIG_M * mu[i][k]     >= x[k][0] - fs[i].x, "74");
                model.addConstr(BIG_M * nu[i][k]     >= y[k]    - fs[i].y, "75");
                model.addConstr(BIG_M * fbdn_1[i][k] >= fs[i].x + fs[i].w - x[k][0] + 1, "76");
                model.addConstr(BIG_M * fbdn_2[i][k] >= x[k][1]    - fs[i].x + 1, "77");
                model.addConstr(BIG_M * fbdn_3[i][k] >= fs[i].y + fs[i].h - y[k] + 1, "78");
                model.addConstr(BIG_M * fbdn_4[i][k] >= y[k] + h[k] - fs[i].y + 1, "79");
            }
        }

        /*************************************************************************
        Constraint 4.1:
        **************************************************************************/
        for(i = 0; i < num_forbidden_slots; i++) {
            //GRBLinExpr exp_delta;

            for(k = 0; k < num_slots; k++) {
                model.addConstr(delta[1][i][k] >= mu[i][k] + nu[i][k] + fbdn_1[i][k] + fbdn_3[i][k] - 3, "80");

                model.addConstr(delta[1][i][k] >= (1- mu[i][k]) + nu[i][k] + fbdn_2[i][k] + fbdn_3[i][k] - 3, "81");

                model.addConstr(delta[1][i][k] >= mu[i][k] + (1 - nu[i][k]) + fbdn_1[i][k] + fbdn_4[i][k] - 3, "82");

                model.addConstr(delta[1][i][k] >= (1 - mu[i][k]) + (1 - nu[i][k]) + fbdn_2[i][k] + fbdn_4[i][k] - 3, "83");

                //exp_delta += delta[1][i][k];
                model.addConstr(delta[1][i][k] == 0, "84");
            }
        }
//#endif
        //Optimize
        /****************************************************************************
        Optimize
        *****************************************************************************/
        model.set(GRB_IntParam_Threads, 8);
        model.set(GRB_DoubleParam_TimeLimit, 120);
        model.optimize();

        status = model.get(GRB_IntAttr_Status);

        if(status == GRB_OPTIMAL) {
            cout << "---------------------------------------------------------------------\
-------------------------------------------------------------------- "<< endl;
            cout<< "slot \t" << "x_0 \t" << "x_1 \t" << "y \t" << "w \t" << "h \t" << "clb_0 \t"
                    << "clb_1 \t" << "clb \t" << "req\t" << "bram_0 \t" << "bram_1 \t" << "bram \t"
                    << "req\t" << "dsp_0 \t" << "dsp_1 \t" << "dsp\t" << "req" <<endl;
            cout << "----------------------------------------------------------------------\
---------------------------------------------------------------------" << endl;

            for(i = 0; i < num_slots; i++) {
                cout <<endl;

                cout << i << "\t" << x[i][0].get(GRB_DoubleAttr_X) <<"\t"
                    << x[i][1].get(GRB_DoubleAttr_X) << "\t" << y[i].get(GRB_DoubleAttr_X)
                    <<" \t" <<  w[i].get(GRB_DoubleAttr_X) << "\t" << h[i].get(GRB_DoubleAttr_X)

                    <<"\t" << clb[i][0].get(GRB_DoubleAttr_X) <<"\t" <<
                    clb[i][1].get(GRB_DoubleAttr_X) << "\t" << (clb[i][1].get(GRB_DoubleAttr_X) -
                     clb[i][0].get(GRB_DoubleAttr_X)) * h[i].get(GRB_DoubleAttr_X) * 5 << "\t" << clb_req[i]

                    <<"\t" << bram[i][0].get(GRB_DoubleAttr_X) <<"\t" <<
                    bram[i][1].get(GRB_DoubleAttr_X) << "\t" << (bram[i][1].get(GRB_DoubleAttr_X) -
                    bram[i][0].get(GRB_DoubleAttr_X)) * h[i].get(GRB_DoubleAttr_X) << "\t" << bram_req[i]

                    << "\t" << dsp[i][0].get(GRB_DoubleAttr_X) << "\t" <<
                    dsp[i][1].get(GRB_DoubleAttr_X) << "\t" << (dsp[i][1].get(GRB_DoubleAttr_X) -
                            dsp[i][0].get(GRB_DoubleAttr_X)) * h[i].get(GRB_DoubleAttr_X) * 2 << "\t" << dsp_req[i] <<endl;


                    cout <<endl;

                    (*to_sim->x)[i] = (int) x[i][0].get(GRB_DoubleAttr_X);
                    (*to_sim->y)[i] = (int) y[i].get(GRB_DoubleAttr_X);
                    (*to_sim->w)[i] =  (int) w[i].get(GRB_DoubleAttr_X);
                    (*to_sim->h)[i] = (int) h[i].get(GRB_DoubleAttr_X);

                    for(k=0; k < 2; k++) {
                        for(l = 0; l < 3; l++)
                         cout <<"z" << l << " " << z[0][i][k][l].get(GRB_DoubleAttr_X) << "\t";
                         cout <<endl;
                    }

                    /*
                    for(l =0; l < num_clk_regs; l++)
                    for(k =0; k < num_rows; k++)
                        cout << "b"<< k << " " <<beta[i][l][k].get(GRB_DoubleAttr_X) << " " ;
*/

                    //std::cout << "kappa" << kappa[1][i].get(GRB_DoubleAttr_X) << res_constr[0][i].get(GRB_DoubleAttr_X) << res_constr[1][i].get(GRB_DoubleAttr_X) << endl;

                    cout <<  endl;

            }
         }

    else {
       model.computeIIS();

        cout<< "the following constraints can not be satisfied" <<endl;
        c = model.getConstrs();

        for(i = 0; i < model.get(GRB_IntAttr_NumConstrs); i++)
            if(c[i].get(GRB_IntAttr_IISConstr) == 1)
                cout << c[i].get(GRB_StringAttr_ConstrName) << endl;

    }
}
    catch(GRBException e)
    {
        cout << "Error code =" << e.getErrorCode() << endl;
        cout<< e.getMessage() << endl;

        return 0;
    }
    catch (...)
    {
        cout <<"exception while solving milp" << endl;
        return 0;
    }

   return status;

}

int virtex_start_optimizer(param_to_solver *param, param_from_solver *to_sim)
{
    num_slots = param->num_slots;
    num_forbidden_slots = param->forbidden_slots;
    num_rows = param->num_rows;
    H = 70;
    W = param->width;
    //num_forbidden_slots = param->forbidden_slots;

    unsigned long i;

    for(i = 0; i < num_slots; i++) {
        clb_req[i]  = (*param->clb)[i];
        bram_req[i] = (*param->bram)[i];
        dsp_req[i]  = (*param->dsp)[i];
    }

    for(i = 0; i < num_forbidden_slots; i++) {
        fs[i] = (*param->fbdn_slot)[i];
        cout <<"forbidden " << num_forbidden_slots << " " << fs[i].x << " " << fs[i].y << " " << fs[i].h << " " << fs[i].w <<endl;
    }
    cout << "finished copying" << endl;

    status = solve_milp_virtex(to_sim);

return 0;
}
