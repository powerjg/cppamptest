//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"

namespace cppamptest
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

	private:

		const int M = 1024;
		const int N = 1024;
		const int W = 512;

		void Button_Click_CPU(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Button_Click_ACC(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
		void mm_cpu(std::vector<float> &vA, std::vector<float> &vB, std::vector<float> &vC, const int N, const int M, const int W);
		void mm(std::vector<float> &vA, std::vector<float> &vB, std::vector<float> &vC, const int N, const int M, const int W);


		std::vector<float> vA;
		std::vector<float> vB;
		std::vector<float> vC;
	};
}
