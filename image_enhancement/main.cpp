/*
------------how to compile---------------------
go to the directory containing this file
$ g++ a1_abhinav_2016csb1026.cpp -o output `pkg-config --cflags --libs opencv`
$ ./output

----------instructions----------------
After running the file it will ask to enter image name and integer according to functionality needed which will take you to that function.
Respective inputs will be asked to enter if needed.
Image file should be present in the same directory as the file.

-----------------references----------------
I used the slides provided by Sir to understand some concepts and wiki for adaptive histogram concepts as that was not covered in class(did not see any code).
Web to find some inbuilt functions of opencv.

------------assumptions--------------
I have assumed wrap around condition for corner pixels in case of bilinear interpolation.
Final image size has been increased for rotation and tranlation but not for tie points. 
*/
#include <iostream>
#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <cmath>
# define pi  3.141592653589793238462643383279502884
using namespace std;
using namespace cv;
Mat increasewindow(Mat input,int size);
Mat negative(Mat input);
Mat resize_by_near(Mat input,float rowscale,float colscale);
Mat resize_by_bilinear(Mat input,float rowscale,float colscale);
Mat logtransformation(Mat input);
Mat gammatransformation(Mat input);
Mat bitplane(Mat input,int bit);
Mat bitplaneslicing(Mat input);
Mat translate(Mat input,int x,int y);
Mat shearbynear(Mat input,float s,char d);
Mat shearbybilinear(Mat input,float s,char d);
Mat rotatebynear(Mat input,double theta);
Mat rotatebybilinear(Mat input,double theta);
Mat histogramequal(Mat input);
Mat adaptivehist(Mat input);
Mat histogrammatching(Mat input,Mat result);
Mat tie(Mat input);
Mat piecewise(Mat input);

float error(Mat input,Mat output);



