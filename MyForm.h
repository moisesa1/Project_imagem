#pragma once
#include<iostream>
#include "opencv2\highgui.hpp"
#include "opencv2\imgproc.hpp"
#include <msclr\marshal_cppstd.h>
#include <fstream>
#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
//Load Images
#define STB_IMAGE_IMPLEMENTATION
// Write Images
#define STB_IMAGE_WRITE_IMPLEMENTATION

//#define STBIR_INCLUDE_STB_IMAGE_RESIZE_H

#include "include/stb_image.h"
//#include "include/stb_image_resize.h"
#include "include/stb_image_write.h"

using namespace std;

using namespace cv;




// Convert Image to Gray at CPU
void ConvertImageToGrayCpu(unsigned char* imageRGBA, int width, int height) {


	struct Pixel
	{
		unsigned char red, green, blue;
	};
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {

			/*  Color to Grayscale Equation
			 Ylinear = 0.2126RLinear + 0.7152GLiners +0.0722BLiners */
			Pixel* ptrPixel = (Pixel*)&imageRGBA[y * width * 3 +3  * x];
			unsigned char pixelValue = (unsigned char)(ptrPixel->red * 0.2126f + ptrPixel->green * 0.7152f + ptrPixel->blue * 0.0722f);

			//float pixelValue = ptrPixel->red * 0.2126f + ptrPixel->green * 0.7152f + ptrPixel-> blue * 0.0722f;
			//unsigned char pixelValue = pixelValue;
			ptrPixel->red = pixelValue;
			ptrPixel->green = pixelValue;
			ptrPixel->blue = pixelValue;
			//ptrPixel->alpha = 255;

		}
	}

}
void std_to_box(int boxes[], float sigma, int n)
{
	// ideal filter width
	float wi = std::sqrt((12 * sigma * sigma / n) + 1);
	int wl = std::floor(wi);
	if (wl % 2 == 0) wl--;
	int wu = wl + 2;

	float mi = (12 * sigma * sigma - n * wl * wl - 4 * n * wl - 3 * n) / (-4 * wl - 4);
	int m = std::round(mi);

	for (int i = 0; i < n; i++)
		boxes[i] = ((i < m ? wl : wu) - 1) / 2;
}

void horizontal_blur(float* in, float* out, int w, int h, int r)
{
	float iarr = 1.f / (r + r + 1);
#pragma omp parallel for
	for (int i = 0; i < h; i++)
	{
		int ti = i * w, li = ti, ri = ti + r;
		float fv = in[ti], lv = in[ti + w - 1], val = (r + 1) * fv;

		for (int j = 0; j < r; j++) val += in[ti + j];
		for (int j = 0; j <= r; j++) { val += in[ri++] - fv; out[ti++] = val * iarr; }
		for (int j = r + 1; j < w - r; j++) { val += in[ri++] - in[li++]; out[ti++] = val * iarr; }
		for (int j = w - r; j < w; j++) { val += lv - in[li++]; out[ti++] = val * iarr; }
	}
}
void total_blur(float* in, float* out, int w, int h, int r)
{
	float iarr = 1.f / (r + r + 1);
#pragma omp parallel for
	for (int i = 0; i < w; i++)
	{
		int ti = i, li = ti, ri = ti + r * w;
		float fv = in[ti], lv = in[ti + w * (h - 1)], val = (r + 1) * fv;
		for (int j = 0; j < r; j++) val += in[ti + j * w];
		for (int j = 0; j <= r; j++) { val += in[ri] - fv; out[ti] = val * iarr; ri += w; ti += w; }
		for (int j = r + 1; j < h - r; j++) { val += in[ri] - in[li]; out[ti] = val * iarr; li += w; ri += w; ti += w; }
		for (int j = h - r; j < h; j++) { val += lv - in[li]; out[ti] = val * iarr; li += w; ti += w; }
	}
}

