#include<iostream>
#include<CKernal.h>
#include<CMySql.h>
#include<mysocket.h>
int main() {
	CKernal* kernal = new CKernal;
	kernal->startNet();
	while (1) {
		sleep(5);
		cout << "server runing..." << endl;
	}
	return 0;
}