int main( int argc, const char** argv )
{	
	cout<<"Enter image name:";
    string imagename;
    cin>>imagename;
    Mat image = imread(imagename,0);
    if(image.empty())
    {
        cout<< "Cannot open image file "<<imagename<<endl;
        return -1;
    }
    cout<<"Enter any of the following integers to perform an operation on  the image:\n1-->negative\n2-->resize\n3-->log transformation\n4-->gammatransformation\n5-->bitplane slicing\n6-->translate\n7-->shear\n8-->rotate\n9-->histogram equalization\n10-->adaptive histogram equalization\n11-->histogram matching\n12->Image reconstruction given 4 tie points\n13-->Piecewise linear transformation\n";
    int inst;
    cin>>inst;
    Mat final;
    if(inst==1){
    	final=negative(image);
    }
    else if(inst==2){
    	cout<<"Enter 1 for nearest neighbour and 2 for bilinear:";
    	cin>>inst;
    	if(inst==1){
    		float rowscale,colscale;
    		cout<<"Enter scales along x and y axis:";
    		cin>>rowscale>>colscale;
    		final=resize_by_near(image,rowscale,colscale);
    	}
    	else if(inst==2){
    		float rowscale,colscale;
    		cout<<"Enter scales along x and y axis:";
    		cin>>rowscale>>colscale;
    		final=resize_by_bilinear(image,rowscale,colscale);
    	}
    	else{
    		cout<<"invalid\n";
    		return -1;
    	}
    }
    else if(inst==3){
    	final=logtransformation(image);
    }
    else if(inst==4){
    	final=gammatransformation(image);
    }
    else if(inst==5){
    	final=bitplaneslicing(image);
    }
    else if(inst==6){
    	int x,y;
    	cout<<"Enter transalate parameter along x and y axis:";
    	cin>>x>>y;
    	int size=max(abs(x),abs(y));
    	image=increasewindow(image,size);
    	final=translate(image,x,y);
    }
    else if(inst==7){
    	cout<<"Enter 1 for nearest neighbour and 2 for bilinear:";
    	cin>>inst;
    	if(inst==1){
    		float para;
    		cout<<"Enter 1 for vertical and 2 for horizontal:";
	    	cin>>inst;
	    	cout<<"Enter shear parameter value:";
	    	cin >>para;
	    	if(inst==1){
	    		final=shearbynear(image,para,'v');
	    	}
	    	else if(inst==2){
	    		final=shearbynear(image,para,'h');
	    	}
	    	else{
	    		return -1;
	    	}
    	}
    	else if(inst==2){
    		float para;
    		cout<<"Enter 1 for vertical and 2 for horizontal:";
	    	cin>>inst;
	    	cout<<"Enter shear parameter value:";
	    	cin >>para;
	    	if(inst==1){
	    		final=shearbybilinear(image,para,'v');
	    	}
	    	else if(inst==2){
	    		final=shearbybilinear(image,para,'h');
	    	}
	    	else{
	    		return -1;
	    	}
    	}
    	else{
    		cout<<"invalid\n";
    		return -1;
    	}
    }
    else if(inst==8){
    	image=increasewindow(image,max(image.rows,image.cols)/2);
    	cout<<"Enter 1 for nearest neighbour and 2 for bilinear:";
    	cin>>inst;
    	if(inst==1){
    		double degrees;
    		cout<<"Enter degrees in anticlockwise direction:";
    		cin>>degrees;
    		final=rotatebynear(image,degrees);
    	}
    	else if(inst==2){
    		double degrees;
    		cout<<"Enter degrees in anticlockwise direction:";
    		cin>>degrees;
    		final=rotatebybilinear(image,degrees);
    	}
    	else{
    		cout<<"invalid\n";
    		return -1;
    	}
    }
    else if(inst==9){
    	final=histogramequal(image);
    }
    else if(inst==10){
    	final=adaptivehist(image);
    }
    else if(inst==11){
    	string result;
    	cout<<"Enter image name with whom to be matched:";
    	cin>>result;
    	Mat toget = imread(result,0);
    	if(toget.empty())
    	{
        	cout<< "Cannot open image file "<<imagename<<endl;
        	return -1;
    	}
    	final=histogrammatching(image,toget);
    }
    else if(inst==12){
    	final=tie(image);
    }
    else if(inst==13){
    	final=piecewise(image);
    }
    else{
    	cout<<"invalid instruction\n";
    	return -1;
    }
    //resize(image,temp,Size(900,900),2,4,INTER_NEAREST);
    //
    //cout<<error(inbuilt,temp)<<endl;
    imshow("original",image);
    imshow("new",final);
    waitKey(0);
    return 0;
}
Mat increasewindow(Mat input,int size){
	Mat output(2*size+input.rows,2*size+input.cols,CV_8U,0.0);
	for (int i=0;i<output.rows;i++){
    	for (int j=0;j<output.cols;j++){
    		if(i-(size)>=0&&j-(size)>=0&&(i-size)<input.rows&&(j-size)<input.cols){
    			output.at<uchar>(i,j)=input.at<uchar>(i-(size),j-(size));
    		}
    	}
    }
    return output;
}

Mat negative(Mat input){
	Mat output(input.rows,input.cols,CV_8U);
	for (int i=0;i<input.rows;i++){
    	for (int j=0;j<input.cols;j++){
    		output.at<uchar>(i,j)=255-input.at<uchar>(i,j);
    	}
    }
    return output;
}

Mat resize_by_near(Mat input,float rowscale,float colscale){
	Mat output(input.rows*rowscale,input.cols*colscale,CV_8U);
	for (int i=0;i<output.rows;i++){
    	for (int j=0;j<output.cols;j++){
    		float x=(i+0.5)*((float)(1/rowscale))+0.5;
    		float y=(j+0.5)*((float)(1/colscale))+0.5;
    		int newi,newj;
    		if(x>=(int)x+0.5){
    			newi=int(x);
    		}
    		else{
    			newi=int(x)-1;
    		}
    		if(y>=(int)y+0.5){
    			newj=int(y);
    		}
    		else{
    			newj=int(y)-1;
    		}
    		output.at<uchar>(i,j)=input.at<uchar>(newi,newj);
    	}
    }
    Mat inbuilt;
    //resize(input,inbuilt,Size(colscale*input.cols,rowscale*input.rows),((float)1/rowscale),((float)1/colscale),INTER_NEAREST);
    cout<<error(output,inbuilt);
    return output;
}