void box_blur(float*& in, float*& out, int w, int h, int r)
{
	std::swap(in, out);
	horizontal_blur(out, in, w, h, r);
	total_blur(in, out, w, h, r);
	// Note to myself : 
	// here we could go anisotropic with different radiis rx,ry in HBlur and TBlur
}
void fast_gaussian_blur(float*& in, float*& out, int w, int h, float sigma)
{
	// sigma conversion to box dimensions
	int boxes[3];
	std_to_box(boxes, sigma, 3);
	box_blur(in, out, w, h, boxes[0]);
	box_blur(out, in, w, h, boxes[1]);
	box_blur(in, out, w, h, boxes[2]);
}

namespace Project2 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace cv;
	
	
	// Namespace to nullify use of cv::function(); syntax
	using namespace std;
	/// <summary>
	/// Sumário para MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO: Adicione o código do construtor aqui
			//
		}

	protected:
		/// <summary>
		/// Limpar os recursos que estão sendo usados.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^ load_img;
	protected:

	private: System::Windows::Forms::PictureBox^ img_output;
	private: System::Windows::Forms::PictureBox^ img_input;
	private: System::Windows::Forms::Button^ close_img;
	private: System::Windows::Forms::Button^ gray;


	private: System::Windows::Forms::Button^ button2;

	private: System::Windows::Forms::Button^ exit;
	private: System::Windows::Forms::TrackBar^ constrast_level;
	private: System::Windows::Forms::Button^ blur;


	protected:

	private:
		/// <summary>
		/// Variável de designer necessária.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Método necessário para suporte ao Designer - não modifique 
		/// o conteúdo deste método com o editor de código.
		/// </summary>
		void InitializeComponent(void)
		{
			this->load_img = (gcnew System::Windows::Forms::Button());
			this->img_output = (gcnew System::Windows::Forms::PictureBox());
			this->img_input = (gcnew System::Windows::Forms::PictureBox());
			this->close_img = (gcnew System::Windows::Forms::Button());
			this->gray = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->exit = (gcnew System::Windows::Forms::Button());
			this->constrast_level = (gcnew System::Windows::Forms::TrackBar());
			this->blur = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->img_output))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->img_input))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->constrast_level))->BeginInit();
			this->SuspendLayout();
			// 
			// load_img
			// 
			this->load_img->Location = System::Drawing::Point(241, 39);
			this->load_img->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->load_img->Name = L"load_img";
			this->load_img->Size = System::Drawing::Size(87, 38);
			this->load_img->TabIndex = 0;
			this->load_img->Text = L"Load Img";
			this->load_img->UseVisualStyleBackColor = true;
			this->load_img->Click += gcnew System::EventHandler(this, &MyForm::button1_Click);
			// 
			// img_output
			// 
			this->img_output->Location = System::Drawing::Point(704, 144);
			this->img_output->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->img_output->Name = L"img_output";
			this->img_output->Size = System::Drawing::Size(667, 615);
			this->img_output->TabIndex = 2;
			this->img_output->TabStop = false;
			// 
			// img_input
			// 
			this->img_input->Location = System::Drawing::Point(15, 144);
			this->img_input->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->img_input->Name = L"img_input";
			this->img_input->Size = System::Drawing::Size(667, 615);
			this->img_input->TabIndex = 3;
			this->img_input->TabStop = false;
			this->img_input->Click += gcnew System::EventHandler(this, &MyForm::img_input_Click);
			// 
			// close_img
			// 
			this->close_img->Location = System::Drawing::Point(356, 39);
			this->close_img->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->close_img->Name = L"close_img";
			this->close_img->Size = System::Drawing::Size(85, 38);
			this->close_img->TabIndex = 4;
			this->close_img->Text = L"Close";
			this->close_img->UseVisualStyleBackColor = true;
			this->close_img->Click += gcnew System::EventHandler(this, &MyForm::button1_Click_1);
			// 
			// gray
			// 
			this->gray->Location = System::Drawing::Point(166, 97);
			this->gray->Name = L"gray";
			this->gray->Size = System::Drawing::Size(99, 38);
			this->gray->TabIndex = 5;
			this->gray->Text = L"Gray";
			this->gray->UseVisualStyleBackColor = true;
			this->gray->Click += gcnew System::EventHandler(this, &MyForm::button1_Click_2);
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(54, 97);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(92, 42);
			this->button2->TabIndex = 6;
			this->button2->Text = L"Filter1";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &MyForm::button2_Click);
			// 
			// exit
			// 
			this->exit->Location = System::Drawing::Point(1246, 25);
			this->exit->Name = L"exit";
			this->exit->Size = System::Drawing::Size(100, 38);
			this->exit->TabIndex = 7;
			this->exit->Text = L"Exit";
			this->exit->UseVisualStyleBackColor = true;
			this->exit->Click += gcnew System::EventHandler(this, &MyForm::exit_Click);
			// 
			// constrast_level
			// 
			this->constrast_level->Location = System::Drawing::Point(821, 6);
			this->constrast_level->Name = L"constrast_level";
			this->constrast_level->Size = System::Drawing::Size(329, 56);
			this->constrast_level->TabIndex = 8;
			this->constrast_level->Scroll += gcnew System::EventHandler(this, &MyForm::constrast_level_Scroll);
			// 
			// blur
			// 
			this->blur->Location = System::Drawing::Point(291, 97);
			this->blur->Name = L"blur";
			this->blur->Size = System::Drawing::Size(91, 38);
			this->blur->TabIndex = 9;
			this->blur->Text = L"Blur";
			this->blur->UseVisualStyleBackColor = true;
			this->blur->Click += gcnew System::EventHandler(this, &MyForm::blur_Click);
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1425, 784);
			this->Controls->Add(this->blur);
			this->Controls->Add(this->constrast_level);
			this->Controls->Add(this->exit);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->gray);
			this->Controls->Add(this->close_img);
			this->Controls->Add(this->img_input);
			this->Controls->Add(this->img_output);
			this->Controls->Add(this->load_img);
			this->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->Name = L"MyForm";
			this->Text = L"MyForm";
			this->Load += gcnew System::EventHandler(this, &MyForm::MyForm_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->img_output))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->img_input))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->constrast_level))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
		Bitmap^ bmpPicture;
