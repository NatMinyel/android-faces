package com.nathnael.faces;

import android.content.Context;
import android.os.Environment;
import android.support.annotation.NonNull;
import android.support.annotation.RawRes;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

/**
 * Created by nathnel on 1/15/18.
 */

public class ProjectConstants {
    @NonNull
    public static String getFolderPath() {
        return Environment.getExternalStorageDirectory() + File.separator + "Faces";
    }

    @NonNull
    public static String getLandmarksPath() {
        return Environment.getExternalStorageDirectory() + File.separator + "Faces" + File.separator + "shape_predictor_68_face_landmarks.dat";
    }

    @NonNull
    public static String getPreTrainedPath() {
        return Environment.getExternalStorageDirectory() + File.separator + "Faces" + File.separator + "dlib_face_recognition_resnet_model_v1.dat";
    }

    public static void copyFileFromRawToOthers(@NonNull final Context context, @RawRes int id, @NonNull final String targetPath) {
        InputStream in = context.getResources().openRawResource(id);
        FileOutputStream out = null;
        try {
            out = new FileOutputStream(targetPath);
            byte[] buff = new byte[1024];
            int read = 0;
            while ((read = in.read(buff)) > 0) {
                out.write(buff, 0, read);
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                if (in != null) {
                    in.close();
                }
                if (out != null) {
                    out.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
