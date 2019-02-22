#ifndef BOUNDCONDS_H
#define BOUNDCONDS_H


double fxzero(int yind, int ny);
double fxone(int yind, int ny);

double fyzero(int xind, int nx);
double fyone(int xind, int nx);

void getycoord(int ind, int n, double *y);
void getxcoord(int ind, int n, double *x);

#endif
