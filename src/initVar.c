#include "header.h"

int initVar(VApp* data) {
    int i;
    int error = 0;
    int ok = 0;
    GError* fileErr = NULL;
    GFileInputStream* inStream = NULL;
    GFileInfo* info = NULL;
    int total_size = -1;
    gboolean err = 0;
    size_t length = 0;

    if (data->status.selNum == 1 || data->status.selNum == 2) {
        data->settings = gSet;

        // DrawArea **********

        for (i = 0; i < 5; i++) {
            data->draw1[i].x = (double*)malloc(data->settings.frames * data->settings.channels * sizeof(double));
            if (data->draw1[i].x == NULL) error = 1;
            data->draw1[i].y = (double*)malloc(data->settings.frames * data->settings.channels * sizeof(double));
            if (data->draw1[i].y == NULL) error = 1;
            data->draw1[i].pos = (int*)malloc(data->settings.frames * data->settings.channels * sizeof(int));
            if (data->draw1[i].pos == NULL) error = 1;
            data->draw1[i].Height = 65536;
            data->draw1[i].Width = data->settings.frames;
            data->draw1[i].on = 0;
            data->draw1[i].log = 0;
            data->draw1[i].bar = 0;

            data->draw2[i].x = (double*)malloc(data->settings.frames * data->settings.channels * sizeof(double));
            if (data->draw2[i].x == NULL) error = 1;
            data->draw2[i].y = (double*)malloc(data->settings.frames * data->settings.channels * sizeof(double));
            if (data->draw2[i].y == NULL) error = 1;
            data->draw2[i].pos = (int*)malloc(data->settings.frames * data->settings.channels * sizeof(int));
            if (data->draw2[i].pos == NULL) error = 1;
            data->draw2[i].Height = 65536;
            data->draw2[i].Width = data->settings.frames / 2;
            data->draw2[i].on = 0;
            data->draw2[i].log = 0;
            data->draw2[i].bar = 0;
        }
        data->crossPoint.x = (double*)malloc(data->settings.frames * sizeof(double));
        if (data->crossPoint.x == NULL) error = 1;
        data->crossPoint.y = (double*)malloc(data->settings.frames * sizeof(double));
        if (data->crossPoint.y == NULL) error = 1;
        data->crossPoint.pos = (int*)calloc(data->settings.frames * data->settings.channels, sizeof(int));
        if (data->crossPoint.pos == NULL) error = 1;
        data->crossPoint.Height = 0;
        data->crossPoint.Width = 0;
        data->crossPoint.on = 0;
        data->crossPoint.log = 0;
        data->crossPoint.bar = 0;

        // Sound Data ***********

        data->dataBuf.sound = (double*)malloc(data->settings.frames * data->settings.channels * sizeof(double) * 2);
        if (data->dataBuf.sound == NULL) error = 1;
        data->dataBuf.soundSize = data->settings.frames * data->settings.channels * 2;
        data->dataBuf.row = (double*)malloc(data->settings.frames * data->settings.channels * sizeof(double) * 2);
        if (data->dataBuf.row == NULL) error = 1;
        data->dataBuf.rowSize = data->settings.frames * data->settings.channels * 2;
        data->dataBuf.tmp = (double*)malloc(data->settings.frames * data->settings.channels * sizeof(double) * 2);
        if (data->dataBuf.tmp == NULL) error = 1;
        data->dataBuf.tmpSize = data->settings.frames * data->settings.channels * 2;

        if (gSet.readfile != NULL) {
            inStream = g_file_read(gSet.readfile, NULL, &fileErr);
            if (fileErr != NULL) {
                printf("Could not open %s for reading: %s \n", gSet.readfilename, fileErr->message);
                data->flag.soundFile = 0;
            }
            info = g_file_input_stream_query_info(G_FILE_INPUT_STREAM(inStream), G_FILE_ATTRIBUTE_STANDARD_SIZE, NULL,
                                                  &fileErr);
            if (info) {
                if (g_file_info_has_attribute(info, G_FILE_ATTRIBUTE_STANDARD_SIZE))
                    total_size = g_file_info_get_size(info);
                printf("total_size = %d\n", total_size);

            } else {
                printf("Error in initVar -> Can't get info\n");
                data->flag.soundFile = 0;
            }

            if (total_size > 0) {
                if (data->dataBuf.read == NULL) {
                    data->dataBuf.read = (int16_t*)calloc(total_size, 1);
                    data->dataBuf.write = (int16_t*)calloc(total_size, 1);

                    if ((length = g_input_stream_read(G_INPUT_STREAM(inStream), data->dataBuf.read, total_size, NULL,
                                                      &fileErr)) != -1) {
                        data->dataBuf.readSize = length;
                        printf("reading file length = %ld\n", length);
                    } else {
                        data->dataBuf.readSize = 0;
                        data->flag.soundFile = 0;
                        printf("Error in initVar -> file read\n");
                    }

                } else {
                    data->flag.soundFile = 0;
                    printf("Error in initVar -> Buffer busy\n");
                }
            }
            err = g_input_stream_close(G_INPUT_STREAM(inStream), NULL, &fileErr);
            if (err == FALSE) printf("Error in initVar -> stream_close failed\n");
            g_object_unref(inStream);
            g_object_unref(info);
            g_clear_object(&gSet.readfile);
        }
        if (data->flag.soundFile == 1) {
            ok = wavCheck(data);
            if (ok) {
                data->flag.soundFile = 0;
                error = 1;
            }
        }

        data->soundRead.samples = (int16_t*)malloc(data->settings.frames * data->settings.channels * sizeof(short));
        if (data->soundRead.samples == NULL) error = 1;
        data->soundWrite.samples = (int16_t*)malloc(data->settings.frames * data->settings.channels * sizeof(short));
        if (data->soundWrite.samples == NULL) error = 1;
        data->soundRead.areas = (snd_pcm_channel_area_t*)malloc(sizeof(snd_pcm_channel_area_t) * 2);
        if (data->soundRead.areas == NULL) error = 1;
        data->soundWrite.areas = (snd_pcm_channel_area_t*)malloc(sizeof(snd_pcm_channel_area_t) * 2);
        if (data->soundWrite.areas == NULL) error = 1;

        data->soundRead.phase = 0.0;
        data->soundWrite.phase = 0.0;
        data->soundRead.pos = 0;
        data->soundWrite.pos = 0;
        data->soundRead.ready = 0;
        data->soundWrite.ready = 0;

        // FFT ***********

        data->fftForward.dataIn =
            (fftw_complex*)fftw_malloc(data->settings.frames * data->settings.channels * sizeof(fftw_complex));
        if (data->fftForward.dataIn == NULL) error = 1;
        data->fftForward.dataOut =
            (fftw_complex*)fftw_malloc(data->settings.frames * data->settings.channels * sizeof(fftw_complex));
        if (data->fftForward.dataOut == NULL) error = 1;
        data->fftForward.fftPlan = fftw_plan_dft_1d(data->settings.frames, data->fftForward.dataIn,
                                                    data->fftForward.dataOut, FFTW_FORWARD, FFTW_ESTIMATE);
        data->fftForward.size = data->settings.frames * data->settings.channels;

        data->fftBack.dataIn =
            (fftw_complex*)fftw_malloc(data->settings.frames * data->settings.channels * sizeof(fftw_complex));
        if (data->fftBack.dataIn == NULL) error = 1;
        data->fftBack.dataOut =
            (fftw_complex*)fftw_malloc(data->settings.frames * data->settings.channels * sizeof(fftw_complex));
        if (data->fftBack.dataOut == NULL) error = 1;
        data->fftBack.size = data->settings.frames * data->settings.channels;
        data->fftBack.fftPlan = fftw_plan_dft_1d(data->settings.frames, data->fftBack.dataIn, data->fftBack.dataOut,
                                                 FFTW_BACKWARD, FFTW_ESTIMATE);
        data->fftCep.dataIn =
            (fftw_complex*)fftw_malloc(data->settings.frames * data->settings.channels * sizeof(fftw_complex));
        if (data->fftCep.dataIn == NULL) error = 1;
        data->fftCep.dataOut =
            (fftw_complex*)fftw_malloc(data->settings.frames * data->settings.channels * sizeof(fftw_complex));
        if (data->fftCep.dataOut == NULL) error = 1;
        data->fftCep.fftPlan = fftw_plan_dft_1d(data->settings.frames, data->fftCep.dataIn, data->fftCep.dataOut,
                                                FFTW_FORWARD, FFTW_ESTIMATE);
        data->fftCep.size = data->settings.frames * data->settings.channels;

        data->fftWindow = (double*)malloc(data->settings.frames * data->settings.channels * sizeof(double));
        if (data->fftWindow == NULL) error = 1;
        data->fftPower = (double*)malloc(data->settings.frames * data->settings.channels * sizeof(double));
        if (data->fftPower == NULL) error = 1;
        data->fftCepstrum = (double*)malloc(data->settings.frames * data->settings.channels * sizeof(double));
        if (data->fftCepstrum == NULL) error = 1;
        for (i = 0; i < data->settings.frames; i++) {
            *(data->fftWindow + i) =
                (0.54 - 0.46 * cos((G_PI * 2.0) * (double)i / (double)(data->settings.frames - 1)));
        }
        // etc.
        for (i = 2; i < 14; i++) {
            int* flag = (int*)&data->flag;
            *(flag + i) = 0;
        }
        for (i = 0; i < 11; i++) {
            int* flag = (int*)&data->mlFlag;
            *(flag + i) = 0;
        }
        data->flag.drawResize = 1;
        data->status.filter = 0; // IIR Lo

        data->selPointS.x = 0;
        data->selPointE.x = 0;
        data->nextPoint.x = 0;
        data->nextPoint.y = 0;
        data->nextPoint.z = 0;
        data->drawstatus1.log = 0; // Use drawArea X
        data->drawstatus1.on = 0;  // Use drawArea Y
        data->drawstatus1.x = (double*)malloc(4 * sizeof(double));
        if (data->drawstatus1.x == NULL) error = 1;
        *(data->drawstatus1.x) = 0.0;
        data->drawstatus1.y = (double*)malloc(4 * sizeof(double));
        if (data->drawstatus1.y == NULL) error = 1;
        *(data->drawstatus1.y) = 0.0;
        gtk_button_set_label(GTK_BUTTON(data->priv->button1), "IIR L");
        gtk_button_set_label(GTK_BUTTON(data->priv->button2), "Row");
        gtk_button_set_label(GTK_BUTTON(data->priv->button3), "Procd");
        gtk_button_set_label(GTK_BUTTON(data->priv->button4), "Oct Hi");
        gtk_button_set_label(GTK_BUTTON(data->priv->button5), "Oct Lo");
        gtk_button_set_label(GTK_BUTTON(data->priv->button6), "6");
        gtk_button_set_label(GTK_BUTTON(data->priv->button7), "7");
        gtk_button_set_label(GTK_BUTTON(data->priv->button8), "8");
        gtk_button_set_label(GTK_BUTTON(data->priv->button11), "Log");
        gtk_button_set_label(GTK_BUTTON(data->priv->button12), "Spec");
        gtk_button_set_label(GTK_BUTTON(data->priv->button13), "Ceps");
        gtk_button_set_label(GTK_BUTTON(data->priv->button14), "14");
        gtk_button_set_label(GTK_BUTTON(data->priv->button15), "15");
        gtk_button_set_label(GTK_BUTTON(data->priv->button16), "16");
        gtk_button_set_label(GTK_BUTTON(data->priv->button17), "17");
        gtk_button_set_label(GTK_BUTTON(data->priv->button18), "18");
        gtk_button_set_label(GTK_BUTTON(data->priv->button19), "19");
        gtk_button_set_label(GTK_BUTTON(data->priv->button20), "Pause");

    } else if (data->status.selNum == 3) {

        // 333333**************

        // DrawArea ***********
        data->settings = gSet;
        for (i = 0; i < 5; i++) {
            data->draw1[i].x = (double*)malloc(data->settings.frames * data->settings.channels * sizeof(double));
            if (data->draw1[i].x == NULL) error = 1;
            data->draw1[i].y = (double*)malloc(data->settings.frames * data->settings.channels * sizeof(double));
            if (data->draw1[i].y == NULL) error = 1;
            data->draw1[i].pos = (int*)malloc(data->settings.frames * data->settings.channels * sizeof(int));
            if (data->draw1[i].pos == NULL) error = 1;
            data->draw1[i].Height = 65536;
            data->draw1[i].Width = data->settings.frames;
            data->draw1[i].on = 0;
            data->draw1[i].log = 0;
            data->draw1[i].bar = 0;

            data->draw2[i].x = (double*)malloc(data->settings.frames * data->settings.channels * sizeof(double));
            if (data->draw2[i].x == NULL) error = 1;
            data->draw2[i].y = (double*)malloc(data->settings.frames * data->settings.channels * sizeof(double));
            if (data->draw2[i].y == NULL) error = 1;
            data->draw2[i].pos = (int*)malloc(data->settings.frames * data->settings.channels * sizeof(int));
            if (data->draw2[i].pos == NULL) error = 1;
            data->draw2[i].Height = 65536;
            data->draw2[i].Width = data->settings.frames;
            data->draw2[i].on = 0;
            data->draw2[i].log = 0;
            data->draw2[i].bar = 0;
        }
        data->crossPoint.x = (double*)malloc(data->settings.frames * sizeof(double));
        if (data->crossPoint.x == NULL) error = 1;
        data->crossPoint.y = (double*)malloc(data->settings.frames * sizeof(double));
        if (data->crossPoint.y == NULL) error = 1;
        data->crossPoint.pos = (int*)calloc(data->settings.frames * data->settings.channels, sizeof(int));
        if (data->crossPoint.pos == NULL) error = 1;
        data->crossPoint.Height = 0;
        data->crossPoint.Width = 0;
        data->crossPoint.on = 0;
        data->crossPoint.log = 0;
        data->crossPoint.bar = 0;

        // File read *********

        if (gSet.readfile != NULL) {
            inStream = g_file_read(gSet.readfile, NULL, &fileErr);
            if (fileErr != NULL) {
                printf("Could not open %s for reading: %s \n", gSet.readfilename, fileErr->message);
                data->flag.soundFile = 0;
            }
            info = g_file_input_stream_query_info(G_FILE_INPUT_STREAM(inStream), G_FILE_ATTRIBUTE_STANDARD_SIZE, NULL,
                                                  &fileErr);
            if (info) {
                if (g_file_info_has_attribute(info, G_FILE_ATTRIBUTE_STANDARD_SIZE))
                    total_size = g_file_info_get_size(info);
                printf("total_size = %d\n", total_size);

            } else {
                printf("Error in initVar -> Can't get info\n");
                data->flag.soundFile = 0;
            }

            if (total_size > 0) {
                if (data->dataBuf.read == NULL) {

                    data->dataBuf.read = (int16_t*)calloc(total_size, 1);
                    data->dataBuf.write = (int16_t*)calloc(total_size, 1);

                    if ((length = g_input_stream_read(G_INPUT_STREAM(inStream), data->dataBuf.read, total_size, NULL,
                                                      &fileErr)) != -1) {
                        data->dataBuf.readSize = length;
                        printf("reading file length = %ld\n", length);
                    } else {
                        data->dataBuf.readSize = 0;
                        data->flag.soundFile = 0;
                        printf("Error in initVar -> file read\n");
                    }

                } else {
                    data->flag.soundFile = 0;
                    printf("Error in initVar -> Buffer busy\n");
                }
            }
            err = g_input_stream_close(G_INPUT_STREAM(inStream), NULL, &fileErr);
            if (err == FALSE) printf("Error in initVar -> stream_close failed\n");
            g_object_unref(inStream);
            g_object_unref(info);
            g_clear_object(&gSet.readfile);
        }

        if (strlen(gSet.readfilename) < 199) strcpy(data->statusBuf, gSet.readfilename);
        statusprint((void*)data);

        if (data->flag.soundFile == 1) {
            ok = wavCheck(data);
            if (ok) {
                data->flag.soundFile = 0;
                error = 1;
            }
        }

        data->sData = (mlData*)malloc(NUM_MLDATA * sizeof(mlData));
        if (data->sData == NULL) error = 1;

        data->dataBuf.sound = (double*)malloc(data->settings.frames * data->settings.channels * sizeof(double) * 2);
        if (data->dataBuf.sound == NULL) error = 1;
        data->dataBuf.row = (double*)malloc(data->settings.frames * data->settings.channels * sizeof(double) * 2);
        if (data->dataBuf.row == NULL) error = 1;
        data->dataBuf.tmp = (double*)malloc(data->settings.frames * data->settings.channels * sizeof(double) * 2);
        if (data->dataBuf.tmp == NULL) error = 1;

        // ect **********
        // 0 clear flags
        for (i = 0; i < 14; i++) {
            int* flag = (int*)&data->flag;
            *(flag + i) = 0;
        }
        for (i = 0; i < 11; i++) {
            int* flag = (int*)&data->mlFlag;
            *(flag + i) = 0;
        }
        data->flag.drawResize = 1;
        data->status.filter = 0; // IIR Lo

        data->selPointS.x = 0;
        data->selPointE.x = 0;
        data->nextPoint.x = 0;
        data->nextPoint.y = 0;
        data->nextPoint.z = 0;
        data->drawstatus1.log = 0; // Use drawArea X
        data->drawstatus1.on = 0;  // Use drawArea Y
        data->drawstatus1.x = (double*)malloc(4 * sizeof(double));
        if (data->drawstatus1.x == NULL) error = 1;
        *(data->drawstatus1.x) = 0.0;
        data->drawstatus1.y = (double*)malloc(4 * sizeof(double));
        if (data->drawstatus1.y == NULL) error = 1;
        *(data->drawstatus1.y) = 0.0;
        gtk_button_set_label(GTK_BUTTON(data->priv->button1), "IIR L");
        gtk_button_set_label(GTK_BUTTON(data->priv->button2), "S >");
        gtk_button_set_label(GTK_BUTTON(data->priv->button3), "< S");
        gtk_button_set_label(GTK_BUTTON(data->priv->button4), "E >");
        gtk_button_set_label(GTK_BUTTON(data->priv->button5), "< E");
        gtk_button_set_label(GTK_BUTTON(data->priv->button6), "Ent.0");
        gtk_button_set_label(GTK_BUTTON(data->priv->button7), "Ent.1");
        gtk_button_set_label(GTK_BUTTON(data->priv->button8), "Auto -");
        gtk_button_set_label(GTK_BUTTON(data->priv->button11), "Dele 1");
        gtk_button_set_label(GTK_BUTTON(data->priv->button12), "<- 1");
        gtk_button_set_label(GTK_BUTTON(data->priv->button13), "1 ->");
        gtk_button_set_label(GTK_BUTTON(data->priv->button14), "Dele all");
        gtk_button_set_label(GTK_BUTTON(data->priv->button15), "15");
        gtk_button_set_label(GTK_BUTTON(data->priv->button16), "16");
        gtk_button_set_label(GTK_BUTTON(data->priv->button17), "17");
        gtk_button_set_label(GTK_BUTTON(data->priv->button18), "18");
        gtk_button_set_label(GTK_BUTTON(data->priv->button19), "Next");
        gtk_button_set_label(GTK_BUTTON(data->priv->button20), "Prev.");

    } else if (data->status.selNum == 5) {

        // 55555555 **************

        // DrawArea ***********
        data->settings = gSet;
        for (i = 0; i < 5; i++) {
            data->draw1[i].x = (double*)malloc(data->settings.frames * data->settings.channels * sizeof(double));
            if (data->draw1[i].x == NULL) error = 1;
            data->draw1[i].y = (double*)malloc(data->settings.frames * data->settings.channels * sizeof(double));
            if (data->draw1[i].y == NULL) error = 1;
            data->draw1[i].pos = (int*)malloc(data->settings.frames * data->settings.channels * sizeof(int));
            if (data->draw1[i].pos == NULL) error = 1;
            data->draw1[i].Height = 65536;
            data->draw1[i].Width = data->settings.frames;
            data->draw1[i].on = 0;
            data->draw1[i].log = 0;
            data->draw1[i].bar = 0;

            data->draw2[i].x = (double*)malloc(data->settings.frames * data->settings.channels * sizeof(double));
            if (data->draw2[i].x == NULL) error = 1;
            data->draw2[i].y = (double*)malloc(data->settings.frames * data->settings.channels * sizeof(double));
            if (data->draw2[i].y == NULL) error = 1;
            data->draw2[i].pos = (int*)malloc(data->settings.frames * data->settings.channels * sizeof(int));
            if (data->draw2[i].pos == NULL) error = 1;
            data->draw2[i].Height = 65536;
            data->draw2[i].Width = data->settings.frames;
            data->draw2[i].on = 0;
            data->draw2[i].log = 0;
            data->draw2[i].bar = 0;
        }
        data->crossPoint.x = (double*)malloc(data->settings.frames * sizeof(double));
        if (data->crossPoint.x == NULL) error = 1;
        data->crossPoint.y = (double*)malloc(data->settings.frames * sizeof(double));
        if (data->crossPoint.y == NULL) error = 1;
        data->crossPoint.pos = (int*)calloc(data->settings.frames * data->settings.channels, sizeof(int));
        if (data->crossPoint.pos == NULL) error = 1;
        data->crossPoint.Height = 0;
        data->crossPoint.Width = 0;
        data->crossPoint.on = 0;
        data->crossPoint.log = 0;
        data->crossPoint.bar = 0;

        data->selCross.x = (double*)malloc(data->settings.frames * sizeof(double));
        if (data->selCross.x == NULL) error = 1;
        data->selCross.y = (double*)malloc(data->settings.frames * sizeof(double));
        if (data->selCross.y == NULL) error = 1;
        data->selCross.pos = (int*)calloc(data->settings.frames * data->settings.channels, sizeof(int));
        if (data->selCross.pos == NULL) error = 1;
        data->selCross.Height = 0;
        data->selCross.Width = 0;
        data->selCross.on = 0;
        data->selCross.log = 0;
        data->selCross.bar = 0;

        // File read *********

        if (gSet.readfile != NULL) {
            inStream = g_file_read(gSet.readfile, NULL, &fileErr);
            if (fileErr != NULL) {
                printf("Could not open %s for reading: %s \n", gSet.readfilename, fileErr->message);
                data->flag.soundFile = 0;
            }
            info = g_file_input_stream_query_info(G_FILE_INPUT_STREAM(inStream), G_FILE_ATTRIBUTE_STANDARD_SIZE, NULL,
                                                  &fileErr);
            if (info) {
                if (g_file_info_has_attribute(info, G_FILE_ATTRIBUTE_STANDARD_SIZE))
                    total_size = g_file_info_get_size(info);
                printf("total_size = %d\n", total_size);

            } else {
                printf("Error in initVar -> Can't get info\n");
                data->flag.soundFile = 0;
            }

            if (total_size > 0) {
                if (data->dataBuf.read == NULL) {

                    data->dataBuf.read = (int16_t*)calloc(total_size, 1);
                    data->dataBuf.write = (int16_t*)calloc(total_size, 1);

                    if ((length = g_input_stream_read(G_INPUT_STREAM(inStream), data->dataBuf.read, total_size, NULL,
                                                      &fileErr)) != -1) {
                        data->dataBuf.readSize = length;
                        printf("reading file length = %ld\n", length);
                    } else {
                        data->dataBuf.readSize = 0;
                        data->flag.soundFile = 0;
                        printf("Error in initVar -> file read\n");
                    }

                } else {
                    data->flag.soundFile = 0;
                    printf("Error in initVar -> Buffer busy\n");
                }
            }
            err = g_input_stream_close(G_INPUT_STREAM(inStream), NULL, &fileErr);
            if (err == FALSE) printf("Error in initVar -> stream_close failed\n");
            g_object_unref(inStream);
            g_object_unref(info);
            g_clear_object(&gSet.readfile);
        }

        if (strlen(gSet.readfilename) < 199) strcpy(data->statusBuf, gSet.readfilename);
        statusprint((void*)data);

        if (data->flag.soundFile == 1) {
            ok = wavCheck(data);
            if (ok) {
                data->flag.soundFile = 0;
                error = 1;
            }
        }

        data->sData = (mlData*)malloc(NUM_MLDATA * sizeof(mlData));
        if (data->sData == NULL) error = 1;

        data->dataBuf.sound = (double*)malloc(data->settings.frames * data->settings.channels * sizeof(double) * 2);
        if (data->dataBuf.sound == NULL) error = 1;
        data->dataBuf.row = (double*)malloc(data->settings.frames * data->settings.channels * sizeof(double) * 2);
        if (data->dataBuf.row == NULL) error = 1;
        data->dataBuf.tmp = (double*)malloc(data->settings.frames * data->settings.channels * sizeof(double) * 2);
        if (data->dataBuf.tmp == NULL) error = 1;

        // ect **********
        // 0 clear flags
        for (i = 0; i < 14; i++) {
            int* flag = (int*)&data->flag;
            *(flag + i) = 0;
        }
        for (i = 0; i < 11; i++) {
            int* flag = (int*)&data->mlFlag;
            *(flag + i) = 0;
        }
        data->flag.drawResize = 1;
        data->status.filter = 0; // IIR Lo

        data->selPointS.x = 0;
        data->selPointE.x = 0;
        data->nextPoint.x = 0;
        data->nextPoint.y = 0;
        data->nextPoint.z = 0;
        data->drawstatus1.log = 0; // Use drawArea X
        data->drawstatus1.on = 0;  // Use drawArea Y
        data->drawstatus1.x = (double*)malloc(4 * sizeof(double));
        if (data->drawstatus1.x == NULL) error = 1;
        *(data->drawstatus1.x) = 0.0;
        data->drawstatus1.y = (double*)malloc(4 * sizeof(double));
        if (data->drawstatus1.y == NULL) error = 1;
        *(data->drawstatus1.y) = 0.0;
        gtk_button_set_label(GTK_BUTTON(data->priv->button1), "IIR L");
        gtk_button_set_label(GTK_BUTTON(data->priv->button2), "S >");
        gtk_button_set_label(GTK_BUTTON(data->priv->button3), "< S");
        gtk_button_set_label(GTK_BUTTON(data->priv->button4), "E >");
        gtk_button_set_label(GTK_BUTTON(data->priv->button5), "< E");
        gtk_button_set_label(GTK_BUTTON(data->priv->button6), "Ent.0");
        gtk_button_set_label(GTK_BUTTON(data->priv->button7), "Ent.1");
        gtk_button_set_label(GTK_BUTTON(data->priv->button8), "Auto -");
        gtk_button_set_label(GTK_BUTTON(data->priv->button11), "Dele 1");
        gtk_button_set_label(GTK_BUTTON(data->priv->button12), "<- 1");
        gtk_button_set_label(GTK_BUTTON(data->priv->button13), "1 ->");
        gtk_button_set_label(GTK_BUTTON(data->priv->button14), "Dele all");
        gtk_button_set_label(GTK_BUTTON(data->priv->button15), "15");
        gtk_button_set_label(GTK_BUTTON(data->priv->button16), "16");
        gtk_button_set_label(GTK_BUTTON(data->priv->button17), "17");
        gtk_button_set_label(GTK_BUTTON(data->priv->button18), "18");
        gtk_button_set_label(GTK_BUTTON(data->priv->button19), "Next");
        gtk_button_set_label(GTK_BUTTON(data->priv->button20), "Prev.");

    } else {
        printf("Error in initVar");
        exit(1);
    }
    return error;
}

