//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

#include <amp.h>
#include <thread>
#include <chrono>
#include <random>
#include <string>
#include <sstream>
#include <omp.h>

using namespace concurrency;

using namespace cppamptest;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	InitializeComponent();

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(0, 1);

	for (int i = 0; i < M*W; i++)
	{
		vA.push_back(dis(gen));
	}
	for (int i = 0; i < N*W; i++)
	{
		vB.push_back(dis(gen));
	}
	for (int i = 0; i < N*M; i++)
	{
		vC.push_back(dis(gen));
	}

	std::vector<accelerator> accs = accelerator::get_all();

	std::wstringstream ss;
	for (auto acc : accs) {
		ss << "Accelerator: " << acc.description << " Default? " << acc.default_accelerator << "\n";
	}

	ss << "M: " << M << ". N: " << N << ". W: " << W << "\n";

	Platform::String^ s2 = ref new String(ss.str().c_str());
	accel_tb->Text = s2;
}


void cppamptest::MainPage::Button_Click_CPU(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto begin = std::chrono::high_resolution_clock::now();
	//for (int i = 0; i < 10; i++) {
		mm_cpu(vA, vB, vC, M, N, W);
	//}
	auto end = std::chrono::high_resolution_clock::now();

	std::wstringstream ss;
	ss << "Done! Took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms";

	Platform::String^ s2 = ref new String(ss.str().c_str());
	result->Text = s2;
}

void cppamptest::MainPage::Button_Click_ACC(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto begin = std::chrono::high_resolution_clock::now();
	//for (int i = 0; i < 10; i++) {
	mm(vA, vB, vC, M, N, W);
	//}
	auto end = std::chrono::high_resolution_clock::now();

	std::wstringstream ss;
	ss << "Done! Took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms";

	Platform::String^ s2 = ref new String(ss.str().c_str());
	result2->Text = s2;
}

void cppamptest::MainPage::mm_cpu(std::vector<float> &vA, std::vector<float> &vB, std::vector<float> &vC, const int N, const int M, const int W)
{
	array_view<const float, 2> a(M, W, vA), b(W, N, vB);
	array_view<float, 2> c(M, N, vC);

#pragma omp parallel for
	for (int i = 0; i < M; i++) {
#pragma omp parallel for
		for (int j = 0; j < N; j++) {
			float sum = 0;
			for (int k = 0; k < W; k++) {
				sum += a(i, k) * b(k, j);
			}
			c(i,j) = sum;
		}
	}
	c.synchronize();
}

void cppamptest::MainPage::mm(std::vector<float> &vA, std::vector<float> &vB, std::vector<float> &vC, const int N, const int M, const int W)
{
	array_view<const float, 2> a(M, W, vA), b(W, N, vB);
	array_view<float, 2> c(M, N, vC);
	a.refresh();
	b.refresh();
	c.discard_data();

	parallel_for_each(c.extent, [=](index<2> idx) restrict(amp)
	{
		float sum = 0;
		for (int k = 0; k < W; k++) {
			sum += a(idx[0], k) * b(k, idx[1]);
		}
		c[idx] = sum;
	});
	c.synchronize();
}