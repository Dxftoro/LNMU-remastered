#pragma once
#include<iostream>
#include<iomanip>
#include<thread>
#include<chrono>
using namespace std;
class TheGreatHelper {
public:
	unsigned long double factl(int x) {
		unsigned long int cer1 = 0;
		double res_cloud = 1;
		while (cer1 < x) {
			cer1++;
			res_cloud = res_cloud * cer1;
		}
		return res_cloud;
	}
public:
	int code(char symbol) {
		return static_cast<int>(symbol);
	}
public:
	int valm(int value) {
		if (value >= 0) {
			value = value;
			return value;
		}
		if (value < 0) {
			value = value * -1;
			return value;
		}
	}
public:
	double tg(double tgX) {
		tgX = sin(tgX) / cos(tgX);
		return tgX;
	}
public:
	double ctg(double ctgX) {
		ctgX = 1 / tg(ctgX);
		return ctgX;
	}
public:
	void author(string auth) {
		cout << "Program autor: " + auth << endl;
	}
public:
	void wait_ms(int time) {
		this_thread::sleep_for(std::chrono::milliseconds(time));
	}
public:
	void wait_s(int time) {
		this_thread::sleep_for(std::chrono::seconds(time));
	}
public:
	void wait_m(int time) {
		this_thread::sleep_for(std::chrono::minutes(time));
	}
};