int delVar(VApp* data) {
    int i;
    int error = 0;
    int* flag = NULL;

    if (data->status.selNum == 1 || data->status.selNum == 2) {

        for (i = 0; i < 5; i++) {
            free(data->draw1[i].x);
            data->draw1[i].x = NULL;
            free(data->draw1[i].y);
            data->draw1[i].y = NULL;
            free(data->draw1[i].pos);
            data->draw1[i].pos = NULL;
            free(data->draw2[i].x);
            data->draw2[i].x = NULL;
            free(data->draw2[i].y);
            data->draw2[i].y = NULL;
            free(data->draw2[i].pos);
            data->draw2[i].pos = NULL;

            data->draw1[i].on = 0;
            data->draw1[i].log = 0;
            data->draw1[i].bar = 0;

            data->draw2[i].on = 0;
            data->draw2[i].log = 0;
            data->draw2[i].bar = 0;
        }
        free(data->crossPoint.x);
        data->crossPoint.x = NULL;
        free(data->crossPoint.y);
        data->crossPoint.y = NULL;
        free(data->crossPoint.pos);
        data->crossPoint.pos = NULL;

        data->crossPoint.on = 0;
        data->crossPoint.log = 0;
        data->crossPoint.bar = 0;

        free(data->dataBuf.read);
        data->dataBuf.read = NULL;
        free(data->dataBuf.write);
        data->dataBuf.write = NULL;
        free(data->dataBuf.sound);
        data->dataBuf.sound = NULL;
        free(data->dataBuf.row);
        data->dataBuf.row = NULL;
        free(data->dataBuf.tmp);
        data->dataBuf.tmp = NULL;

        free(data->soundRead.samples);
        data->soundRead.samples = NULL;
        free(data->soundWrite.samples);
        data->soundWrite.samples = NULL;
        free(data->soundRead.areas);
        data->soundRead.areas = NULL;
        free(data->soundWrite.areas);
        data->soundWrite.areas = NULL;

        fftw_free(data->fftForward.dataIn);
        data->fftForward.dataIn = NULL;
        fftw_free(data->fftBack.dataIn);
        data->fftBack.dataIn = NULL;
        fftw_free(data->fftCep.dataIn);
        data->fftCep.dataIn = NULL;
        fftw_free(data->fftForward.dataOut);
        data->fftForward.dataOut = NULL;
        fftw_free(data->fftBack.dataOut);
        data->fftBack.dataOut = NULL;
        fftw_free(data->fftCep.dataOut);
        data->fftCep.dataOut = NULL;
        fftw_destroy_plan(data->fftForward.fftPlan);
        data->fftForward.fftPlan = NULL;
        fftw_destroy_plan(data->fftBack.fftPlan);
        data->fftBack.fftPlan = NULL;
        fftw_destroy_plan(data->fftCep.fftPlan);
        data->fftCep.fftPlan = NULL;
        free(data->fftWindow);
        data->fftWindow = NULL;
        free(data->fftPower);
        data->fftPower = NULL;
        free(data->fftCepstrum);
        data->fftCepstrum = NULL;

        free(data->drawstatus1.x);
        data->drawstatus1.x = NULL;
        free(data->drawstatus1.y);
        data->drawstatus1.y = NULL;

        data->status.selNum = 0;
        data->status.filter = 0; // IIR Lo

        flag = (int*)&data->flag;
        for (i = 0; i < 14; i++) {
            *(flag + i) = 0;
        }
        flag = (int*)&data->mlFlag;
        for (i = 0; i < 11; i++) {
            *(flag + i) = 0;
        }
        data->status.open = 0;

    } else if (data->status.selNum == 3) { //************************************:
        for (i = 0; i < 5; i++) {
            free(data->draw1[i].x);
            data->draw1[i].x = NULL;
            free(data->draw1[i].y);
            data->draw1[i].y = NULL;
            free(data->draw1[i].pos);
            data->draw1[i].pos = NULL;
            free(data->draw2[i].x);
            data->draw2[i].x = NULL;
            free(data->draw2[i].y);
            data->draw2[i].y = NULL;
            free(data->draw2[i].pos);
            data->draw2[i].pos = NULL;

            data->draw1[i].on = 0;
            data->draw1[i].log = 0;
            data->draw1[i].bar = 0;

            data->draw2[i].on = 0;
            data->draw2[i].log = 0;
            data->draw2[i].bar = 0;
        }
        free(data->crossPoint.x);
        data->crossPoint.x = NULL;
        free(data->crossPoint.y);
        data->crossPoint.y = NULL;
        free(data->crossPoint.pos);
        data->crossPoint.pos = NULL;

        data->crossPoint.on = 0;
        data->crossPoint.log = 0;
        data->crossPoint.bar = 0;

        free(data->dataBuf.read);
        data->dataBuf.read = NULL;
        free(data->dataBuf.write);
        data->dataBuf.write = NULL;
        free(data->sData);
        data->sData = NULL;

        free(data->dataBuf.sound);
        data->dataBuf.sound = NULL;
        free(data->dataBuf.row);
        data->dataBuf.row = NULL;
        free(data->dataBuf.tmp);
        data->dataBuf.tmp = NULL;

        free(data->drawstatus1.x);
        data->drawstatus1.x = NULL;
        free(data->drawstatus1.y);
        data->drawstatus1.y = NULL;

        data->status.selNum = 0;
        data->status.filter = 0; // IIR Lo
        flag = (int*)&data->flag;
        for (i = 0; i < 14; i++) {
            *(flag + i) = 0;
        }
        flag = (int*)&data->mlFlag;
        for (i = 0; i < 11; i++) {
            *(flag + i) = 0;
        }

        data->status.open = 0;

    } else if (data->status.selNum == 5) { //************************************:
        for (i = 0; i < 5; i++) {
            free(data->draw1[i].x);
            data->draw1[i].x = NULL;
            free(data->draw1[i].y);
            data->draw1[i].y = NULL;
            free(data->draw1[i].pos);
            data->draw1[i].pos = NULL;

            free(data->draw2[i].x);
            data->draw2[i].x = NULL;
            free(data->draw2[i].y);
            data->draw2[i].y = NULL;
            free(data->draw2[i].pos);
            data->draw2[i].pos = NULL;

            data->draw1[i].Height = 65536;
            data->draw1[i].Width = data->settings.frames;
            data->draw1[i].on = 0;
            data->draw1[i].log = 0;
            data->draw1[i].bar = 0;

            data->draw2[i].Height = 65536;
            data->draw2[i].Width = data->settings.frames;
            data->draw2[i].on = 0;
            data->draw2[i].log = 0;
            data->draw2[i].bar = 0;
        }
        free(data->crossPoint.x);
        data->crossPoint.x = NULL;
        free(data->crossPoint.y);
        data->crossPoint.y = NULL;
        free(data->crossPoint.pos);
        data->crossPoint.pos = NULL;

        data->crossPoint.Height = 0;
        data->crossPoint.Width = 0;
        data->crossPoint.on = 0;
        data->crossPoint.log = 0;
        data->crossPoint.bar = 0;

        free(data->selCross.x);
        data->selCross.x = NULL;
        free(data->selCross.y);
        data->selCross.y = NULL;
        free(data->selCross.pos);
        data->selCross.pos = NULL;

        data->selCross.Height = 0;
        data->selCross.Width = 0;
        data->selCross.on = 0;
        data->selCross.log = 0;
        data->selCross.bar = 0;

        free(data->dataBuf.read);
        data->dataBuf.read = NULL;
        free(data->dataBuf.write);
        data->dataBuf.write = NULL;
        free(data->sData);
        data->sData = NULL;

        free(data->dataBuf.sound);
        data->dataBuf.sound = NULL;
        free(data->dataBuf.row);
        data->dataBuf.row = NULL;
        free(data->dataBuf.tmp);
        data->dataBuf.tmp = NULL;

        free(data->drawstatus1.x);
        data->drawstatus1.x = NULL;
        free(data->drawstatus1.y);
        data->drawstatus1.y = NULL;

        data->status.selNum = 0;
        data->status.filter = 0; // IIR Lo
                                 // 0 set flags
        flag = (int*)&data->flag;
        for (i = 0; i < 14; i++) {
            *(flag + i) = 0;
        }
        flag = (int*)&data->mlFlag;
        for (i = 0; i < 11; i++) {
            *(flag + i) = 0;
        }

        data->status.open = 0;

    } else {
        printf("Error in delVar");
        exit(1);
    }
    return error;
}