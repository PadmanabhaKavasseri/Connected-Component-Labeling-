#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include "bmplib.h"
#include "queue.h"
using namespace std;

void usage() {
    cerr << "usage: ./label <options>" << endl;
    cerr <<"Examples" << endl;
    cerr << "./label test_queue" << endl;
    cerr << "./label gray <input file> <outputfile>" << endl;
    cerr << "./label binary <inputfile> <outputfile>" << endl;
    cerr << "./label segment <inputfile> <outputfile>" << endl;
}

//function prototypes
void test_queue();
void rgb2gray(unsigned char ***in,unsigned char **out,int height,int width);
void gray2binary(unsigned char **in,unsigned char **out,int height,int width);
int component_labeling(unsigned char **binary_image,int **labeled_image,int height,int width);
void label2RGB(int  **labeled_image, unsigned char ***rgb_image,int num_segment,int height,int width);
void clean(unsigned char ***input,unsigned char **gray,unsigned char **binary, int **labeled_image,int height , int width);
void BFS(unsigned char **binary_image,int **label, Location location, int *current_label, int height,int width);



int main(int argc,char **argv) {

    srand( time(0) );
    if(argc < 2 )  {
        usage();
        return -1;
    }
    unsigned char ***input=0;
    unsigned char **gray=0;
    unsigned char **binary=0;
    int **labeled_image=0;
    if( strcmp("test_queue",argv[1]) == 0 ) {
        test_queue();
    }
    else if(strcmp("gray",argv[1]) == 0 ) {
        if(argc <4 ) {
            cerr << "not enough arguemnt for gray" << endl;
            return -1;
        }
        int height,width;
        input = readRGBBMP(argv[2],&height,&width);
        if(input == 0)
        {
            cerr << "unable to open " << argv[2] << " for input." << endl;
            return -1;
        }
        gray = new unsigned char*[height];
        for(int i=0;i<height;i++)
            gray[i] = new unsigned char[width];
        rgb2gray(input,gray,height,width);
        if(writeGSBMP(argv[3],gray,height,width) != 0) {
            cerr << "error writing file " << argv[3] << endl;
            clean(input,gray,binary,labeled_image,height,width);
            return -1;
        }
        clean(input,gray,binary,labeled_image,height,width);

    }
    else if(strcmp("binary",argv[1]) == 0 ) {
        if(argc <4 ) {
            cerr << "not enough arguemnt for binary" << endl;
            return -1;
        }
        int height,width;
        input = readRGBBMP(argv[2],&height,&width);
        if(input == 0)
        {
            cerr << "unable to open " << argv[2] << " for input." << endl;
            clean(input,gray,binary,labeled_image,height,width);
            return -1;
        }

        gray = new unsigned char*[height];
        for(int i=0;i<height;i++)
            gray[i] = new unsigned char[width];
        rgb2gray(input,gray,height,width);

        binary = new unsigned char*[height];
        for(int i=0;i<height;i++)
            binary[i] = new unsigned char[width];

        gray2binary(gray,binary,height,width);
        if(writeBinary(argv[3],binary,height,width) != 0) {
            cerr << "error writing file " << argv[3] << endl;
            clean(input,gray,binary,labeled_image,height,width);
            return -1;
        }
        clean(input,gray,binary,labeled_image,height,width);

    }
    else if(strcmp("segment",argv[1]) == 0 ) {
        if(argc <4 ) {
            cerr << "not enough arguemnt for segment" << endl;
            return -1;
        }
        int height,width;
        input = readRGBBMP(argv[2],&height,&width);
        if(input == 0)
        {
            cerr << "unable to open " << argv[2] << " for input." << endl;
            clean(input,gray,binary,labeled_image,height,width);
            return -1;
        }

        gray = new unsigned char*[height];
        for(int i=0;i<height;i++)
            gray[i] = new unsigned char[width];
        rgb2gray(input,gray,height,width);

        binary = new unsigned char*[height];
        for(int i=0;i<height;i++)
            binary[i] = new unsigned char[height];

        gray2binary(gray,binary,height,width);


        labeled_image = new int*[height];
        for(int i=0;i<height;i++) labeled_image[i] = new int[width];
        int segments= component_labeling(binary, labeled_image,height,width);

        for(int i=0;i<height;i++)
            for(int j=0;j<width;j++)
                for(int k=0;k<RGB;k++)
                    input[i][j][k] = 0;
        label2RGB(labeled_image, input ,segments, height,width);
        if(writeRGBBMP(argv[3],input,height,width) != 0) {
            cerr << "error writing file " << argv[3] << endl;
            clean(input,gray,binary,labeled_image,height,width);
            return -1;
        }
        clean(input,gray,binary,labeled_image,height,width);

    }

    return 0;
}



void test_queue() {
    // create some locations;
    Location three_one, two_two;
    three_one.row = 3; three_one.col = 1;
    two_two.row = 2; two_two.col = 2;

    //create an Queue with max capacity 5
    Queue q(5);

    cout << boolalpha;
    cout << q.is_empty() << endl;           // true
    q.push(three_one);
    cout << q.is_empty() << endl;           // false
    q.push(two_two);

    Location loc = q.pop();
    cout << loc.row << " " << loc.col << endl; // 3 1
    loc = q.pop();
    cout << loc.row << " " << loc.col << endl; // 2 2
    cout << q.is_empty() << endl;           // true
}

