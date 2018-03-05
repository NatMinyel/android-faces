#include <jni.h>
#include <string>
#include <dlib/dnn.h>
#include <dlib/clustering.h>
#include <dlib/string.h>
#include <fstream>
#include <sstream>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <android/log.h>
#include <android/bitmap.h>
using namespace std;
using namespace dlib;


#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "dlib-jni:", __VA_ARGS__))


template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual = add_prev1<block<N,BN,1,tag1<SUBNET>>>;

template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual_down = add_prev2<avg_pool<2,2,2,2,skip1<tag2<block<N,BN,2,tag1<SUBNET>>>>>>;

template <int N, template <typename> class BN, int stride, typename SUBNET>
using block  = BN<con<N,3,3,1,1,relu<BN<con<N,3,3,stride,stride,SUBNET>>>>>;

template <int N, typename SUBNET> using ares      = relu<residual<block,N,affine,SUBNET>>;
template <int N, typename SUBNET> using ares_down = relu<residual_down<block,N,affine,SUBNET>>;

template <typename SUBNET> using alevel0 = ares_down<256,SUBNET>;
template <typename SUBNET> using alevel1 = ares<256,ares<256,ares_down<256,SUBNET>>>;
template <typename SUBNET> using alevel2 = ares<128,ares<128,ares_down<128,SUBNET>>>;
template <typename SUBNET> using alevel3 = ares<64,ares<64,ares<64,ares_down<64,SUBNET>>>>;
template <typename SUBNET> using alevel4 = ares<32,ares<32,ares<32,SUBNET>>>;

using anet_type = loss_metric<fc_no_bias<128,avg_pool_everything<alevel0<alevel1<alevel2<alevel3<alevel4<max_pool<3,3,2,2,relu<affine<con<32,7,7,2,2, input_rgb_image_sized<150>>>>>>>>>>>>>;

frontal_face_detector f_detector;
shape_predictor sp;
anet_type net;
array2d<rgb_pixel> img;


matrix<float, 0, 1> arr_to_mat(float *arr) {
    dlib::array<float> float_array = dlib::array<float>();
    for (long i = 0; i<128; i++) {
        float tmp = arr[i];
        float_array.push_back(tmp);
    }
    auto float_mat = mat(float_array);
    auto float_matrix = matrix<float, 0, 1>(float_mat);
    return float_matrix;
}

void stringToArr(string s, float *arr) {
    stringstream iss(s);
    float num; int count = 0;
    while (iss>>num) {
        arr[count] = num;
        count++;
    }
}



void initialize() {

    f_detector = get_frontal_face_detector();
    try {
        dlib::deserialize("storage/emulated/0/Faces/shape_predictor_68_face_landmarks.dat") >> sp;

        __android_log_print(ANDROID_LOG_VERBOSE, "FROM_NDK", "Done with the face landmarks.");
        dlib::deserialize("storage/emulated/0/Faces/dlib_face_recognition_resnet_model_v1.dat") >> net;

    } catch (error e) {
        __android_log_print(ANDROID_LOG_VERBOSE, "FROM_NDK", "Error");
    }
}
void convertBitmapToArray2d(JNIEnv* env,
                            jobject bitmap,
                            dlib::array2d<dlib::rgb_pixel>& out) {
    AndroidBitmapInfo bitmapInfo;
    void* pixels;
    int state;

    if (0 > (state = AndroidBitmap_getInfo(env, bitmap, &bitmapInfo))) {
        LOGI("L%d: AndroidBitmap_getInfo() failed! error=%d", __LINE__, state);
        return;
    } else if (bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        LOGI("L%d: Bitmap format is not RGB_565!", __LINE__);
    }

    // Lock the bitmap for copying the pixels safely.
    if (0 > (state = AndroidBitmap_lockPixels(env, bitmap, &pixels))) {
        LOGI("L%d: AndroidBitmap_lockPixels() failed! error=%d", __LINE__, state);
        return;
    }

    LOGI("L%d: info.width=%d, info.height=%d", __LINE__, bitmapInfo.width, bitmapInfo.height);
    out.set_size((long) bitmapInfo.height, (long) bitmapInfo.width);

    char* line = (char*) pixels;
    for (int h = 0; h < bitmapInfo.height; ++h) {
        for (int w = 0; w < bitmapInfo.width; ++w) {
            uint32_t* color = (uint32_t*) (line + 4 * w);

            out[h][w].red = (unsigned char) (0xFF & ((*color) >> 24));
            out[h][w].green = (unsigned char) (0xFF & ((*color) >> 16));
            out[h][w].blue = (unsigned char) (0xFF & ((*color) >> 8));
        }

        line = line + bitmapInfo.stride;
    }

    // Unlock the bitmap.
    AndroidBitmap_unlockPixels(env, bitmap);
}

//extern "C"
//JNIEXPORT jstring
//
//JNICALL
//Java_com_nathnael_faces_MainActivity_stringFromJNI(
//        JNIEnv *env,
//        jobject /* this */) {
//    std::string hello = "Hello from C++";
//    return env->NewStringUTF(hello.c_str());
//}