//I have assumed wrap around condition
Mat resize_by_bilinear(Mat input,float rowscale,float colscale){
	Mat output(input.rows*rowscale,input.cols*colscale,CV_8U);
	for (int i=0;i<output.rows;i++){
    	for (int j=0;j<output.cols;j++){
    		float belowx,belowy;
    		float x=((float)(i+0.5)*((float)(1/rowscale)));
    		if(x>=(int)x+0.5){
    			belowx=(int)x+0.5;
    		}
    		else{
    			belowx=(int)x-0.5;
    		}
    		float diffx=x-belowx;

    		float y=((float)(j+0.5)*((float)(1/colscale)));
    		if(y>=(int)y+0.5){
    			belowy=(int)y+0.5;
    		}
    		else{
    			belowy=(int)y-0.5;
    		}
    		float diffy=y-belowy;

    		int bx=(int)belowx;
    		int by=(int)belowy;
    		output.at<uchar>(i,j)=((1-diffx)*(1-diffy))*input.at<uchar>(bx%input.rows,by%input.cols)+(diffx*diffy)*input.at<uchar>((bx+1)%input.rows,(by+1)%input.cols)+((diffx)*(1-diffy))*input.at<uchar>((bx+1)%input.rows,by%input.cols)+((1-diffx)*diffy)*input.at<uchar>(bx%input.rows,(by+1)%input.cols);
    	}
    }
    return output;
}


Mat logtransformation(Mat input){
	Mat output(input.rows,input.cols,CV_8U);
	int c;
	cout<<"Enter the value of c(only integer):";
	cin>>c;
	for (int i=0;i<input.rows;i++){
    	for (int j=0;j<input.cols;j++){
    		output.at<uchar>(i,j)=c*log(input.at<uchar>(i,j)+1);
    	}
    }
    return output;
}


Mat gammatransformation(Mat input){
	Mat output(input.rows,input.cols,CV_8U);
	float gamma;
	float c;
	cout<<"Enter the value of gamma:";
	cin>>gamma;
	cout<<"Enter the value of c:";
	cin>>c;
	for (int i=0;i<input.rows;i++){
    	for (int j=0;j<input.cols;j++){
    		output.at<uchar>(i,j)=c*pow((input.at<uchar>(i,j)),gamma);
    	}
    }
    return output;
}

Mat bitplane(Mat input,int bit){
	Mat output(input.rows,input.cols,CV_8U);
	int mul=1<<(bit-1);
	for (int i=0;i<input.rows;i++){
    	for (int j=0;j<input.cols;j++){
    		output.at<uchar>(i,j)=(input.at<uchar>(i,j)&mul);
    	}
    }
    return output;
}
Mat bitplaneslicing(Mat input){
	Mat output(input.rows,input.cols,CV_8U,0.0);
	Mat temp(input.rows,input.cols,CV_8U);
	while(1){
		int bit;
		cout<<"Enter the bitplane(0 to exit):";
		cin>>bit;
		if(bit==0){
			break;
		}
		else{
			temp=bitplane(input,bit);
			for (int i=0;i<input.rows;i++){
    			for (int j=0;j<input.cols;j++){
    				output.at<uchar>(i,j)+=temp.at<uchar>(i,j);
    			}
    		}
		}
	}
	return output;
}

