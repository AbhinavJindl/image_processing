/*
----------------------readme---------------

--------how to compile--------------
$g++ a2_abhinav_2016csb1026.cpp -o output `pkg-config --cflags --libs opencv`
$./output

-------------references--------------
1)Learn Opencv for triangulation syntax and understanding the concept of how to use triangulation
2)Material provided by sir for concept understanding
3)github for input example


--------assumptions---------------
1)input and output images and text file should be present in same folder.
2)text file should have y coordinate followed by x coordinate.
3)Size of image can change for part 1 but not for part 2.

*/
#include <iostream>
#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <cmath>
#include <fstream>
# define pi  3.141592653589793238462643383279502884
using namespace std;
using namespace cv;

float area(float t0,float t1,float t2,float t3,float t4,float t5){
	float area=(abs((t0-t2)*(t5-t1)-(t0-t4)*(t3-t1)))/2;
	return area;
}


void part1(string image1,int noOfImages){
    Mat image = imread(image1,0);
    Mat T(2,3,CV_32F,0.0);
    //T.at<float>(0,1)=1;
    //T.at<float>(1,0)=-1;
    //T.at<float>(0,0)=2;
    //T.at<float>(1,1)=2;
    cout<<"Enter the transf matrix:";
    for(int i=0;i<2;i++){
        for(int j=0;j<3;j++){
            cin>>T.at<float>(i,j);
        }
    }

    vector<Point2f>initialcoord,tempinitialcoord;
    initialcoord.push_back(Point2f(0,0));
    initialcoord.push_back(Point2f(image.rows,0));
    initialcoord.push_back(Point2f(0,image.cols));
    initialcoord.push_back(Point2f(image.rows,image.cols));

    tempinitialcoord.push_back(Point2f(0,0));
    tempinitialcoord.push_back(Point2f(image.rows,0));
    tempinitialcoord.push_back(Point2f(0,image.cols));

    float finalcoord[4][2]={0.0};
    for(int i=0;i<4;i++){
        //cout<<initialcoord[i]<<endl;
            finalcoord[i][0]=initialcoord[i].x*T.at<float>(0,0)+initialcoord[i].y*T.at<float>(0,1)+T.at<float>(0,2);
            finalcoord[i][1]=initialcoord[i].x*T.at<float>(1,0)+initialcoord[i].y*T.at<float>(1,1)+T.at<float>(0,2);
    }
    for(int n=0;n<=noOfImages;n++){
        Mat input=image.clone();
        float alpha=(float)n/noOfImages;
        vector<Point2f> cornerpoints;
        cornerpoints.push_back(Point2f((1-alpha)*0+(alpha)*finalcoord[0][0],(1-alpha)*0+(alpha)*finalcoord[0][1]));
        cornerpoints.push_back(Point2f((1-alpha)*initialcoord[1].x+(alpha)*finalcoord[1][0],(1-alpha)*initialcoord[1].y+(alpha)*finalcoord[1][1]));
        cornerpoints.push_back(Point2f((1-alpha)*initialcoord[2].x+(alpha)*finalcoord[2][0],(1-alpha)*initialcoord[2].y+(alpha)*finalcoord[2][1]));
        cornerpoints.push_back(Point2f((1-alpha)*initialcoord[3].x+(alpha)*finalcoord[3][0],(1-alpha)*initialcoord[3].y+(alpha)*finalcoord[3][1]));
        int maxR=0;
        int minR=image.rows;
        int maxC=0;
        int minC=image.cols;
        for(int i=0;i<cornerpoints.size();i++){
            //cout<<cornerpoints[i]<<endl;
                if(cornerpoints[i].x<minR){
                    minR=cornerpoints[i].x;
                }
                if(cornerpoints[i].y<minC){
                    minC=cornerpoints[i].y;
                }
                if(cornerpoints[i].x>maxR){
                    maxR=cornerpoints[i].x;
                }
                if(cornerpoints[i].y>maxC){
                    maxC=cornerpoints[i].y;
                } 
        }
        cornerpoints.pop_back();
        Mat transf=getAffineTransform(cornerpoints,tempinitialcoord);
        int colsize=maxC-minC;
        int rowsize=maxR-minR;
        Mat out(rowsize,colsize,CV_8U,0.0);
        for(int i=0;i<rowsize;i++){
            for(int j=0;j<colsize;j++){
                int x=transf.at<double>(0,0)*(i+minR)+transf.at<double>(0,1)*(j+minC)+transf.at<double>(0,2);
                int y=transf.at<double>(1,0)*(i+minR)+transf.at<double>(1,1)*(j+minC)+transf.at<double>(1,2);
                if(x>=0&&y>=0&&x<input.rows&&y<input.cols)
                out.at<uchar>(i,j)=input.at<uchar>(x,y);
            }
        }
        
ostringstream str1;
            str1 << n;
            string n1 = str1.str();
            string a="./images/morph"+n1+".jpg";
            imwrite(a,out);


    }

}