//Loop over the 'in' image array and calculate the single 'out' pixel value using the formula
// GS = 0.2989 * R + 0.5870 * G + 0.1140 * B
void rgb2gray(unsigned char ***in,unsigned char **out,int height,int width) {
    double temp = 0;
    for (int i=0; i<(height); i++){
        for (int j=0; j<(width); j++) {

            double temp = (double)(in[i][j][0] * 0.2989) + (double)(in[i][j][1] * 0.5870) + (double)(in[i][j][2] * 0.1140);
            if (temp > 255){
                temp = 255;
            }
            if (temp < 0){
                temp = 0;
            }
            out[i][j] = (unsigned char)temp;

        }
    }

}

//Loop over the 'in' gray scale array and create a binary (0,1) valued image 'out'
//Set the 'out' pixel to 1 if 'in' is above the threshold, else 0
void gray2binary(unsigned char **in,unsigned char **out,int height,int width) {
    for (int i=0; i<(height); i++) {
        for (int j = 0; j < (width); j++) {
            if(in[i][j]>THRESHOLD){
                out[i][j] = 1;
            }
            else{
                out[i][j] = 0;
            }
        }
    }
}

//This is the function that does the work of looping over the binary image and doing the connected component labeling
int component_labeling(unsigned char **binary_image,int **label,int height,int width) {
    int current_label = 1;
    for (int i=0; i<(height); i++) {
        for (int j = 0; j < (width); j++) {
            Location location;
            location.row = i;
            location.col = j;
            if ((binary_image[i][j] != 0)&&(label[i][j] == 0)){
                label[i][j] = current_label;
                BFS(binary_image,label,location,&current_label,height,width);
                current_label++;
            }
        }
    }

return current_label;
}
void BFS(unsigned char **binary_image,int **label, Location location, int *current_label, int height,int width){
    Location temp;
    Location holder;
    Queue q ( height*width ) ;
    q.push(location);
    while (!(q.is_empty())) {
        temp = q.pop();
        //bottom
        if (((temp.row + 1) < height-1) && ((temp.row + 1) >= 0) && (binary_image[temp.row + 1][temp.col] == 1) && (label[temp.row + 1][temp.col] == 0)) {
            holder.row = temp.row+1;
            holder.col = temp.col;
            q.push(holder);
            label[temp.row + 1][temp.col]  = *current_label;
        }
        //top
        if (((temp.row - 1) <= height-1) && ((temp.row - 1) >= 0) && (binary_image[temp.row - 1][temp.col] == 1) && (label[temp.row - 1][temp.col] == 0)) {
            holder.row = temp.row-1;
            holder.col = temp.col;
            q.push(holder);
            label[temp.row - 1][temp.col]  = *current_label;
        }
        //right
        if (((temp.col + 1) < width-1) && ((temp.col + 1 >= 0)) && (binary_image[temp.row][temp.col + 1] == 1) && (label[temp.row][temp.col + 1] == 0)) {
            holder.row = temp.row;
            holder.col = temp.col+1;
            q.push(holder);
            label[temp.row][temp.col + 1] = *current_label;
        }
        //left
        if (((temp.col-1) <= width-1) && ((temp.col-1) >= 0) && (binary_image[temp.row][temp.col - 1] == 1) && (label[temp.row][temp.col - 1] == 0)) {
            holder.row = temp.row;
            holder.col = temp.col-1;
            q.push(holder);
            label[temp.row][temp.col - 1] = *current_label;
        }
    }
}



//First make num_segments random colors to use for coloring the labeled parts of the image.
//Then loop over the labeled image using the label to index into your random colors array.
//Set the rgb_pixel to the corresponding color, or set to black if the pixel was unlabeled.
#ifndef AUTOTEST
void label2RGB(int  **labeled_image, unsigned char ***rgb_image,int num_segments,int height,int width){
    int* colors = new int[num_segments];
    for (int i=0; i<num_segments; i++){
        colors[i] = rand() % 255;
    }

    for (int i=0; i<(height); i++) {
        for (int j = 0; j < (width); j++){
            if (labeled_image[i][j] != 0){
                rgb_image[i][j][0] = colors[labeled_image[i][j]];
                rgb_image[i][j][1] = colors[labeled_image[i][j]];
                rgb_image[i][j][2] = colors[labeled_image[i][j]];
            }
            else{
                rgb_image[i][j][0] = 0;
                rgb_image[i][j][1] = 0;
                rgb_image[i][j][2] = 0;
            }
        }
    }//0 is black and 1 is white
    delete [] colors;
}
#endif

//deletes all of the dynamic arrays created
void clean(unsigned char ***input,unsigned char **gray,unsigned char **binary, int **labeled_image,int height , int width) {
    if(input ) {
        for (int i=0; i<(height); i++) {
            for (int j = 0; j < (width); j++) {
                delete [] input [i][j];
            }
            delete [] input[i];
        }

        delete [] input;
    }
    if(gray) {
        for (int i=0; i<(height); i++) {
            delete [] gray [i];
        }
        delete [] gray;
    }
    if(binary) {
        for (int i=0; i<(height); i++) {
            delete [] binary [i];

        }
        delete [] binary;
    }
    if(labeled_image) {
        for (int i=0; i<(height); i++) {
            delete [] labeled_image[i];
        }
        delete [] labeled_image;
        
    }


}