Mat translate(Mat input,int x,int y){
	Mat output(input.rows,input.cols,CV_8U,0.0);
	for (int i=0;i<output.rows;i++){
    	for (int j=0;j<output.cols;j++){
    		if(i-x>=0&&i-x<input.rows&&j-y>=0&&j-y<input.cols){
    			output.at<uchar>(i,j)=input.at<uchar>(i-x,j-y);
    		}
    		
    	}
    }
	return output;

}
Mat createmat(int row,int col){
    Mat output(row,col,CV_8U,0.0);
    return output;
}
/*For shear,I have assumed only positive shearing(window size is correct for positive).*/
Mat shearbynear(Mat input,float s,char d){
	Mat output;
    if(d=='h')output=createmat(input.rows,input.cols+abs(s)*(input.cols));
    else if(d=='v')output=createmat(abs(s)*(input.rows)+input.rows,input.cols);
	for (int i=0;i<output.rows;i++){
    	for (int j=0;j<output.cols;j++){
    		if(d=='h'){
    			int y=j-s*(i+(0.5));
    			int x=i;
    			if(x>=0&&x<input.rows&&y>=0&&y<input.cols){
    			output.at<uchar>(i,j)=input.at<uchar>(x,y);
    			}
    		}
    		if(d=='v'){
    			int y=j;
    			int x=i-s*(j+0.5);	
    			if(x>=0&&x<input.rows&&y>=0&&y<input.cols){
    			output.at<uchar>(i,j)=input.at<uchar>(x,y);
    			}
    		}
    		
    	}
    }
	return output;

}
Mat shearbybilinear(Mat input,float s,char d){
	Mat output;
	if(d=='h')output=createmat(input.rows,input.cols+abs(s)*(input.cols));
    else if(d=='v')output=createmat(abs(s)*(input.rows)+input.rows,input.cols);

	float belowx,belowy;
	for (int i=0;i<output.rows;i++){
    	for (int j=0;j<output.cols;j++){
    		if(d=='h'){
    			float y=j-s*(i+(0.5));
    			int x=i;

    			if(y>=(int)y+0.5){
	    			belowy=(int)y+0.5;
	    		}
	    		else{
	    			belowy=(int)y-0.5;
	    		}
	    		float diffy=y-belowy;
	    		int by=(int)belowy;
    			if(x>=0&&x<input.rows&&by>=0&&by<input.cols){
    				output.at<uchar>(i,j)=(1-diffy)*input.at<uchar>(x,by)+(diffy)*input.at<uchar>(x,(by+1)%input.cols);
    			}
    		}
    		if(d=='v'){
    			int y=j;
    			float x=i-s*(j+0.5);
    			if(x>=(int)x+0.5){
    				belowx=(int)x+0.5;
    			}
    			else{
    				belowx=(int)x-0.5;
    			}
    			float diffx=x-belowx;
    			int bx=(int)belowx;	
    			if(bx>=0&&bx<input.rows&&y>=0&&y<input.cols){
    				output.at<uchar>(i,j)=(1-diffx)*input.at<uchar>(bx%input.rows,y)+(diffx)*input.at<uchar>((bx+1)%input.rows,y);
    			}
    		}
    		
	 	}
	 	//output=translate(output,output.rows/2,output.cols/2);
    }
	return output;

}

Mat rotatebynear(Mat input,double theta){
	theta=theta*pi/180;
	int size=max(input.rows,input.cols);
	Mat output(input.rows,input.cols,CV_8U,0.0);
	for (int i=0;i<output.rows;i++){
    	for (int j=0;j<output.cols;j++){
    		float tempi=(i+0.5)-(float)output.rows/2;
    		float tempj=(j+0.5)-(float)output.cols/2;
    		float x=cos(theta)*tempi+sin(theta)*tempj;
    		x=x+(float)output.rows/2;
    		float y=cos(theta)*tempj-sin(theta)*tempi;
    		y=y+(float)output.cols/2;
    		x=(int)x;
    		y=(int)y;
    		if(x>=0&&x<input.rows&&y>=0&&y<input.cols){
    			output.at<uchar>(i,j)=input.at<uchar>(x,y);
    		}
    	}
    }
	return output;

}

