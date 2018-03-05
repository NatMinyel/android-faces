package com.nathnael.faces;


import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.design.widget.BottomNavigationView;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.GridLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

import com.bumptech.glide.Glide;
import com.bumptech.glide.request.RequestOptions;

import java.io.File;


public class MainActivity extends AppCompatActivity {

    RecyclerView recyclerView;

    private class ImageGalleryAdapter extends RecyclerView.Adapter<ImageGalleryAdapter.MyViewHolder>  {

        private ImageModel[] mPhotos;
        private Context mContext;

        ImageGalleryAdapter(Context context, ImageModel[] photos) {
            mContext = context;
            mPhotos = photos;
        }

        @Override
        public ImageGalleryAdapter.MyViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {

            Context context = parent.getContext();
            LayoutInflater inflater = LayoutInflater.from(context);
            View photoView = inflater.inflate(R.layout.item_photo, parent, false);
            ImageGalleryAdapter.MyViewHolder viewHolder = new ImageGalleryAdapter.MyViewHolder(photoView);
            return viewHolder;
        }

        @Override
        public void onBindViewHolder(ImageGalleryAdapter.MyViewHolder holder, int position) {
            ImageModel photo = mPhotos[position];
            ImageView imageView = holder.mPhotoImageView;

            RequestOptions op = new RequestOptions();
            op.placeholder(R.drawable.ic_cloud_off_red);

            Glide.with(mContext)
                    .setDefaultRequestOptions(op)
                    .load(photo.getUrl())
                    .into(imageView);
        }

        @Override
        public int getItemCount() {
            if (mPhotos != null)
                return (mPhotos.length);
            return 0;
        }

        public class MyViewHolder extends RecyclerView.ViewHolder implements View.OnClickListener {

            public ImageView mPhotoImageView;

            public MyViewHolder(View itemView) {

                super(itemView);
                mPhotoImageView = (ImageView) itemView.findViewById(R.id.iv_photo);
                itemView.setOnClickListener(this);
            }

            @Override
            public void onClick(View view) {

                int position = getAdapterPosition();
                if(position != RecyclerView.NO_POSITION) {
                    ImageModel photo = mPhotos[position];
                    Intent intent = new Intent(mContext, PhotoActivity.class);
                    intent.putExtra(PhotoActivity.EXTRA_PHOTO, photo);
                    startActivity(intent);
                }
            }
        }
    }

    private BottomNavigationView.OnNavigationItemSelectedListener mOnNavigationItemSelectedListener
            = new BottomNavigationView.OnNavigationItemSelectedListener() {
        @Override
        public boolean onNavigationItemSelected(@NonNull MenuItem item) {
            switch (item.getItemId()) {
                case R.id.navigation_home:
//                    mTextMessage.setText(R.string.title_home);
//                    File folder = new File(Environment.getExternalStorageDirectory() + "/Pictures/Saved Faces");
//                    if (!folder.exists() || folder.listFiles().length == 0) {
//                        mTextMessage.setText("There are currently no faces stored.");
//                    } else {
//                        String files = "";
//                        for (File f : folder.listFiles()) {
//                            files += (f.toString() + "\n");
//                        }
//                        mTextMessage.setText(files);
//                    }
                    return true;
                case R.id.navigation_dashboard:
//                    mTextMessage.setText(R.string.title_camera);
                    setToFull();
                    return true;
                case R.id.navigation_notifications:
                    setToMembers();
                    return true;
            }
            return false;
        }
    };


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


//        mTextMessage = (TextView) findViewById(R.id.message);
        BottomNavigationView navigation = (BottomNavigationView) findViewById(R.id.navigation);
        navigation.setOnNavigationItemSelectedListener(mOnNavigationItemSelectedListener);

        File folderPath = new File(ProjectConstants.getFolderPath());
        if (!folderPath.exists()) {
            folderPath.mkdir();
        }
        if (!new File(ProjectConstants.getLandmarksPath()).exists()) {
            ProjectConstants.copyFileFromRawToOthers(getApplicationContext(), R.raw.shape_predictor_68_face_landmarks, ProjectConstants.getLandmarksPath());
        }
        if (!new File(ProjectConstants.getPreTrainedPath()).exists()) {
            ProjectConstants.copyFileFromRawToOthers(getApplicationContext(), R.raw.dlib_face_recognition_resnet_model_v1, ProjectConstants.getPreTrainedPath());
        }

        RecyclerView.LayoutManager layoutManager = new GridLayoutManager(this, 2);
        recyclerView = (RecyclerView) findViewById(R.id.rv_images);
        recyclerView.setHasFixedSize(true);
        recyclerView.setLayoutManager(layoutManager);

        ImageGalleryAdapter adapter = new ImageGalleryAdapter(this, ImageModel.getPhotos());
        recyclerView.setAdapter(adapter);
    }

    private void setToFull() {
        Intent cameraIntent = new Intent(this, FullscreenActivity.class);
        cameraIntent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        startActivity(cameraIntent);
    }

    private void setToMembers() {
        Intent memberIntent = new Intent(this, MemberActivity.class);
        memberIntent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        startActivity(memberIntent);
    }
}
