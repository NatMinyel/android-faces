package com.nathnael.faces;

import android.os.Environment;
import android.os.Parcel;
import android.os.Parcelable;
import android.util.Log;

import java.io.File;
import java.util.ArrayList;

/**
 * Created by nathnel on 1/12/18.
 */

public class ImageModel implements Parcelable {
    private String mUrl;
    private String mTitle;

    public ImageModel(String url, String title) {
        mUrl = url;
        mTitle = title;
    }

    protected ImageModel(Parcel in) {
        mUrl = in.readString();
        mTitle = in.readString();
    }

    public static final Creator<ImageModel> CREATOR = new Creator<ImageModel>() {
        @Override
        public ImageModel createFromParcel(Parcel in) {
            return new ImageModel(in);
        }

        @Override
        public ImageModel[] newArray(int size) {
            return new ImageModel[size];
        }
    };

    public String getUrl() {
        return mUrl;
    }

    public void setUrl(String url) {
        mUrl = url;
    }

    public String getTitle() {
        return mTitle;
    }

    public void setTitle(String title) {
        mTitle = title;
    }

    public static ImageModel[] getPhotos() {
        ArrayList<ImageModel> ims = new ArrayList<ImageModel>();
        File folder = new File(Environment.getExternalStorageDirectory() + "/Pictures/Saved Faces");
        try {
            if (folder.exists() && folder.listFiles().length != 0) {
                for (File f : folder.listFiles()) {
                    ims.add(new ImageModel(f.toString(), f.getName()));
                }
            } else {
                return null;
            }
        } catch (NullPointerException e) {
            return null;
        }
//        Log.e("Loooooog", "Found " + ims.size());
        return ims.toArray(new ImageModel[ims.size()]);
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel parcel, int i) {
        parcel.writeString(mUrl);
        parcel.writeString(mTitle);
    }
}
