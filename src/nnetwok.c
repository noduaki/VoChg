#include "header.h"

int judgeWave(weights* w, double* diffData, double* tmp1, double* tmp2) {

    int i;
    int k;
    double sum;
    double softSum;
    double softMax1;
    double softMax2;

    // First layer
    sum = 0;
    for (k = 0; k < NEURONS_1; k++) {
        for (i = 0; i < NUM_DATA_X; i++) {
            sum += (*(diffData + i) / 6553.0) * *(w[0].weight + i + (NUM_DATA_X * k));
        }
        *(tmp1 + k) = sum + *(w[0].bias + k);
        sum = 0;
    }
    for (k = 0; k < NEURONS_1; k++) {
        if (*(tmp1 + k) < 0.0) *(tmp1 + k) = 0.0;
    }
    // Second layer
    sum = 0;
    for (k = 0; k < NEURONS_2; k++) {
        for (i = 0; i < NEURONS_1; i++) {
            sum += *(tmp1 + i) * *(w[1].weight + i + (NEURONS_1 * k));
        }
        *(tmp2 + k) = sum + *(w[1].bias + k);
        sum = 0;
    }
    for (k = 0; k < NEURONS_2; k++) {
        if (*(tmp1 + k) < 0.0) *(tmp2 + k) = 0.0;
    }

    // Output layer
    sum = 0;
    for (k = 0; k < NEURONS_3; k++) {
        for (i = 0; i < NEURONS_2; i++) {
            sum += *(tmp2 + i) * *(w[2].weight + i + (NEURONS_2 * k));
        }
        *(tmp1 + k) = sum + *(w[2].bias + k);
        sum = 0;
    }

    softSum = exp(*(tmp1)) + exp(*(tmp1 + 1));
    softMax1 = exp(*(tmp1)) / softSum;
    softMax2 = exp(*(tmp1 + 1)) / softSum;

    

    if(softMax2 < 0.5)return 0;
    else if(softMax2 >= 0.5)return 1;
    else return -1;
    
}