Mat rotatebybilinear(Mat input,double theta){
	theta=theta*pi/180;
	int size=max(input.rows,input.cols);
	Mat output(input.rows,input.cols,CV_8U,0.0);
	for (int i=0;i<output.rows;i++){
    	for (int j=0;j<output.cols;j++){
    		float tempi=(i+0.5)-(float)output.rows/2;
    		float tempj=(j+0.5)-(float)output.cols/2;
    		float x=cos(theta)*tempi+sin(theta)*tempj;
    		x=x+(float)output.rows/2;
    		float y=cos(theta)*tempj-sin(theta)*tempi;
    		y=y+(float)output.cols/2;
    		float belowx,belowy;
    		if(x>=(int)x+0.5){
    			belowx=(int)x+0.5;
    		}
    		else{
    			belowx=(int)x-0.5;
    		}
    		float diffx=x-belowx;
    		if(y>=(int)y+0.5){
    			belowy=(int)y+0.5;
    		}
    		else{
    			belowy=(int)y-0.5;
    		}
    		float diffy=y-belowy;
    		int bx=(int)belowx;
    		int by=(int)belowy;
    		
    		if(bx>=0&&bx<input.rows&&by>=0&&by<input.cols){
    			output.at<uchar>(i,j)=((1-diffx)*(1-diffy))*input.at<uchar>(bx%input.rows,by%input.cols)+(diffx*diffy)*input.at<uchar>((bx+1)%input.rows,(by+1)%input.cols)+((diffx)*(1-diffy))*input.at<uchar>((bx+1)%input.rows,by%input.cols)+((1-diffx)*diffy)*input.at<uchar>(bx%input.rows,(by+1)%input.cols);
    		}
    	}
    }
	return output;

}

Mat histogramequal(Mat input){
	Mat output(input.rows,input.cols,CV_8U,0.0);
	int hist[256]={0};
	for (int i=0;i<input.rows;i++){
		for(int j=0;j<input.cols;j++){
			hist[input.at<uchar>(i,j)]++;
		}
	}
	for (int i=1;i<256;i++){
		hist[i]+=hist[i-1];
	}
	for (int i=0;i<256;i++){
		hist[i]=(hist[i]*255)/(input.rows*input.cols);
	}
	for (int i=0;i<input.rows;i++){
		for(int j=0;j<input.cols;j++){
			output.at<uchar>(i,j)=hist[input.at<uchar>(i,j)];
		}
	}
	return output;

}

Mat adaptivehist(Mat input){
	Mat output(input.rows,input.cols,CV_8U,0.0);
	int window=15;
	for(int i=0;i<input.rows;i++){
		for (int j=0;j<input.cols;j++){
			Mat temp(2*window+1,2*window+1,CV_8U,0.0);
			for(int k=i-window;k<=i+window;k++){
				for(int l=j-window;l<=j+window;l++){
					if(k>=0&&k<input.rows&&l>=0&&l<input.cols){
						temp.at<uchar>(k-(i-window),l-(j-window))=input.at<uchar>(k,l);					
					}
				}
			}
			temp=histogramequal(temp);
			output.at<uchar>(i,j)=temp.at<uchar>(window,window);
		}
	}
	return output;
}

Mat histogrammatching(Mat input,Mat result){
	Mat output(input.rows,input.cols,CV_8U,0.0);
	int hist[256]={0};
	for (int i=0;i<input.rows;i++){
		for(int j=0;j<input.cols;j++){
			hist[input.at<uchar>(i,j)]++;
		}
	}
	for (int i=1;i<256;i++){
		hist[i]+=hist[i-1];
	}
	for (int i=0;i<256;i++){
		hist[i]=(hist[i]*255)/(input.rows*input.cols);
	}
	int finalhist[256]={0};
	for (int i=0;i<result.rows;i++){
		for(int j=0;j<result.cols;j++){
			finalhist[result.at<uchar>(i,j)]++;
		}
	}
	for (int i=1;i<256;i++){
		finalhist[i]+=finalhist[i-1];
	}
	for (int i=0;i<256;i++){
		finalhist[i]=(finalhist[i]*255)/(result.rows*result.cols);
	}
	int revhist[256]={-1};
	for(int i=0;i<256;i++){
		revhist[finalhist[i]]=i;
	}
	if(revhist[0]==-1){
		for(int i=1;i<256;i++){
			if(revhist[i]!=-1){
				revhist[0]=revhist[i];
				break;
			}
		}
	}
	for(int i=1;i<256;i++){
		if(revhist[i]==-1){
			revhist[i]=revhist[i-1];
		}
	}
	for (int i=0;i<input.rows;i++){
		for (int j=0;j<input.cols;j++){
			output.at<uchar>(i,j)=revhist[hist[input.at<uchar>(i,j)]];
		}
	}
    cout<<error(result,output);
	return output;
}