void part2(string image1,string image2,string file1,string file2,int noOfImages){
    Mat oriimg = imread(image1,0);
    Mat finalimg = imread(image2,0);
    Size size = oriimg.size();
    cout<<size.height<<" "<<size.width<<endl;
    Rect rect(0, 0,  size.height,size.width);
    vector<Point2f> points[2];
    char* File1=new char(file1.size());
    for(int i=0;i<file1.size();i++){
        File1[i]=file1[i];
    }
    char* File2=new char(file2.size());
    for(int i=0;i<file2.size();i++){
        File2[i]=file2[i];
    }
    cout<<File1<<" "<<File2<<endl;
    int x, y,fx,fy;

    ifstream ift(File2);
    //ift.open (File2);
    if (ift.is_open())
    {
       // cout << "File successfully open";
    }
    else
    {   
        cout<<"2\n";
        cout << "Error opening file";
        return;
    }
    while(ift >>fy>>fx)
    {
        //cout<<"csd\n";
        points[1].push_back(Point2f(fx,fy));
    }
    ifstream ifs(File1);
    if (ifs.is_open())
    {
        //cout << "File successfully open";
    }
    else
    {   
        cout<<"!\n";
        cout << "Error opening file";
        return ;
    }

    while(ifs >>y>>x)
    {
        points[0].push_back(Point2f(x,y));
        //points[1].push_back(Point2f(fx,fy));
    }
    
    Mat finalout[noOfImages+1];
    //loop for no of frames
    //cout<<"2\n";   
    for(int n=0;n<=noOfImages;n++){
        Mat out(oriimg.rows,oriimg.cols,CV_8U,0.0);
        Subdiv2D subdiv(rect);
        vector<Point2f> intpoints;
        //intermediate points for triangulation
        for(int i=0;i<points[0].size();i++){
            intpoints.push_back(Point2f(points[0][i].x*(1-(float)((float)n/noOfImages))+points[1][i].x*(float)((float)n/noOfImages),points[0][i].y*(1-((float)n/noOfImages))+points[1][i].y*((float)n/noOfImages)));
            //cout<<intpoints[i]<<endl;
        }
        for( vector<Point2f>::iterator it = intpoints.begin(); it != intpoints.end(); it++)
        {
            subdiv.insert(*it);
         
        }
          
        Mat img;
        img=oriimg.clone();

        //making 3 triangle lists for initial ,final and intermediate images and one temporary
        vector<Vec6f> temptriangleList,triangleList,initialtriangleList,finaltriangleList;
        subdiv.getTriangleList(temptriangleList);
        Vec6f ini,f,t;
        vector<Point> pt(3);
        for(size_t it=0;it<temptriangleList.size();it++){
            int temp=0;
            for(int i=0;i<6;i++){
                if(temptriangleList[it][i]>=0 && temptriangleList[it][i]<img.rows &&  i%2==0){
                    temp++;
                }
                else if(temptriangleList[it][i]>=0 && temptriangleList[it][i]<img.cols && i%2==1){
                    temp++;
                }
            }
            if(temp==6){
                triangleList.push_back(temptriangleList[it]);
            }
        }
        for( size_t it = 0; it < triangleList.size(); it++ ){
            t = triangleList[it];
            pt[0] = Point(cvRound(t[0]), cvRound(t[1]));
            pt[1] = Point(cvRound(t[2]), cvRound(t[3]));
            pt[2] = Point(cvRound(t[4]), cvRound(t[5]));
            int minI=min(cvRound(t[0]),cvRound(t[2]));
            minI=min(minI,cvRound(t[4]));
            int minJ=min(cvRound(t[1]),cvRound(t[3]));
            minJ=min(minJ,cvRound(t[5]));
            int maxI=max(cvRound(t[0]),cvRound(t[2]));
            maxI=max(maxI,cvRound(t[4]));
            int maxJ=max(cvRound(t[1]),cvRound(t[3]));
            maxJ=max(maxJ,cvRound(t[5]));
            //cout<<pt[0]<<" cds "<<pt[1]<<" Csd "<<pt[2]<<endl;
            /*if (rect.contains(pt[0]) && rect.contains(pt[1]) && rect.contains(pt[2])){
                line(img, pt[0], pt[1], delaunay_color, 1);
                line(img, pt[1], pt[2], delaunay_color, 1);
                line(img, pt[2], pt[0], delaunay_color, 1);
            }*/

            //getting initial anf final points with respect to triangle
            for( int i=0;i<intpoints.size();i++) {
                if(intpoints[i].x==t[0]&&intpoints[i].y==t[1]){
                    ini[0]=points[0][i].x;
                    ini[1]=points[0][i].y;
                    f[0]=points[1][i].x;
                    f[1]=points[1][i].y;
                    break;
                 }       
            }
            for( int i=0;i<intpoints.size();i++) {
                if(intpoints[i].x==t[2]&&intpoints[i].y==t[3]){
                    ini[2]=points[0][i].x;
                    ini[3]=points[0][i].y;
                    f[2]=points[1][i].x;
                    f[3]=points[1][i].y;
                    break;
                }       
            }
            for( int i=0;i<intpoints.size();i++) {
                if(intpoints[i].x==t[4]&&intpoints[i].y==t[5]){
                    ini[4]=points[0][i].x;
                    ini[5]=points[0][i].y;
                    f[4]=points[1][i].x;
                    f[5]=points[1][i].y;
                    break;
                 }       
            }
            initialtriangleList.push_back(ini);
            finaltriangleList.push_back(f);

            //vector created to give as input to get affine
            vector<Point2f> intpointstransf;
            vector<Point2f> inipointstransf;
            vector<Point2f> finalpointstransf;
            intpointstransf.push_back(Point2f(cvRound(triangleList[it][0]),cvRound(triangleList[it][1])));
            intpointstransf.push_back(Point2f(cvRound(triangleList[it][2]),cvRound(triangleList[it][3])));
            intpointstransf.push_back(Point2f(cvRound(triangleList[it][4]),cvRound(triangleList[it][5])));
            inipointstransf.push_back(Point2f(cvRound(initialtriangleList[it][0]),cvRound(initialtriangleList[it][1])));
            inipointstransf.push_back(Point2f(cvRound(initialtriangleList[it][2]),cvRound(initialtriangleList[it][3])));
            inipointstransf.push_back(Point2f(cvRound(initialtriangleList[it][4]),cvRound(initialtriangleList[it][5])));
            finalpointstransf.push_back(Point2f(cvRound(finaltriangleList[it][0]),cvRound(finaltriangleList[it][1])));
            finalpointstransf.push_back(Point2f(cvRound(finaltriangleList[it][2]),cvRound(finaltriangleList[it][3])));
            finalpointstransf.push_back(Point2f(cvRound(finaltriangleList[it][4]),cvRound(finaltriangleList[it][5])));
            
            //get transform matrices
            Mat t1=getAffineTransform(intpointstransf,inipointstransf);
            Mat t2=getAffineTransform(intpointstransf,finalpointstransf);
            int count=0;
            for(int r=minI-1;r<maxI+1;r++){
                for(int c=minJ-1;c<maxJ+1;c++){
                    float area1=area(t[0],t[1],t[2],t[3],t[4],t[5]);
                    float area2=area(t[0],t[1],t[2],t[3],(float)r,(float)c);
                    float area3=area(t[0],t[1],t[4],t[5],(float)r,(float)c);
                    float area4=area(t[4],t[5],t[2],t[3],(float)r,(float)c);
                    if(abs(area1-(area2+area3+area4))<0.01){
                        int inir=t1.at<double>(0,0)*(double)r+t1.at<double>(0,1)*(double)c+t1.at<double>(0,2);
                        int inic=t1.at<double>(1,0)*(double)r+t1.at<double>(1,1)*(double)c+t1.at<double>(1,2);
                        int finr=t2.at<double>(0,0)*(double)r+t2.at<double>(0,1)*(double)c+t2.at<double>(0,2);
                        int finc=t2.at<double>(1,0)*(double)r+t2.at<double>(1,1)*(double)c+t2.at<double>(1,2);
                        out.at<uchar>(r,c)=(uchar)(oriimg.at<uchar>(inir,inic)*(1-((float)n/noOfImages))+finalimg.at<uchar>(finr,finc)*(((float)n/noOfImages)));
                    }
                }
            }


        }
        //imshow("morph",out);
        //waitKey(0);
        finalout[n]=out.clone();
    }
    bool f=true;
        for(int n=0;n<=noOfImages;n++){
            ostringstream str1;
            str1 << n;
            string n1 = str1.str();
            string a="./images2/morph"+n1+".jpg";
            imwrite(a,finalout[n]);

            imshow("morph",finalout[n]);
            waitKey(100);
        }
    

}









int main(int argc, char** argv){
 
    string imagetitle= "Morphing";
    Scalar delaunay_color(255,255,255), points_color(0, 0, 255);
    bool part;
    int noOfImages;
    string image1,image2;
    cout<<"Enter the part you want to execute(0 or 1):";
    cin>>part;
    if(part==0){
        cout<<"Enter image name:";
        cin>>image1;
        cout<<"Enter no of frames";
        cin>>noOfImages;
        noOfImages--;
        part1(image1,noOfImages);
    }
    else if(part==1){
        cout<<"Enter first image name:";
        cin>>image1;
        cout<<"Enter second image name:";
        cin>>image2;
        string file1,file2;
        cout<<"Enter file names in resp. order(files should have tie points as y x):";
        cin>>file1>>file2;
        cout<<"Enter no of frames";
        cin>>noOfImages;
        noOfImages--;
        //image1="image1.jpg";
        //image2="image2.jpg";
        //file1="1.txt";
        //file2="2.txt";
        part2(image1,image2,file1,file2,noOfImages);
    }
    
 
    return 0;
}