#pragma endregion
	private: System::Void button1_Click(System::Object^ sender, System::EventArgs^ e) {
		// open file dialog   
		OpenFileDialog^ ofd = gcnew OpenFileDialog();
		//insert here the filter if you want
		//ofd->Filter..
		if (ofd->ShowDialog() == System::Windows::Forms::DialogResult::OK) {

			//img_input->Image = Image::FromFile(ofd->FileName);
			//System::String^ strFilename = ofd->FileName;
			
			img_input->ImageLocation = ofd->FileName;
			
		}
	}
	private: System::Void label1_Click(System::Object^ sender, System::EventArgs^ e) {
	}

	private: System::Void MyForm_Load(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void img_input_Click(System::Object^ sender, System::EventArgs^ e) {
		
	}
private: System::Void button1_Click_1(System::Object^ sender, System::EventArgs^ e) {
	img_input->ImageLocation = "";
	img_output->Image = nullptr;
}

	private: System::Void button1_Click_2(System::Object^ sender, System::EventArgs^ e) {
		// let's downscale the image using new  width and heigh
		System::String^ file = img_input->ImageLocation;
		//bmpPicture = gcnew Bitmap(file);
		//System::String^ file = "imagens/apple.jpg";
		//bmpPicture = gcnew Bitmap(bmpPicture, down_width, down_height);
		std::string converted_xyz = msclr::interop::marshal_as< std::string >(file);
		//System::String^ teste = gcnew System::String(converted_xyz);
		int width, height, channels;
		// Loading jpeg files
		const char* c = converted_xyz.c_str();
		unsigned char* img = stbi_load(c, &width, &height, &channels, 3);

		ConvertImageToGrayCpu(img, width, height);
		stbi_write_jpg("images/output.jpg", width, height, 3, img, 100);
		img_output->ImageLocation = "images/output.jpg";
		//Mat src, src_gray, dst;
		
		//Mat frameDetected = imread(converted_xyz);

		//ConvertImageToGrayCpu(img_input->Image, down_width, height);
		//cvtColor(frameDetected, src_gray, COLOR_BGR2GRAY, 1);
		//Mat frameDetected = Mat(width,height, CV_8UC1, *img);

		//System::Drawing::Graphics^ graphics2 = img_output->CreateGraphics();
		//System::IntPtr ptr2(frameDetected.ptr());
		//System::Drawing::Bitmap^ b2 = gcnew System::Drawing::Bitmap(frameDetected.cols,
		//	frameDetected.rows, frameDetected.step, System::Drawing::Imaging::PixelFormat::Format24bppRgb, ptr2);
		//System::Drawing::RectangleF rect2(0, 0, img_output->Width, img_output->Height);
		//graphics2->DrawImage(b2, rect2);
		//do smth
		//myImage->Save(L"C:/heartBUFF.jpg");
		//img_output->Image = System::Drawing::Image::FromFile(L"C:/heartBUFF.jpg");
		//img_output->Image = bmpPicture;
			//resize down
	}
private: System::Void button2_Click(System::Object^ sender, System::EventArgs^ e) {
	//Blur the image with 3x3 Gaussian kernel
	Mat image_blurred_with_3x3_kernel;
	System::String^ file = img_input->ImageLocation;
	bmpPicture = gcnew Bitmap(file);
	
}
private: System::Void openFileDialog1_FileOk(System::Object^ sender, System::ComponentModel::CancelEventArgs^ e) {
}
private: System::Void exit_Click(System::Object^ sender, System::EventArgs^ e) {
	Application::Exit();
}
private: System::Void constrast_level_Scroll(System::Object^ sender, System::EventArgs^ e) {
	//img_output->Controls->Clear();//first Empty the picture box's contents
	//int contrast_threshold = constrast_level->Value;//set the value of variable contrast_threshold to the value of the trackbar
	Mat image, result;//create two Mat objects
}
private: System::Void blur_Click(System::Object^ sender, System::EventArgs^ e) {
	// Image loading
	int width, height, channels;
	unsigned char* image_data = stbi_load("images/output.jpg", &width, &height, &channels, 0);
	std::cout << "Source image: " << width << "x" << height << " (" << channels << ")" << std::endl;
	if (channels < 3)
	{
		std::cout << "Input images must be RGB images." << std::endl;
	}

	// copy data
	int size = width * height;

	// output channels r,g,b
	float* newb = new float[size];
	float* newg = new float[size];
	float* newr = new float[size];

	// input channels r,g,b
	float* oldb = new float[size];
	float* oldg = new float[size];
	float* oldr = new float[size];

	// channels copy r,g,b
	for (int i = 0; i < size; ++i)
	{
		oldb[i] = image_data[channels * i + 0] / 255.f;
		oldg[i] = image_data[channels * i + 1] / 255.f;
		oldr[i] = image_data[channels * i + 2] / 255.f;
	}

	// per channel filter
	fast_gaussian_blur(oldb, newb, width, height, 2);
	fast_gaussian_blur(oldg, newg, width, height, 2);
	fast_gaussian_blur(oldr, newr, width, height, 2);



	// channels copy r,g,b
	for (int i = 0; i < size; ++i)
	{
		image_data[channels * i + 0] = (unsigned char) min(255.f, max(0.f, 255.f * newb[i]));
		image_data[channels * i + 1] = (unsigned char) min(255.f, max(0.f, 255.f * newg[i]));
		image_data[channels * i + 2] = (unsigned char) min(255.f, max(0.f, 255.f * newr[i]));
	}
	
	// save
	const char* output_file = "images/output.jpg";
	std::string file(output_file);
	std::string ext = file.substr(file.size() - 3);
	stbi_write_jpg(output_file, width, height, channels, image_data, 90);
	stbi_image_free(image_data);

	// clean memory
	delete[] newr;
	delete[] newb;
	delete[] newg;
	delete[] oldr;
	delete[] oldb;
	delete[] oldg;
	img_output->ImageLocation = "images/output.jpg";
}
};
}