Mat tie(Mat image){
	Mat input=image;
	Mat x(4,1,CV_32F);
	Mat y(4,1,CV_32F);
	Mat nx(4,1,CV_32F);
	Mat ny(4,1,CV_32F);
	Mat X(8,1,CV_32F);
	Mat c(8,1,CV_32F);
	for(int i=0;i<4;i++){
		cout<<"Enter the old coordinates and corresponding new coordinates separated by spaces("<<i<<"):";
		cin>>x.at<float>(i,0)>>y.at<float>(i,0)>>nx.at<float>(i,0)>>ny.at<float>(i,0);
	}
	Mat trans(8,8,CV_32F,0.0);
	for (int i=0;i<4;i++){
		trans.at<float>(2*i+0,0)=x.at<float>(i,0);
		trans.at<float>(2*i+0,1)=y.at<float>(i,0);
		trans.at<float>(2*i+0,2)=x.at<float>(i,0)*y.at<float>(i,0);
		trans.at<float>(2*i+0,3)=1;
		trans.at<float>(2*i+1,4)=x.at<float>(i,0);
		trans.at<float>(2*i+1,5)=y.at<float>(i,0);
		trans.at<float>(2*i+1,6)=x.at<float>(i,0)*y.at<float>(i,0);
		trans.at<float>(2*i+1,7)=1;
	}
	for(int i=0;i<4;i++){
		X.at<float>(2*i,0)=nx.at<float>(i,0);
		X.at<float>(2*i+1,0)=ny.at<float>(i,0);
	}
	Mat inverse=trans.inv();
	c=inverse*X;
	Mat output(input.rows,input.cols,CV_8U,0.0);
	for (int i=0;i<output.rows;i++){
		for (int j=0;j<output.cols;j++){
			float newi=(c.at<float>(0,0)*i+c.at<float>(1,0)*j+c.at<float>(2,0)*i*j+c.at<float>(3,0));
			float newj=(c.at<float>(4,0)*i+c.at<float>(5,0)*j+c.at<float>(6,0)*i*j+c.at<float>(7,0));
			if(newi>=0&&newi<input.rows&&newj>=0&&newj<input.cols){
				output.at<uchar>(i,j)=input.at<uchar>(newi,newj);
			}
		}
	}
	return output;
}
Mat piecewise(Mat input){
	Mat output(input.rows,input.cols,CV_8U,0.0);
	int x1,x2,y1,y2;
	cout<<"enter the 2 coordinates separated by space:";
	cin>>x1>>y1>>x2>>y2;
	float slope1,slope2,slope3;
	if(x1!=0)slope1=(float)y1/x1;
	if(x1!=x2){
		slope2=(float)((y2-y1)/(x2-x1));
	}
	if(x2!=255){
		slope3=(float)((255-y2)/(255-x2));
	}
	for (int i=0;i<input.rows;i++){
		for (int j=0;j<input.cols;j++){
			//cout<<"1\n";
			if(input.at<uchar>(i,j)<x1){
				output.at<uchar>(i,j)=(slope1*input.at<uchar>(i,j));
			}
			else if(input.at<uchar>(i,j)==x1){
				output.at<uchar>(i,j)=y1;
			}
			else if(input.at<uchar>(i,j)==x2){
				output.at<uchar>(i,j)=y2;
			}
			else if(input.at<uchar>(i,j)>x1&&input.at<uchar>(i,j)<x2){
				output.at<uchar>(i,j)=(y1+slope2*(input.at<uchar>(i,j)-x1));
			}
			else if(input.at<uchar>(i,j)>x2&&input.at<uchar>(i,j)<=255){
				output.at<uchar>(i,j)=(y2+slope3*(input.at<uchar>(i,j)-x2));
			}
			//cout<<(int)output.at<uchar>(i,j)<<" "<<(int)input.at<uchar>(i,j)<<endl;
		}
	}
	return output;
}



//root mean square error
float error(Mat input,Mat output){
	float err=0.0;
	for (int i=0;i<input.rows;i++){
		for (int j=0;j<input.cols;j++){
			err+=pow((input.at<uchar>(i,j)-output.at<uchar>(i,j)),2);
		}
	}
	err=err/(output.rows*output.cols);
	err=sqrt(err);
	return err;
}
