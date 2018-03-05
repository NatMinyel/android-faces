package com.nathnael.faces;

/**
 * Created by nathnel on 1/12/18.
 */
import android.app.ActionBar;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.media.Image;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.ImageView;
import com.bumptech.glide.Glide;
import com.bumptech.glide.load.engine.DiskCacheStrategy;
import com.bumptech.glide.request.RequestOptions;

public class PhotoActivity extends AppCompatActivity {

    public static final String EXTRA_PHOTO = "PhotoActivity.PHOTO";
    private ImageView mImageView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_photo_view);

        mImageView = (ImageView) findViewById(R.id.image);
        ImageModel photo = getIntent().getParcelableExtra(EXTRA_PHOTO);
        setTitle(photo.getTitle());
        RequestOptions requestOptions = new RequestOptions();
        requestOptions.diskCacheStrategy(DiskCacheStrategy.ALL);
        requestOptions.error(R.drawable.ic_cloud_off_red);

        Glide.with(this)
                .setDefaultRequestOptions(requestOptions)
                .asBitmap()
                .load(photo.getUrl())
                .into(mImageView);
    }
}