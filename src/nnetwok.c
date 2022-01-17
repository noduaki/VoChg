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
            sum += (*(diffData + i) / 6553.0) * *(w[0].weight + k + (NUM_DATA_X * i));
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
            sum += *(tmp1 + i) * *(w[1].weight + k + (NEURONS_2 * i));
        }
        *(tmp2 + k) = sum + *(w[1].bias + k);
        sum = 0;
    }

    for (k = 0; k < NEURONS_2; k++) {
        if (*(tmp2 + k) < 0.0) *(tmp2 + k) = 0.0;
    }

    // Output layer
    sum = 0;
    for (k = 0; k < NEURONS_3; k++) {
        for (i = 0; i < NEURONS_2; i++) {
            sum += *(tmp2 + i) * *(w[2].weight + k + (NEURONS_3 * i));
        }
        *(tmp1 + k) = sum + *(w[2].bias + k);
        sum = 0;
    }

    softSum = exp(*(tmp1)) + exp(*(tmp1 + 1));
    softMax1 = exp(*(tmp1)) / softSum;
    softMax2 = exp(*(tmp1 + 1)) / softSum;

    if (softMax2 < 0.5)
        return 0;
    else if (softMax2 >= 0.5)
        return 1;
    else
        return -1;
}

int judgeWave2(weights* w, double* diffData, double* tmp1, double* tmp2, double* a, double* b) {

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
            sum += (*(diffData + i) / 6553.0) * *(w[0].weight + k + (NUM_DATA_X * i));
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
            sum += *(tmp1 + i) * *(w[1].weight + k + (NEURONS_2 * i));
        }
        *(tmp2 + k) = sum + *(w[1].bias + k);
        sum = 0;
    }

    for (k = 0; k < NEURONS_2; k++) {
        if (*(tmp2 + k) < 0.0) *(tmp2 + k) = 0.0;
    }

    // Output layer
    sum = 0;
    for (k = 0; k < NEURONS_3; k++) {
        for (i = 0; i < NEURONS_2; i++) {
            sum += *(tmp2 + i) * *(w[2].weight + k + (NEURONS_3 * i));
        }
        *(tmp1 + k) = sum + *(w[2].bias + k);
        sum = 0;
    }

    softSum = exp(*(tmp1)) + exp(*(tmp1 + 1));
    *(a) = exp(*(tmp1)) / softSum;
    *(b) = exp(*(tmp1 + 1)) / softSum;

    if (softMax2 < 0.5)
        return 0;
    else if (softMax2 >= 0.5)
        return 1;
    else
        return -1;
}

void testnn(GtkWidget* window, gpointer data) {

    VApp* da = (VApp*)data;

    GFile* W_file = NULL;
    char name[256];
    char* read = NULL;

    int i;
    int m;
    int k;
    int dataSize;
    int tmp = 0;
    int num[20] = {0};
    int charSize[20] = {0};
    int jj = 0;

    char* string = NULL;
    char* stopstring = NULL;
    char* strtmp = NULL;
    errno = 0;
    double x;
    double a;
    double b;
    double* dataDiff;
    double* tmp1 = (double*)malloc(65536 * sizeof(double));
    double* tmp2 = (double*)malloc(65536 * sizeof(double));

    GError* fileErr = NULL;
    GFileInputStream* inStream = NULL;
    GFileInfo* info = NULL;
    int total_size = -1;
    gboolean err = 0;
    size_t length = 0;

    err = getFilePath(W_file, name);
    if (err) {
        printf("Error in Weight func.\n");
    }
    W_file = g_file_new_for_path(name);

    printf("%s\n", name);

    if (W_file != NULL) {
        inStream = g_file_read(W_file, NULL, &fileErr);
        if (fileErr != NULL) {
            printf("Could not open %s for reading: %s \n", name, fileErr->message);
        }
        info = g_file_input_stream_query_info(G_FILE_INPUT_STREAM(inStream), G_FILE_ATTRIBUTE_STANDARD_SIZE, NULL,
                                              &fileErr);
        if (info) {
            if (g_file_info_has_attribute(info, G_FILE_ATTRIBUTE_STANDARD_SIZE))
                total_size = g_file_info_get_size(info);
            printf("total_size = %d\n", total_size);

        } else {
            printf("Error in Test NN -> Can't get info\n");
        }

        if (total_size > 0) {

            if (read == NULL) {
                read = (char*)calloc(total_size, 1);
                if ((length = g_input_stream_read(G_INPUT_STREAM(inStream), read, total_size, NULL, &fileErr)) != -1) {

                    printf("reading file length = %ld\n", length);
                } else {

                    printf("Error in Test NN -> file read\n");
                }
            }
        }
        err = g_input_stream_close(G_INPUT_STREAM(inStream), NULL, &fileErr);
        if (err == FALSE) printf("Error in Test NN -> stream_close failed\n");
        g_object_unref(inStream);
        g_object_unref(info);
        g_clear_object(&W_file);
    } else {
        return;
    }

    // k is num of w and b It's k-layers nn network
    // num[k] is size of each w or b

    k = 0;
    dataSize = 0;
    tmp = 0;
    for (i = 0; i < total_size; i++) {
        if (*(read + i) == 0x2c || *(read + i) == 0x0a) dataSize++; // 0x2c = ',' 0x0d = CR

        if (*(read + i) == 0x0) break;
    }
    // Error check ****
    if (i != total_size) {
        printf("Error in getWeight -> readsize\n");
        strcat(da->statusBuf, " Error-> weght file reading");
        statusprint(data);
        return;
    }

    dataDiff = (double*)malloc(dataSize * sizeof(double));

    strtmp = read;
    for (i = 0; i < dataSize; i++) {
        *(dataDiff + i) = strtod(strtmp, &stopstring);
        strtmp = stopstring + 1;

        if (errno == ERANGE) {
            printf("Error in get diffData ->%d,,%d\n", errno, ERANGE);
            return;
        }
        if (strtmp == stopstring) {
            printf("Error in get diffData -> Can't Convert\n");
            return;
        }
    }

    for (i = 0; i < dataSize; i += NUM_DATA_X) {

        jj = judgeWave2(da->W, dataDiff + i, tmp1, tmp2, &a, &b);
        printf("%d-->%f==%f\n", i / 128, a, b);
    }

    free(read);
    read = NULL;
    free(dataDiff);
    dataDiff = NULL;
    free(tmp1);
    tmp1 = NULL;
    free(tmp2);
    tmp2 = NULL;
}
