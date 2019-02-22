#ifndef DECOMP2D_H
#define DECOMP2D_H

int decomp2d(int nx, int ny, int layers, int nprocs, int myid, int *sxp, int *syp, int *exp, int *eyp);
int choose_layers(int nprocs, int* layers);

#endif