extern "C"
JNIEXPORT jint JNICALL
Java_com_nathnael_faces_FullscreenActivity_getFaces(JNIEnv *env, jobject instance, jobject bmap, jstring name) {
    if (sp.num_features() == 0) {
        LOGI("Here");
        initialize();
        LOGI("Here Again");
    }

    try {
        convertBitmapToArray2d(env, bmap, img);
    } catch (error e) {
        LOGI(e.what());
    }
    LOGI("Image Loaded");
    std::vector<matrix<rgb_pixel>> faces;
    for (auto face : f_detector(img)) {
        auto shape = sp(img, face);
        matrix<rgb_pixel> face_chip;
        extract_image_chip(img, get_face_chip_details(shape,150,0.25), face_chip);
        faces.push_back(move(face_chip));
    }

    LOGI(::to_string(faces.size()).data());
    if (faces.size() == 0) {
        LOGI("No faces");
        return 0;
    }
//    } else if (faces.size() > 1) {
//        return faces.size();
//    }

    std::vector<matrix<float,0,1>> face_descriptors = net(faces);

    ofstream f("/storage/emulated/0/faces.txt", ios::app);
    if (f.is_open()) {
        f<<env->GetStringUTFChars(name, 0)<<"|"<<trans(face_descriptors[0])<<"\n";
    }
    LOGI("Face Descriptors");
    return faces.size();
}




extern "C"
JNIEXPORT void JNICALL
Java_com_nathnael_faces_FullscreenActivity_getStoredFaces(JNIEnv *env, jobject instance) {
    std::vector<matrix<float,0,1>> stored_face_descriptors;
    std::vector<string> names;
    ifstream f("/storage/emulated/0/faces.txt");
    if (!f.bad()) {
        LOGI("Hello from here");
        string name;
        matrix<float,0,1> stored_face;
        string l;
        string delimiter = "|";
        while(getline(f, l)) {
//            stored_face_descriptors.push_back(stored_face);
            size_t pos = 0;
            string token;
            LOGI("Hello from here2");
            while((pos = l.find(delimiter)) != string::npos) {
                token = l.substr(0, pos);
                names.push_back(token);
                l.erase(0, pos+delimiter.length());
            }

            float a[128];
            stringToArr(l, a);
            stored_face_descriptors.push_back(arr_to_mat(a));
            LOGI("Hello from here4");
        }
        LOGI(names[1].c_str());
    }

    LOGI("Hello from here3");
}


void getStore() {
    std::vector<matrix<float,0,1>> stored_face_descriptors;
    std::vector<string> names;
    ifstream f("/storage/emulated/0/faces.txt");
    if (!f.bad()) {
        LOGI("Hello from here");
        string name;
        matrix<float,0,1> stored_face;
        string l;
        string delimiter = "|";
        while(getline(f, l)) {
//            stored_face_descriptors.push_back(stored_face);
            size_t pos = 0;
            string token;
            LOGI("Hello from here2");
            while((pos = l.find(delimiter)) != string::npos) {
                token = l.substr(0, pos);
                names.push_back(token);
                l.erase(0, pos+delimiter.length());
            }

            float a[128];
            stringToArr(l, a);
            stored_face_descriptors.push_back(arr_to_mat(a));
            LOGI("Hello from here4");
        }
        LOGI(names[1].c_str());
    }

    LOGI("Hello from here3");
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_nathnael_faces_FullscreenActivity_compareFaces(JNIEnv *env, jobject instance, jobject bmp) {
//    if (stored_face_descriptors.empty()) {
//        LOGI("TTTTTTTTTTTTTTTTTTTTTT");
//        getStore();
//    }
    std::vector<matrix<float,0,1>> stored_face_descriptors;
    std::vector<string> names;

    if (sp.num_features() == 0) {
        LOGI("Here");
        initialize();
        LOGI("Here Again");
    }
    array2d<rgb_pixel> imag;
    try {
        convertBitmapToArray2d(env, bmp, imag);
    } catch (error e) {
        LOGI(e.what());
    }
    LOGI("Image Loaded");
    std::vector<matrix<rgb_pixel>> faces_in_im;
    for (auto face : f_detector(imag)) {
        auto shape = sp(imag, face);
        matrix<rgb_pixel> face_chip;
        extract_image_chip(imag, get_face_chip_details(shape,150,0.25), face_chip);
        faces_in_im.push_back(move(face_chip));
    }

    LOGI(::to_string(faces_in_im.size()).data());
    if (faces_in_im.size() == 0) {
        LOGI("No faces");
        string s = "No faces found in image.";
        return env->NewStringUTF(s.c_str());
    }
//    } else if (faces.size() > 1) {
//        return faces.size();
//    }
    LOGI("HERE");
    std::vector<matrix<float,0,1>> face_descriptors_in_im = net(faces_in_im);

    string resp = "";
    for (int i = 0; i < stored_face_descriptors.size(); i++) {
//        LOGI("FSFFSFSFS");
        if (length(stored_face_descriptors[i] - face_descriptors_in_im[0]) < 0.6) {
            LOGI(names[i].c_str());
            resp += names[i];
        }
    }

    LOGI(resp.c_str());
    return env->NewStringUTF(resp.data());
}



//extern "C"
//JNIEXPORT jstring
//
//JNICALL
//Java_com_nathnael_faces_FullscreenActivity_stringFromJNI(
//        JNIEnv *env,
//        jobject /* this */) {
//    LOGI("Hello from here");
//    fstream f("/storage/emulated/0/faces.txt", ios::app | ios::in);
//    LOGI("Hello from here");
//    if (f.is_open()) {
//        string name;
//        char delim;
//        matrix<float,0,1> stored_face;
//        while((f>>name>>delim>>stored_face) && (delim == '|')) {
//            stored_face_descriptors.push_back(stored_face);
//            names.push_back(name);
//        }
//    }
//    LOGI(names[0].c_str());
//
//    std::string hello = "Hello from C++";
//    return env->NewStringUTF(hello.c_str());
//}

