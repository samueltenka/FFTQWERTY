#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>

#include <random>
#include <time.h>

using namespace cv;

float rrand() {
   return ((float)rand())/RAND_MAX;
}

int main()
{
    // Data for visual representation
    int width = 512, height = 512;
    Mat image = Mat::zeros(height, width, CV_8UC3);

    // Set up training data
    srand(time(NULL));
    const int nn = 50;
    
    float trainingData[nn][2];
    for(int i=0; i<nn; ++i) {
         trainingData[i][0] = rrand()*256 + rrand()*256;
         trainingData[i][1] = rrand()*256+ rrand()*256;
    }
    Mat trainingDataMat(nn, 2, CV_32FC1, trainingData);

    float labels[nn];
    for(int i=0; i<nn; ++i) {
       float dist = (trainingData[i][0]-255)*(trainingData[i][0]-255) +
                    (trainingData[i][1]-255)*(trainingData[i][1]-255);
       labels[i] = dist>128*128? -1 : +1;
    }
    Mat labelsMat(nn, 1, CV_32FC1, labels);

    // Set up SVM's parameters
    CvSVMParams params;
    params.svm_type    = CvSVM::C_SVC;
    params.kernel_type = CvSVM::RBF;
    params.term_crit   = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);
    params.gamma = 1.0/(128*128);

    // Train the SVM
    CvSVM SVM;
    SVM.train(trainingDataMat, labelsMat, Mat(), Mat(), params);

    Vec3b green(0,255,255), blue (255,255,0);
    // Show the decision regions given by the SVM
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            Mat sampleMat = (Mat_<float>(1,2) << j,i);
            float response = SVM.predict(sampleMat);

            if (response == 1)
                image.at<Vec3b>(i,j)  = green;
            else if (response == -1)
                image.at<Vec3b>(i,j)  = blue;
        }
    }

    // Show the training data
    int thickness = -1;
    int lineType = 8;
    for(int i=0; i<nn; ++i) {
       circle( image, Point(trainingData[i][0],  trainingData[i][1]), 5,
                      Scalar(  128+128*labels[i],   128+128*labels[i],   128+128*labels[i]), thickness, lineType);
    }

    // Show support vectors
    thickness = 2;
    lineType  = 8;
    int c     = SVM.get_support_vector_count();

    for (int i = 0; i < c; ++i)
    {
        const float* v = SVM.get_support_vector(i);
        circle( image,  Point( (int) v[0], (int) v[1]),   6,  Scalar(128, 128, 128), thickness, lineType);
    }

    imwrite("result.png", image);        // save the image

    imshow("SVM Simple Example", image); // show it to the user
    waitKey(0);

}
